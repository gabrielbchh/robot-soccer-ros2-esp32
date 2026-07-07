🤖 Robot Soccer con ESP32 y ROS 2 (WSL)
¡Bienvenido al repositorio oficial del Robot Soccer! En este proyecto aprenderás a controlar un robot físico basado en un microcontrolador ESP32 utilizando ROS 2 Humble ejecutado desde Windows a través de WSL (Ubuntu).
Este tutorial está diseñado para equipos que ya tienen WSL y ROS 2 instalados en sus computadoras.
📑 Índice
Paso 1: Configurar la Red en Windows (Modo Espejo)
Paso 2: Preparación en Ubuntu
Paso 3: Hardware y Código del ESP32
Paso 4: ¡A Jugar! (Ejecución)
🧱 Paso 1: Configurar la Red en Windows (Modo Espejo)
Por defecto, WSL crea una red virtual aislada. Para que tu ESP32 pueda comunicarse con Ubuntu a través de tu router WiFi, debemos unificar las direcciones IP.
En Windows, presiona Windows + R, escribe %USERPROFILE% y presiona Enter.
Crea un nuevo documento de texto y nómbralo exactamente .wslconfig (borra el .txt).
Abre el archivo y pega este código:
[wsl2]
networkingMode=mirrored



Abre PowerShell y reinicia WSL con este comando:
wsl --shutdown



💻 Paso 2: Preparación en Ubuntu
Obtener la IP del Servidor: Abre tu terminal de Ubuntu y ejecuta:
hostname -I


Anota la dirección IP que aparece (ej. 192.168.18.23). Esta es la dirección que el ESP32 buscará en la red.
Instalar el Control por Teclado: Ejecuta este comando para instalar el paquete oficial de teleoperación:
sudo apt update
sudo apt install ros-humble-teleop-twist-keyboard -y



🧠 Paso 3: Hardware y Código del ESP32
El cerebro del robot está alojado en la carpeta /codigo_esp32 de este repositorio.
Conexiones Físicas (TB6612FNG)
AIN1 (Pin 21) y AIN2 (Pin 22) ➔ Dirección Motor Izquierdo
PWMA (Pin 23) ➔ Velocidad Motor Izquierdo
BIN1 (Pin 18) y BIN2 (Pin 5) ➔ Dirección Motor Derecho
PWMB (Pin 19) ➔ Velocidad Motor Derecho
STBY (Pin 4) ➔ Pin de activación (HIGH)
Configuración del Código
Abre el archivo .ino incluido en este repositorio usando Arduino IDE y modifica las siguientes líneas antes de subirlo a tu ESP32:
  // ==========================================
  // ⚠️ MODIFICAR ESTOS DATOS ⚠️
  char ssid[] = "TU_RED_WIFI";       
  char password[] = "TU_CONTRASEÑA"; 
  char agent_ip[] = "IP_DE_UBUNTU";  // La IP obtenida en el Paso 2
  // ==========================================



🎮 Paso 4: ¡A Jugar! (Ejecución)
Con el robot encendido, abre dos terminales de Ubuntu para iniciar la comunicación.
1. Levantar el Servidor (Terminal 1)
Activa el entorno de tu espacio de trabajo y enciende el Agente de micro-ROS:
cd ~/microros_ws
source install/local_setup.bash
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888



(Presiona el botón RESET en tu placa ESP32 para que se conecte).
2. Lanzar el Teclado (Terminal 2)
Abre una nueva pestaña, activa ROS 2 y lanza el control remoto:
source /opt/ros/humble/setup.bash
ros2 run teleop_twist_keyboard teleop_twist_keyboard



3. Controles
Haz clic en la Terminal 2 para asegurarte de que la ventana está activa y conduce tu robot:
I / , : Avanzar / Retroceder.
J / L : Rotar en su propio eje.
U, O, M, . : Curvas y diagonales.
K : Freno total.
Q / Z : Aumentar/Disminuir velocidad máxima.
