#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>

// --- VARIABLES COMPARTIDAS (PUENTE ENTRE NÚCLEOS) ---
volatile float v_lineal = 0.0;
volatile float v_angular = 0.0;
volatile unsigned long ultimo_mensaje = 0; 

String estadoAnterior = "Desconocido"; 

// --- ESTRUCTURAS DE ROS 2 ---
rcl_subscription_t subscriber;
geometry_msgs__msg__Twist msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
TaskHandle_t TareaROS;

// --- CONFIGURACIÓN DE PINES (TB6612FNG) ---
const int AIN1 = 21;   
const int AIN2 = 22;   
const int BIN1 = 18;   
const int BIN2 = 5;    
const int STBY = 4;    
const int PWMA = 23;   
const int PWMB = 19;   

// --- VARIABLES DE VELOCIDAD Y CALIBRACIÓN ---
int velocidadMax = 255;
int disminuir = 70;
int A = 0; 
int B = 0; 

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// --- FUNCIONES DE MOVIMIENTO (TU CÓDIGO BASE) ---
void adelante(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); analogWrite(PWMB, velocidadB);
}

void retrocede(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); analogWrite(PWMB, velocidadB);
}

void derecha(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); analogWrite(PWMB, velocidadB);
}

void izquierda(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); analogWrite(PWMB, velocidadB);
}

void izquierda_arriba(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); analogWrite(PWMB, velocidadB);
}

void derecha_arriba(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); analogWrite(PWMB, velocidadB);
}

void derecha_abajo(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); analogWrite(PWMB, velocidadB);
}

void izquierda_abajo(int velocidadA, int velocidadB) {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); analogWrite(PWMA, velocidadA);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); analogWrite(PWMB, velocidadB);
}

void detente() {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW); analogWrite(PWMA, 0);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW); analogWrite(PWMB, 0);
}

// --- CALLBACK ---
void subscription_callback(const void * msgin) {
  const geometry_msgs__msg__Twist * twist_msg = (const geometry_msgs__msg__Twist *)msgin;
  v_lineal = twist_msg->linear.x;
  v_angular = twist_msg->angular.z;
  ultimo_mensaje = millis(); 
}

// --- TAREA DEL CORE 0 ---
void tarea_micro_ros(void * pvParameters) {
  char ssid[] = "Fernandito";
  char password[] = "87654321";
  char agent_ip[] = "192.168.137.128"; 
  size_t agent_port = 8888;

  set_microros_wifi_transports(ssid, password, agent_ip, agent_port);
  delay(2000);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "robot_soccer_node", "", &support);

  rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
    "cmd_vel");

  rclc_executor_init(&executor, &support.context, 1, &allocator);
  rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA);

  for(;;) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Robot Soccer en espera de conexion micro-ROS...");

  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT); pinMode(PWMA, OUTPUT); pinMode(PWMB, OUTPUT);

  digitalWrite(STBY, HIGH); 
  detente();

  xTaskCreatePinnedToCore(tarea_micro_ros, "TareaROS", 10000, NULL, 1, &TareaROS, 0);
}

// --- LOOP PRINCIPAL (LÓGICA MEJORADA) ---
void loop() {
  unsigned long tiempoActual = millis();
  int velocidadA = velocidadMax - A;
  int velocidadB = velocidadMax - B;
  
  String estadoActual = "";

  if (tiempoActual - ultimo_mensaje > 500) {
    estadoActual = "DETENIDO (Sin conexion o inactivo)";
    detente();
  } 
  else {
    // 1. MOVIMIENTOS RECTOS
    if (v_lineal > 0.0 && v_angular == 0.0) {
      estadoActual = "AVANZANDO HACIA ADELANTE";
      adelante(velocidadA, velocidadB);
    } 
    else if (v_lineal < 0.0 && v_angular == 0.0) {
      estadoActual = "RETROCEDIENDO";
      retrocede(velocidadA, velocidadB);
    } 
    // 2. GIROS EN SU PROPIO EJE
    else if (v_lineal == 0.0 && v_angular > 0.0) {
      estadoActual = "GIRANDO EN SU EJE A LA IZQUIERDA";
      izquierda(velocidadA, velocidadB);
    } 
    else if (v_lineal == 0.0 && v_angular < 0.0) {
      estadoActual = "GIRANDO EN SU EJE A LA DERECHA";
      derecha(velocidadA, velocidadB);
    } 
    // 3. INCLINACIONES (MOVIMIENTOS EN DIAGONAL / CURVAS)
    else if (v_lineal > 0.0 && v_angular < 0.0) {
      estadoActual = "AVANZANDO CON INCLINACION A LA DERECHA";
      derecha_arriba((velocidadMax - A), (velocidadMax - disminuir - B));
    }
    else if (v_lineal > 0.0 && v_angular > 0.0) {
      estadoActual = "AVANZANDO CON INCLINACION A LA IZQUIERDA";
      izquierda_arriba((velocidadMax - disminuir - A), (velocidadMax - B));
    }
    else if (v_lineal < 0.0 && v_angular < 0.0) {
      // En ROS, ir en reversa y girar cambia la lógica angular, cruzamos los controles
      estadoActual = "RETROCEDIENDO CON INCLINACION A LA DERECHA";
      derecha_abajo((velocidadMax - disminuir - A), (velocidadMax - B));
    }
    else if (v_lineal < 0.0 && v_angular > 0.0) {
      estadoActual = "RETROCEDIENDO CON INCLINACION A LA IZQUIERDA";
      izquierda_abajo((velocidadMax - A), (velocidadMax - disminuir - B));
    }
    // 4. DETENIDO
    else {
      estadoActual = "DETENIDO (Comando cero)";
      detente();
    }
  }

  // --- IMPRESIÓN EN EL MONITOR SERIE ---
  if (estadoActual != estadoAnterior) {
    Serial.print("Accion de los motores: ");
    Serial.println(estadoActual);
    estadoAnterior = estadoActual;
  }

  delay(10);
}