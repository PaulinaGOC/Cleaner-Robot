from ultralytics import YOLO
import cv2
import serial
import time

# ==========================================
# Cargar modelo YOLOv8 entrenado
# ==========================================

model = YOLO("../modelos/best.pt")

# ==========================================
# Conexión serial con Arduino
# ==========================================

arduino = serial.Serial('COM3', 115200)

# Esperar inicialización de Arduino
time.sleep(2)

# ==========================================
# Abrir cámara USB
# ==========================================

cap = cv2.VideoCapture(0)   #AQUÍ SE PUEDE CAMBIAR SI NO ABRE LA CÁMARA QUE BUSCAMOS

# ==========================================
# Variables de control
# ==========================================

ultima_clasificacion = ""
ultimo_envio = 0

# Tiempo mínimo entre envíos seriales
cooldown = 2

# ==========================================
# Bucle principal
# ==========================================

while True:

    # Capturar frame
    ret, frame = cap.read()

    # Verificar captura
    if not ret:
        print("No se pudo capturar la cámara")
        break

    # ==========================================
    # Ejecutar detección YOLO
    # ==========================================

    resultados = model(frame)

    # ==========================================
    # Recorrer detecciones
    # ==========================================

    for r in resultados:

        cajas = r.boxes

        for caja in cajas:

            # Confianza
            confianza = float(caja.conf[0])

            # Ignorar detecciones débiles
            if confianza < 0.70:
                continue

            # Clase detectada
            clase_id = int(caja.cls[0])

            clase = model.names[clase_id]

            # Coordenadas
            x1, y1, x2, y2 = map(int, caja.xyxy[0])

            # ==========================================
            # Dibujar rectángulo
            # ==========================================

            cv2.rectangle(
                frame,
                (x1, y1),
                (x2, y2),
                (0, 255, 0),
                2
            )

            # ==========================================
            # Mostrar texto
            # ==========================================

            texto = f"{clase} {confianza:.2f}"

            cv2.putText(
                frame,
                texto,
                (x1, y1 - 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.7,
                (0, 255, 0),
                2
            )

            # ==========================================
            # Enviar solo si pasó cooldown
            # ==========================================

            tiempo_actual = time.time()

            if (
                tiempo_actual - ultimo_envio > cooldown
                and clase != ultima_clasificacion
            ):

                # Enviar clasificación
                arduino.write((clase + "\n").encode())

                print(f"Enviado a Arduino: {clase}")

                # Guardar datos
                ultimo_envio = tiempo_actual
                ultima_clasificacion = clase

    # ==========================================
    # Mostrar ventana
    # ==========================================

    cv2.imshow("Clasificador Inteligente", frame)

    # ==========================================
    # Salir con tecla q
    # ==========================================

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# ==========================================
# Liberar recursos
# ==========================================

cap.release()
cv2.destroyAllWindows()

arduino.close()