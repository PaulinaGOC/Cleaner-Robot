# Tutorial: Sistema de Recolección y Separación de basura
El funcionamu funcionamiento se divide en dos partes:
La primera parte se basa  en un robot móvil autónomo diseñado para apoyar la recolección de residuos sólidos en espacios controlados. El robot inicia con el encendido del sistema y la inicialización de la Raspberry Pi, Arduino y ROS 2. Posteriormente, el robot comienza la lectura de sensores, principalmente el LiDAR, para detectar obstáculos y comprender su entorno.
Una vez iniciado el sistema, el robot navega de manera autónoma siguiendo una ruta o puntos de referencia. Si detecta un obstáculo, evita la colisión y recalcula su trayectoria. Si detecta un residuo, se posiciona frente a él, activa el mecanismo de recolección y almacena el objeto dentro de su compartimiento interno.Luego, en la segunda parte consiste en clasificar y separar los residuos recolectados. Para ello, se utiliza una banda transportadora que, a través de visión y una red neuronal, puede clasificar residuos orgánicos e inorgánicos


# 📋 Requisitos previos 
Antes de ejecutar el proyecto, se recomienda contar con los siguientes elementos:
- **Hardware necesario**:
  - Raspberry Pi 5
  - Arduino Mega
  - Sensor LiDAR
  - Motores DC veloreductores con encoders (se emplearon unos motores de 76 rpms con encoders __)
  - Fuente de alimentación de 12 V con 20 A
  - Chasis del robot (se ve en la sección de cad)
  - Mecanismo de recolección de residuos (se empleó un cepillo controlado por un motor de corriente directa)
  - 2 Puentes H
  - Buck converter 12V-5V
    
- **Software necesario:**
  - Ubuntu 24.04 LTS
  - ROS 2 Jazzy
  - Python
  - Arduino IDE
  - Librerías de Desarrollo para ROS 2:
    - Drivers del LiDAR
    -  RViz 2 (Visualización del robot, del mapa, trayectorias y objetivos de navegación)
    -  Nav2 (Paquete para navegación autónoma)

# 📓  Introducción 
Este es un proyecto de robótica móvil enfocado en el desarrollo de un robot autónomo para la recolección de residuos sólidos en espacios controlados, como pasillos, laboratorios, campus universitarios o áreas públicas semi-estructuradas. La idea principal del proyecto es proponer una alternativa tecnológica que apoye las tareas de limpieza, reduciendo actividades manuales repetitivas y mejorando la eficiencia en la recolección de basura.
El sistema está diseñado sobre una plataforma móvil con tracción diferencial, lo que permite controlar el movimiento del robot mediante dos ruedas motorizadas. Para su funcionamiento, se utiliza una Raspberry Pi 5 como unidad principal de procesamiento, encargada de ejecutar el sistema operativo, ROS 2 Jazzy y los nodos principales del robot. Además, se utiliza un Arduino para apoyar el control de actuadores, motores o mecanismos relacionados con la recolección de residuos.
La navegación autónoma del robot se implementa mediante ROS 2 Jazzy y Nav2, herramientas que permiten al robot desplazarse dentro de un entorno, seguir rutas o waypoints, interpretar información de sensores y generar comandos de movimiento. El robot también integra un sensor LiDAR, utilizado para detectar obstáculos y obtener información del entorno, lo que permite mejorar la seguridad durante el recorrido y evitar colisiones.
En cuanto a su funcionamiento general, este consiste en iniciar el sistema, leer la información de los sensores, navegar de forma autónoma por una ruta definida, detectar obstáculos, posicionarse frente a los residuos y activar un mecanismo de recolección para almacenarlos dentro de un compartimiento interno. 
Además, el proyecto contempla la integración de visión artificial para identificar y clasificar residuos en categorías como orgánicos e inorgánicos gracias al uso de una cámara y una banda transportadora.

# 💾 Instalación necesaria:
Para instalar ROS 2 Jazzy correctamente, lo más importante es elegir bien el sistema operativo y no quedarse corto de RAM/almacenamiento, sobre todo si vas a usar Nav2, RViz, LiDAR y compilación con colcon.

- Sistema Operativo: Ubuntu Server 24.04 LTS (puede ser a través de partición o con máquina virtual)
- RAM: 8GB mínimo
- Almacenamiento: 64 GB o superior
- Procesador: Raspberry Pi 5
- Puertos USB: Para conexión de LiDAR y Arduino
# 🛠️ Instrucciones 


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
      asa
     
  - Denisse Alvarado Palacios
    - 📥Correo electrónico:
      asa
     
  - Daniel __
    - 📥Correo electrónico:
      asa
  
