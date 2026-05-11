# Tutorial: Sistema de Recolección y Separación de basura
El funcionamiento se divide en dos partes:
La primera parte se basa  en un robot móvil autónomo diseñado para apoyar la recolección de residuos sólidos en espacios controlados. El robot inicia con el encendido del sistema y la inicialización de la Raspberry Pi, Arduino y ROS 2. Posteriormente, el robot comienza la lectura de sensores, principalmente el LiDAR, para detectar obstáculos y comprender su entorno.
Una vez iniciado el sistema, el robot navega de manera autónoma siguiendo una ruta o puntos de referencia. Si detecta un obstáculo, evita la colisión y recalcula su trayectoria. Si detecta un residuo, se posiciona frente a él, activa el mecanismo de recolección y almacena el objeto dentro de su compartimiento interno.Luego, en la segunda parte consiste en clasificar y separar los residuos recolectados. Para ello, se utiliza una banda transportadora que, a través de visión y una red neuronal, puede clasificar residuos orgánicos e inorgánicos


# 📋 Requisitos previos 
Antes de ejecutar el proyecto, se recomienda contar con los siguientes elementos:
- **Hardware necesario**:
  - Raspberry Pi 5
  - Arduino Mega
  - Sensor LiDAR
  - Motores DC veloreductores con encoders (se emplearon unos motores de 76 rpms con encoders __)
  - Fuente de alimentación de 12V-20A
  - Chasis del robot (se ve en la carpetas CAT Parts)
  - Mecanismo de recolección de residuos (se empleó un cepillo controlado por un motor de corriente directa)
  - 2 Puentes H
  - Buck converter 12V-5V
    
- **Software necesario:**
  - Ubuntu 24.04 LTS
  - ROS 2 Jazzy
  - Python
  - Arduino IDE
  - Librerías de Desarrollo para ROS 2:
    - Drivers del LiDAR (ldlidar_stl_ros2 - driver del LiDAR LD06, se clona desde GitHub)
    -  RViz 2 (Visualización del robot, del mapa, trayectorias y objetivos de navegación)
    -  Nav2 (Paquete para navegación autónoma)
    -  Slam_toolbox (Utilizado para grabar el mapa)

# 📓  Introducción 
Este es un proyecto de robótica móvil enfocado en el desarrollo de un robot autónomo para la recolección de residuos sólidos en espacios controlados, como pasillos, laboratorios, campus universitarios o áreas públicas semi-estructuradas. La idea principal del proyecto es proponer una alternativa tecnológica que apoye las tareas de limpieza, reduciendo actividades manuales repetitivas y mejorando la eficiencia en la recolección de basura.
El sistema está diseñado sobre una plataforma móvil con tracción diferencial, lo que permite controlar el movimiento del robot mediante dos ruedas motorizadas. Para su funcionamiento, se utiliza una Raspberry Pi 5 como unidad principal de procesamiento, encargada de ejecutar el sistema operativo, ROS 2 Jazzy y los nodos principales del robot. Además, se utiliza un Arduino para apoyar el control de actuadores, motores o mecanismos relacionados con la recolección de residuos.
La navegación autónoma del robot se implementa mediante ROS 2 Jazzy y Nav2, herramientas que permiten al robot desplazarse dentro de un entorno, seguir rutas o waypoints, interpretar información de sensores y generar comandos de movimiento. El robot también integra un sensor LiDAR, utilizado para detectar obstáculos y obtener información del entorno, lo que permite mejorar la seguridad durante el recorrido y evitar colisiones.
En cuanto a su funcionamiento general, este consiste en iniciar el sistema, leer la información de los sensores, navegar de forma autónoma por una ruta definida, detectar obstáculos, posicionarse frente a los residuos y activar un mecanismo de recolección para almacenarlos dentro de un compartimiento interno. 
Además, el proyecto contempla la integración de visión artificial para identificar y clasificar residuos en categorías como orgánicos e inorgánicos gracias al uso de una cámara y una banda transportadora.

# 🧩 Arquitectura del sistema
El robot está dividido en dos capas de control que se comunican entre sí:
Capa alta (Raspberry Pi 5)

-Ejecuta Ubuntu 24.04 LTS y ROS 2 Jazzy
-Procesa los datos del LiDAR
-Ejecuta el algoritmo de SLAM (slam_toolbox)
-Ejecuta la navegación autónoma (Nav2)
-Visualiza el estado del sistema en RViz2
-Envía comandos de velocidad al Arduino vía USB serial

Capa baja (Arduino Mega)

-Recibe comandos de velocidad lineal y angular
-Convierte estos comandos a señales PWM para cada motor
-Controla la dirección de los motores a través de los puentes H L298N
-Lee los encoders por interrupción (en cuadratura)
-Calcula la odometría del robot en tiempo real
-Envía la odometría de regreso a la Raspberry Pi

# 💾 Instalación necesaria:
Para instalar ROS 2 Jazzy correctamente, lo más importante es elegir bien el sistema operativo y no quedarse corto de RAM/almacenamiento, sobre todo si vas a usar Nav2, RViz, LiDAR y compilación con colcon.

- Sistema Operativo: Ubuntu Server 24.04 LTS (puede ser a través de partición o con máquina virtual)
- RAM: 8GB mínimo
- Almacenamiento: 32 GB o superior
- Procesador: Raspberry Pi 5
- Puertos USB: Para conexión de LiDAR y Arduino

# 🛠️ Instrucciones 

🔌 Diagrama de conexiones eléctricas
### Conexiones del Arduino Mega a los puentes H L298N

| Componente L298N | Pin Arduino Mega | Notas |
|---|---|---|
| **L298N #1 (motores de tracción)** | | |
| ENA (Enable motor A) | 6 (PWM) | Velocidad motor derecho |
| IN1 | 24 | Dirección motor derecho |
| IN2 | 25 | Dirección motor derecho |
| ENB (Enable motor B) | 5 (PWM) | Velocidad motor izquierdo |
| IN3 | 22 | Dirección motor izquierdo |
| IN4 | 23 | Dirección motor izquierdo |
| **L298N #2 (rodillo recolector)** | | |
| ENA | 9 (PWM) | Velocidad rodillo |
| IN1 | 26 | Dirección rodillo |
| IN2 | 27 | Dirección rodillo |

### Conexiones de los encoders al Arduino

| Encoder | Pin Arduino Mega | Notas |
|---|---|---|
| Encoder izquierdo - Canal A | 2 | **Pin de interrupción** |
| Encoder izquierdo - Canal B | 4 | Lectura digital |
| Encoder derecho - Canal A | 3 | **Pin de interrupción** |
| Encoder derecho - Canal B | 7 | Lectura digital |
| VCC encoders | 5V (Arduino) | |
| GND encoders | GND (Arduino) | |

⚠️ Importante: los canales A de los encoders deben conectarse a pines de interrupción del Arduino Mega (pines 2, 3, 18, 19, 20 o 21). El Arduino Uno solo tiene 2 y 3.

### Alimentación

| Conexión | Voltaje | Notas |
|---|---|---|
| Fuente 12V → Entrada L298N #1 (VCC motores) | 12V | Alimentación motores tracción |
| Fuente 12V → Entrada L298N #2 (VCC motor) | 12V | Alimentación motor rodillo |
| Fuente 12V → Buck converter | 12V→5V | Para Raspberry Pi |
| Buck 5V → Raspberry Pi (USB-C o pines GPIO) | 5V, mínimo 3A | |
| Raspberry Pi USB → Arduino Mega | 5V vía USB | Alimenta y comunica al Arduino |
| GND común | — | **Todos los GND deben estar conectados entre sí** |

###Comunicación Raspberry Pi ↔ Arduino

-Cable USB tipo B desde la Raspberry Pi al Arduino Mega.
-El Arduino se reconoce como /dev/ttyACM0 o /dev/ttyACM1 en Ubuntu.
-Velocidad de comunicación: 115200 baud.

###Conexión del LiDAR LD06

-Conectar al puerto USB de la Raspberry Pi mediante el adaptador USB-Serial que viene incluido.
-El LiDAR se reconoce como /dev/ttyUSB0.

## 💾 Instalación del software

Esta sección cubre todo el software que necesitas instalar en la Raspberry Pi 5, desde el sistema operativo hasta los paquetes específicos de ROS 2.

### Paso 1: Instalación de Ubuntu 24.04 en la Raspberry Pi 5

1. **Descarga** [Raspberry Pi Imager](https://www.raspberrypi.com/software/) en tu computadora.
2. **Inserta** la tarjeta microSD (mínimo 32 GB) en tu computadora.
3. **Abre Raspberry Pi Imager** y selecciona:
   - **Dispositivo:** Raspberry Pi 5
   - **Sistema operativo:** "Other general-purpose OS" → Ubuntu → **Ubuntu Server 24.04 LTS (64-bit)**
   - **Almacenamiento:** tu tarjeta microSD
4. Antes de escribir, click en el ícono de **engrane (⚙️)** para configurar:
   - **Hostname:** `raspberry` (o el que prefieras)
   - **Habilitar SSH:** ✅
   - **Usuario y contraseña:** define ambos
   - **Wi-Fi:** configura tu red
   - **Zona horaria:** la tuya
5. Click en **"Write"** y espera a que termine.
6. Inserta la microSD en la Raspberry Pi 5 y enciéndela.

> 💡 **Nota:** Aunque Ubuntu Server no tiene interfaz gráfica, se recomienda porque consume menos recursos. Si prefieres la versión Desktop, también funciona pero es más pesada.

### Paso 2: Instalación del entorno gráfico (opcional pero recomendado)

Si elegiste Ubuntu Server pero quieres usar RViz2 directamente en la Raspberry, instala un entorno gráfico ligero:

```bash
sudo apt update
sudo apt install -y ubuntu-desktop-minimal
sudo reboot
```

### Paso 3: Configuración de permisos y limpieza del sistema

Antes de instalar ROS 2, hay que preparar el sistema:

```bash
# Agregar el usuario al grupo dialout (necesario para acceder al Arduino por USB)
sudo usermod -aG dialout $USER

# Quitar brltty que bloquea el puerto USB del Arduino
sudo apt remove -y brltty

# Aplicar cambios de grupo (alternativa a reiniciar sesión)
newgrp dialout
```

### Paso 4: Instalación de ROS 2 Jazzy

Sigue estos comandos en orden:

```bash
# Actualizar el sistema
sudo apt update && sudo apt upgrade -y

# Instalar dependencias básicas
sudo apt install -y software-properties-common curl

# Habilitar el repositorio universe
sudo add-apt-repository universe -y

# Agregar la clave GPG de ROS 2
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
  -o /usr/share/keyrings/ros-archive-keyring.gpg

# Agregar el repositorio de ROS 2
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" \
  | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

# Actualizar repositorios
sudo apt update

# Instalar ROS 2 Jazzy (versión completa con RViz2)
sudo apt install -y ros-jazzy-desktop

# Instalar herramientas de desarrollo
sudo apt install -y python3-colcon-common-extensions python3-rosdep python3-argcomplete

# Inicializar rosdep
sudo rosdep init
rosdep update
```

### Paso 5: Configurar ROS 2 al iniciar la terminal

Para que ROS 2 esté disponible cada vez que abras una terminal:

```bash
echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

Verifica que ROS 2 quedó bien instalado:

```bash
ros2 --help
```

Debe mostrar la ayuda del comando `ros2`.

### Paso 6: Instalación de paquetes específicos

Estos son los paquetes que el robot necesita para SLAM y navegación:

```bash
# Navegación autónoma (Nav2)
sudo apt install -y ros-jazzy-navigation2 ros-jazzy-nav2-bringup

# SLAM (construcción de mapas)
sudo apt install -y ros-jazzy-slam-toolbox

# Servidor de mapas
sudo apt install -y ros-jazzy-nav2-map-server

# Librería de comunicación serial para Python
sudo apt install -y python3-serial
```

### Paso 7: Instalación del Arduino IDE

El Arduino IDE es necesario para subir el firmware al Arduino Mega:

```bash
# Descargar Arduino IDE (versión 2.x)
sudo apt install -y arduino
```

O descargar manualmente desde [arduino.cc/en/software](https://www.arduino.cc/en/software).

---

## 📝 Configuración del proyecto en ROS 2

### Paso 1: Crear el workspace ROS 2

```bash
# Crear la estructura del workspace
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
```

### Paso 2: Clonar e instalar el driver del LiDAR LD06

```bash
cd ~/ros2_ws/src
git clone https://github.com/ldrobotSensorTeam/ldlidar_stl_ros2.git

# Compilar solo el driver del LiDAR
cd ~/ros2_ws
colcon build --packages-select ldlidar_stl_ros2
source install/setup.bash
```

### Paso 3: Crear el paquete del robot

```bash
cd ~/ros2_ws/src
ros2 pkg create --build-type ament_python robot_recolector
```

Esto crea la estructura básica del paquete:

```
robot_recolector/
├── package.xml
├── setup.py
├── setup.cfg
├── resource/
├── test/
└── robot_recolector/
    └── __init__.py
```

### Paso 4: Estructura final del paquete

Dentro de `~/ros2_ws/src/robot_recolector/`, crea estas carpetas:

```bash
cd ~/ros2_ws/src/robot_recolector
mkdir -p config launch
```

La estructura final debe quedar así:

```
robot_recolector/
├── config/
│   ├── mapper_params_online_async.yaml   # Configuración SLAM
│   ├── nav2_params.yaml                   # Configuración Nav2
│   ├── slam.rviz                          # Vista RViz para SLAM
│   └── nav2.rviz                          # Vista RViz para navegación
├── launch/
│   ├── slam_launch.py                     # Launch para SLAM
│   └── nav2_launch.py                     # Launch para navegación
├── robot_recolector/
│   ├── __init__.py
│   ├── cmdvel_serial.py                   # Bridge Arduino ↔ ROS2
│   └── teleop_keyboard.py                 # Control por teclado
├── package.xml
├── setup.cfg
└── setup.py
```

### Paso 5: Configurar el setup.py

Reemplaza el contenido del archivo `setup.py` por lo siguiente:

```python
from setuptools import find_packages, setup
import os
from glob import glob

package_name = 'robot_recolector'

setup(
    name=package_name,
    version='0.0.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'),
            glob('launch/*.py')),
        (os.path.join('share', package_name, 'config'),
            glob('config/*.yaml') + glob('config/*.rviz')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='tu_nombre',
    maintainer_email='tu_correo@ejemplo.com',
    description='Robot recolector de basura autónomo',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'teleop_keyboard = robot_recolector.teleop_keyboard:main',
            'cmdvel_serial = robot_recolector.cmdvel_serial:main',
        ],
    },
)
```

### Paso 6: Crear los archivos de código

Los archivos completos (`cmdvel_serial.py`, `teleop_keyboard.py`, sketch del Arduino, configuraciones de Nav2 y SLAM) están disponibles en este repositorio. Copia cada archivo a su carpeta correspondiente según la estructura del Paso 4.

### Paso 7: Compilar el paquete

```bash
cd ~/ros2_ws
colcon build --packages-select robot_recolector
source install/setup.bash
```

Para que el workspace se cargue automáticamente al abrir una terminal:

```bash
echo "source ~/ros2_ws/install/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

### Paso 8: Subir el firmware al Arduino

1. Abre el Arduino IDE.
2. Conecta el Arduino Mega por USB a la Raspberry.
3. En **Herramientas → Placa**, selecciona **Arduino Mega 2560**.
4. En **Herramientas → Puerto**, selecciona `/dev/ttyACM0` (o el que aparezca).
5. Abre el archivo `robot_arduino.ino` del repositorio.
6. Click en **Subir** (botón con flecha →).
7. Espera a que diga "Subida completa".

> ⚠️ **Importante:** Antes de correr los nodos ROS 2, **cierra el Serial Monitor del IDE de Arduino**, ya que ocupa el puerto USB y bloquea la comunicación.

---

## 🛠️ Modo de uso

El robot tiene tres modos principales de operación:

1. **Teleoperación** — controlar el robot con el teclado para pruebas iniciales.
2. **SLAM** — construir el mapa del entorno por primera vez.
3. **Navegación autónoma** — usar un mapa ya guardado para que el robot vaya solo a un destino.

Cada modo requiere terminales separadas, todas con `source ~/ros2_ws/install/setup.bash` ya aplicado (esto pasa automáticamente si lo agregaste al `.bashrc`).

### 🎮 Modo 1: Teleoperación

Sirve para probar que los motores responden correctamente y para calibrar la odometría.

**Terminal 1 — Bridge serial (Arduino ↔ ROS2):**
```bash
ros2 run robot_recolector cmdvel_serial
```

Espera a ver el mensaje `Puerto /dev/ttyACM0 abierto`.

**Terminal 2 — Teleop por teclado:**
```bash
ros2 run robot_recolector teleop_keyboard
```

Verás un panel con las teclas disponibles:

| Tecla | Acción |
|---|---|
| `w` | Avanzar |
| `x` | Retroceder |
| `a` | Girar izquierda |
| `d` | Girar derecha |
| `s` | Parada de emergencia |
| `Ctrl + C` | Salir |

> El robot se mueve solo mientras presionas la tecla. Al soltar, se detiene.

### 🗺️ Modo 2: SLAM (construcción del mapa)

Este modo se usa **una sola vez** para mapear el espacio donde trabajará el robot.

**Terminal 1 — Bridge serial:**
```bash
ros2 run robot_recolector cmdvel_serial
```

**Terminal 2 — Driver del LiDAR:**
```bash
ros2 launch ldlidar_stl_ros2 ld06.launch.py
```

**Terminal 3 — SLAM + RViz:**
```bash
ros2 launch robot_recolector slam_launch.py
```

**Terminal 4 — Teleop para mover el robot mientras se mapea:**
```bash
ros2 run robot_recolector teleop_keyboard
```

Mueve el robot **lentamente** por todo el espacio que quieras mapear. Procura:
- Avanzar de forma suave, sin giros bruscos.
- Pasar por las paredes y esquinas varias veces.
- Cerrar el recorrido regresando al punto inicial (esto ayuda al "loop closure").

En RViz verás cómo se construye el mapa en tiempo real:
- ⬜ Áreas blancas = zonas libres
- ⬛ Áreas negras = obstáculos detectados
- 🔘 Áreas grises = zonas desconocidas

### 💾 Modo 3: Guardar el mapa

Cuando el mapa esté completo y el robot esté detenido, **en una terminal nueva**:

```bash
mkdir -p ~/ros2_ws/maps
cd ~/ros2_ws/maps
ros2 run nav2_map_server map_saver_cli -f mi_mapa
```

Esto genera dos archivos en `~/ros2_ws/maps/`:
- `mi_mapa.pgm` (imagen del mapa)
- `mi_mapa.yaml` (metadatos del mapa)

> 💡 Cambia `mi_mapa` por el nombre que prefieras. Recuerda actualizar `nav2_params.yaml` para apuntar a este archivo.

### 🚦 Modo 4: Navegación autónoma con Nav2

Una vez que tengas un mapa guardado, puedes usar Nav2 para que el robot navegue solo.

**Terminal 1 — Bridge serial:**
```bash
ros2 run robot_recolector cmdvel_serial
```

**Terminal 2 — Driver del LiDAR:**
```bash
ros2 launch ldlidar_stl_ros2 ld06.launch.py
```

**Terminal 3 — Nav2 + RViz:**
```bash
ros2 launch robot_recolector nav2_launch.py
```

Espera unos 15 segundos a que todos los nodos arranquen.

### 📍 Localizar al robot (obligatorio antes de navegar)

En RViz, antes de mandar cualquier objetivo:

1. Click en el botón **2D Pose Estimate** (arriba en la barra de herramientas).
2. Click en el mapa **donde está físicamente el robot**, y **arrastra** en la dirección que el robot está mirando.
3. Suelta el clic.
4. Verás una nube azul de partículas concentrándose alrededor del robot — es AMCL localizándose.

### 🎯 Mandar un objetivo

1. Click en el botón **2D Goal Pose** (a la derecha del 2D Pose Estimate).
2. Click en el mapa donde quieres que vaya el robot, y arrastra para indicar la orientación final.
3. Suelta. Verás aparecer una línea verde (la ruta calculada).
4. El robot empezará a moverse autónomamente.

---



# 📖 Referencias y Recursos Adicionales

# 📞 Contacto
Para preguntas o sugerencias: 

- Asesor encargado de la página: Dr. César Martínez Torres
    - 📥Correo electrónico:
      cesar.martinez@udlap.mx
    
- Redactores del tutorial:
  - Paulina Gómez Olvera Carpinteyro
    - 📥Correo electrónico:
      paulinagoc@outlook.com
      
  - Rashid Manzur Rodríguez
    - 📥Correo electrónico:
      rashidmanzurrz@udlap.mx
     
  - Denisse Alvarado Palacios
    - 📥Correo electrónico:
      asa
     
  - Daniel Yamil Tlilayatzi Muñoz
    - 📥Correo electrónico:
      daniel.tlilayatzimz@udlap.mx
  
