import cv2
from ultralytics import YOLO
import serial
import time

# =================================================
# CONFIGURACION GENERAL
# =================================================

MODEL_PATH = "best.pt"      # Modelo YOLOv8
CAMERA_INDEX = 0            # Webcam USB
ARDUINO_PORT = "COM4"       # Arduino en puerto COM4
BAUDRATE = 9600

# Tiempo de espera para servo
SERVO_DELAY = 7

# Confianza minima
CONFIDENCE_THRESHOLD = 0.60

# =================================================
# CARGAR MODELO
# =================================================

print("Cargando modelo YOLOv8...")

model = YOLO(MODEL_PATH)

print("Modelo cargado correctamente")

# =================================================
# CONECTAR ARDUINO
# =================================================

print("Conectando Arduino...")

arduino = serial.Serial(ARDUINO_PORT, BAUDRATE)

time.sleep(2)

print("Arduino conectado en COM4")

# =================================================
# INICIAR CAMARA WEB
# =================================================

print("Iniciando webcam...")

cap = cv2.VideoCapture(CAMERA_INDEX)

if not cap.isOpened():
    print("ERROR: No se pudo abrir la webcam")
    exit()

print("Webcam iniciada correctamente")

# =================================================
# VARIABLES
# =================================================

processing = False

# =================================================
# LOOP PRINCIPAL
# =================================================

while True:

    ret, frame = cap.read()

    if not ret:
        print("Error leyendo webcam")
        break

    # =============================================
    # DETECCION YOLO
    # =============================================

    results = model(frame, conf=CONFIDENCE_THRESHOLD)

    detected_class = None

    for result in results:

        boxes = result.boxes

        for box in boxes:

            # Coordenadas
            x1, y1, x2, y2 = map(int, box.xyxy[0])

            # Clase detectada
            cls = int(box.cls[0])

            # Nombre clase
            class_name = model.names[cls]

            # Confianza
            confidence = float(box.conf[0])

            detected_class = class_name

            # =====================================
            # DIBUJAR RECTANGULO
            # =====================================

            cv2.rectangle(
                frame,
                (x1, y1),
                (x2, y2),
                (0, 255, 0),
                2
            )

            # Texto
            label = f"{class_name} {confidence:.2f}"

            cv2.putText(
                frame,
                label,
                (x1, y1 - 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.7,
                (0, 255, 0),
                2
            )

    # =============================================
    # CONTROL DE CLASIFICACION
    # =============================================

    if detected_class is not None and not processing:

        processing = True

        print(f"Objeto detectado: {detected_class}")

        # =========================================
        # ORGANICO
        # =========================================

        if detected_class.lower() == "organico":

            print("Enviando comando ORGANICO al Arduino")

            arduino.write(b'O')

            time.sleep(SERVO_DELAY)

        # =========================================
        # INORGANICO
        # =========================================

        elif detected_class.lower() == "inorganico":

            print("Enviando comando INORGANICO al Arduino")

            arduino.write(b'I')

            time.sleep(SERVO_DELAY)

        processing = False

    # =============================================
    # MOSTRAR VIDEO
    # =============================================

    cv2.imshow("Clasificacion de Basura - YOLOv8", frame)

    # ESC para salir
    key = cv2.waitKey(1)

    if key == 27:
        break

# =================================================
# CERRAR TODO
# =================================================

cap.release()

cv2.destroyAllWindows()

arduino.close()

print("Programa finalizado")