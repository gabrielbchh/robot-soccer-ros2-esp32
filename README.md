# 🤖 Robot Soccer con ESP32 y ROS 2 (WSL)

¡Bienvenido al repositorio oficial del Robot Soccer! En este proyecto aprenderás a controlar un robot físico basado en un microcontrolador ESP32 utilizando **ROS 2 Humble** ejecutado desde Windows a través de WSL (Ubuntu).

Este tutorial está diseñado para equipos que ya tienen WSL y ROS 2 instalados de forma nativa.

---

## 📑 Índice
1. [Paso 1: Configurar la Red en Windows](#paso-1-configurar-la-red-en-windows)
2. [Paso 2: Preparación en Ubuntu](#paso-2-preparacion-en-ubuntu)
3. [Paso 3: Hardware del ESP32](#paso-3-hardware-del-esp32)
4. [Paso 4: ¡A Jugar! (Ejecución)](#paso-4-a-jugar-ejecucion)

---

## 🧱 Paso 1: Configurar la Red en Windows 

Por defecto, WSL crea una red virtual aislada. Para que tu ESP32 pueda comunicarse con Ubuntu a través de tu router WiFi, debemos unificar las direcciones IP mediante el Modo Espejo.

1. En Windows, presiona `Windows + R`, escribe `%USERPROFILE%` y presiona **Enter**.
2. Crea un nuevo documento de texto y nómbralo exactamente **`.wslconfig`** (asegúrate de borrar la extensión oculta `.txt`).
3. Abre el archivo y pega este código:

```ini
[wsl2]
networkingMode=mirrored
```

4. Abre **PowerShell** y reinicia WSL con este comando para aplicar los cambios:

```powershell
wsl --shutdown
```

---

## 💻 Paso 2: Preparación en Ubuntu

1. **Obtener la IP del Servidor:** Abre tu terminal de Ubuntu y ejecuta:

```bash
hostname -I
```
*Anota la dirección IP que aparece (ej. `192.168.18.23`). Esta es la dirección que el ESP32 buscará en la red.*

2. **Instalar el Control por Teclado:** Ejecuta este comando para instalar el paquete oficial de teleoperación:

```bash
sudo apt update
sudo apt install ros-humble-teleop-twist-keyboard -y
```

---

## 🧠 Paso 3: Hardware del ESP32

Asegúrate de cargar tu código fuente al ESP32 (modificando previamente las credenciales WiFi y la IP de Ubuntu).

### Conexiones Físicas (TB6612FNG)
* `AIN1` (Pin 21) y `AIN2` (Pin 22) ➔ Dirección Motor Izquierdo
* `PWMA` (Pin 23) ➔ Velocidad Motor Izquierdo
* `BIN1` (Pin 18) y `BIN2` (Pin 5) ➔ Dirección Motor Derecho
* `PWMB` (Pin 19) ➔ Velocidad Motor Derecho
* `STBY` (Pin 4) ➔ Pin de activación (HIGH)

---

## 🎮 Paso 4: ¡A Jugar! (Ejecución)

Con el robot encendido, abre **dos terminales de Ubuntu** para iniciar la comunicación.

### 1. Levantar el Servidor (Terminal 1)
Activa tu entorno de trabajo y enciende el Agente de micro-ROS:

```bash
cd ~/microros_ws
source install/local_setup.bash
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
```
*(Presiona el botón **RESET** en tu placa ESP32 para que se conecte al servidor).*

### 2. Lanzar el Teclado (Terminal 2)
Abre una nueva pestaña, activa ROS 2 base y lanza el control remoto:

```bash
source /opt/ros/humble/setup.bash
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

### 3. Controles
Haz clic en la Terminal 2 para asegurarte de que la ventana está activa y conduce tu robot:
* **`I` / `,`** : Avanzar / Retroceder rectos.
* **`J` / `L`** : Rotar en su propio eje.
* **`U`, `O`, `M`, `.`** : Curvas y diagonales.
* **`K`** : Freno total.
* **`Q` / `Z`** : Aumentar/Disminuir la velocidad máxima de los motores.
