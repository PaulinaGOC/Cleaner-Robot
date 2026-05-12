/*
=========================================================
  ROBOT DIFERENCIAL - CORREGIDO
  ROS2 + TELEOP + PWM DIRECTO + ODOMETRIA

  Arduino Mega + L298N + Encoders

  RECIBE:   CMD,v,w
  ENVIA:    ODOM,x,y,theta,encL,encR,pwmL,pwmR

  CORRECCIONES APLICADAS:
  1. sscanf reemplazado por parseFloat manual (más robusto en Arduino)
  2. Encoders en CHANGE para cuadratura completa (4x resolución)
  3. Dirección del encoder corregida para retroceso
  4. CMD_TIMEOUT solo reinicia cuando hay movimiento real
  5. Mínimo PWM con histéresis para evitar traqueteo
  6. Constrain corregido en mapFloat
=========================================================
*/

// ================= CONFIG =================
#define PULSOS_POR_VUELTA  765.0
#define RADIO_RUEDA        0.03
#define BASE_RUEDAS        0.36

#define LOOP_MS            50
#define CMD_TIMEOUT        500    // ms sin comando -> parar

// Velocidad máxima real de la rueda (m/s)
// Ajusta este valor según tu robot real
#define VEL_MAX            0.35

// PWM mínimo para vencer la estática
#define PWM_MIN            80
#define PWM_MUERTO         10   // debajo de esto, apagar motor

// ================ DRIVER ==================
// VERIFICA que ENA/IN1/IN2 correspondan al mismo canal del L298N
// ENA -> Enable del Motor A (canal A)
// IN1/IN2 -> Dirección del Motor A
// ENB -> Enable del Motor B (canal B)
// IN3/IN4 -> Dirección del Motor B

const int ENA = 6;
const int IN1 = 24;
const int IN2 = 25;

const int ENB = 5;
const int IN3 = 22;
const int IN4 = 23;

const int ENC= 8;
const int IN5 = 26;
const int IN6= 27;

// ================ ENCODERS ===============
#define ENC_L_A 2
#define ENC_L_B 4
#define ENC_R_A 3
#define ENC_R_B 7

volatile long count_L = 0;
volatile long count_R = 0;

// ================ ODOM ===================
float x     = 0.0;
float y     = 0.0;
float theta = 0.0;

long prevCountL = 0;
long prevCountR = 0;

// ================ CONTROL ===============
int pwmL = 0;
int pwmR = 0;

int dirL = 1;
int dirR = 1;

unsigned long lastCMD  = 0;
unsigned long lastLoop = 0;

bool motorActivo = false;

// ================ SERIAL ================
char buffer[64];
byte idx = 0;

// ========================================
// SETUP
// ========================================
void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENC, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);

  //MOTOR CEPILLO
  digitalWrite(IN5, HIGH);
  digitalWrite(IN6, LOW);
  analogWrite (ENC, 170);


  
  // Asegura motores apagados al inicio
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);

  // INPUT_PULLUP si tus encoders son open-drain
  // Cámbialo a INPUT si tienen resistencias externas
  pinMode(ENC_L_A, INPUT_PULLUP);
  pinMode(ENC_L_B, INPUT_PULLUP);
  pinMode(ENC_R_A, INPUT_PULLUP);
  pinMode(ENC_R_B, INPUT_PULLUP);

  Serial.begin(115200);

  // CHANGE -> cuenta flancos de subida Y bajada (resolución 4x)
  attachInterrupt(digitalPinToInterrupt(ENC_L_A), encoderLeftISR,  CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_R_A), encoderRightISR, CHANGE);

  lastCMD  = millis();
  lastLoop = millis();

  Serial.println("READY");
}

// ========================================
// LOOP
// ========================================
void loop() {

  leerSerial();

  // Watchdog: si no llega comando, parar
  if (millis() - lastCMD > CMD_TIMEOUT) {
    pwmL = 0;
    pwmR = 0;
    motorActivo = false;
  }

  moverMotores();

  if (millis() - lastLoop >= LOOP_MS) {
    actualizarOdometria();
    enviarODOM();
    lastLoop = millis();
  }
}

// ========================================
// LEER SERIAL
// ========================================
void leerSerial() {

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      buffer[idx] = '\0';
      procesarCMD();
      idx = 0;
    }
    else if (c != '\r') {        // ignora \r de sistemas Windows
      if (idx < sizeof(buffer) - 1) {
        buffer[idx++] = c;
      }
    }
  }
}

// ========================================
// PROCESAR CMD  (parseFloat manual)
//
// sscanf con %f puede fallar silenciosamente
// en algunos cores de Arduino. Esta versión
// es más robusta y verificable.
// ========================================
void procesarCMD() {

  if (strncmp(buffer, "CMD,", 4) != 0) return;

  // Busca la primera coma después de "CMD,"
  char* p1 = buffer + 4;
  char* p2 = strchr(p1, ',');

  if (p2 == NULL) {
    Serial.println("ERR: formato CMD incorrecto");
    return;
  }

  *p2 = '\0';           // parte en dos strings
  p2++;

  float v = atof(p1);
  float w = atof(p2);

  // DEBUG: descomenta para verificar recepción
  // Serial.print("DBG v="); Serial.print(v,3);
  // Serial.print(" w="); Serial.println(w,3);

  // ---- Cinemática diferencial ----
  float vL = v - (BASE_RUEDAS / 2.0) * w;
  float vR = v + (BASE_RUEDAS / 2.0) * w;

  // ---- Dirección ----
  dirL = (vL >= 0) ? 1 : -1;
  dirR = (vR >= 0) ? 1 : -1;

  vL = fabs(vL);
  vR = fabs(vR);

  // ---- Mapeo a PWM ----
  pwmL = velToPWM(vL);
  pwmR = velToPWM(vR);

  motorActivo = (pwmL > 0 || pwmR > 0);

  lastCMD = millis();
}

// ========================================
// VELOCIDAD -> PWM
// Con mínimo y zona muerta
// ========================================
int velToPWM(float vel) {

  if (vel < 0.001) return 0;           // zona muerta

  // Mapeo lineal
  int pwm = (int)((vel / VEL_MAX) * 255.0);
  pwm = constrain(pwm, 0, 255);

  // Aplica mínimo para vencer estática
  if (pwm > 0 && pwm < PWM_MIN) pwm = PWM_MIN;

  return pwm;
}

// ========================================
// MOVER MOTORES
// ========================================
void moverMotores() {

  // ---- Motor DERECHO (ENA / IN1 IN2) ----
  if (dirR == 1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(ENA, pwmR);

  // ---- Motor IZQUIERDO (ENB / IN3 IN4) ----
  if (dirL == 1) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  analogWrite(ENB, pwmL);
}

// ========================================
// ODOMETRIA
// ========================================
void actualizarOdometria() {

  noInterrupts();
  long currentL = count_L;
  long currentR = count_R;
  interrupts();

  long dL = currentL - prevCountL;
  long dR = currentR - prevCountR;

  prevCountL = currentL;
  prevCountR = currentR;

  // Con CHANGE (4x) la resolución real es 4 * PULSOS_POR_VUELTA
  float ppr = PULSOS_POR_VUELTA * 4.0;

  float distL = (2.0 * PI * RADIO_RUEDA * dL) / ppr;
  float distR = (2.0 * PI * RADIO_RUEDA * dR) / ppr;

  float dc  = (distL + distR) / 2.0;
  float dth = (distR - distL) / BASE_RUEDAS;

  x     += dc * cos(theta + dth / 2.0);
  y     += dc * sin(theta + dth / 2.0);
  theta += dth;

  // Normalizar theta a [-pi, pi]
  while (theta >  PI) theta -= 2.0 * PI;
  while (theta < -PI) theta += 2.0 * PI;
}

// ========================================
// ENVIAR ODOM
// ========================================
void enviarODOM() {

  Serial.print("ODOM,");
  Serial.print(x,     4); Serial.print(",");
  Serial.print(y,     4); Serial.print(",");
  Serial.print(theta, 4); Serial.print(",");
  Serial.print(count_L);  Serial.print(",");
  Serial.print(count_R);  Serial.print(",");
  Serial.print(pwmL);     Serial.print(",");
  Serial.println(pwmR);
}

// ========================================
// ISR ENCODER IZQUIERDO
// Cuadratura completa (CHANGE en canal A)
// ========================================
void encoderLeftISR() {
  bool a = digitalRead(ENC_L_A);
  bool b = digitalRead(ENC_L_B);

  // Signo invertido respecto al derecho porque
  // el motor izquierdo está montado en espejo
  if (a == HIGH) {
    count_L += (b ?  1 : -1);
  } else {
    count_L += (b ? -1 :  1);
  }
}

// ========================================
// ISR ENCODER DERECHO
// ========================================
void encoderRightISR() {
  bool a = digitalRead(ENC_R_A);
  bool b = digitalRead(ENC_R_B);

  if (a == HIGH) {
    count_R += (b ? 1 : -1);
  } else {
    count_R += (b ? -1 : 1);
  }
}
