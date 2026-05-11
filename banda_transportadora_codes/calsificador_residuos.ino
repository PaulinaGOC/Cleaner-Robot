#include <Servo.h>

// ==============================
// Crear objeto servo
// ==============================

Servo servoSeparador;

// ==============================
// Variable para datos seriales
// ==============================

String dato = "";

// ==============================
// Configuración inicial
// ==============================

void setup() {

  // Iniciar comunicación serial
  Serial.begin(115200);

  // Conectar servo al pin 9
  servoSeparador.attach(9);

  // Posición inicial del servo
  servoSeparador.write(90);

  Serial.println("Sistema iniciado");
}

// ==============================
// Bucle principal
// ==============================

void loop() {

  // Verificar si llegaron datos
  if (Serial.available()) {

    // Leer texto enviado desde Python
    dato = Serial.readStringUntil('\n');

    // Eliminar espacios o saltos de línea
    dato.trim();

    Serial.print("Dato recibido: ");
    Serial.println(dato);

    // ==============================
    // Residuo orgánico
    // ==============================

    if (dato == "organico") {

      Serial.println("Clasificacion: Organico");

      // Mover servo hacia un lado
      servoSeparador.write(30);

      delay(1000);

      // Regresar a posición inicial
      servoSeparador.write(90);

      delay(500);
    }

    // ==============================
    // Residuo inorgánico
    // ==============================

    if (dato == "inorganico") {

      Serial.println("Clasificacion: Inorganico");

      // Mover servo hacia el otro lado
      servoSeparador.write(150);

      delay(1000);

      // Regresar a posición inicial
      servoSeparador.write(90);

      delay(500);
    }
  }
}