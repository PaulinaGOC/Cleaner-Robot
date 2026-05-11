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

Comunicación Raspberry Pi ↔ Arduino

-Cable USB tipo B desde la Raspberry Pi al Arduino Mega.
-El Arduino se reconoce como /dev/ttyACM0 o /dev/ttyACM1 en Ubuntu.
-Velocidad de comunicación: 115200 baud.

Conexión del LiDAR LD06

-Conectar al puerto USB de la Raspberry Pi mediante el adaptador USB-Serial que viene incluido.
-El LiDAR se reconoce como /dev/ttyUSB0.



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
  
