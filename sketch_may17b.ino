// ===== PIN DEFINITIONS =====
#define TRIG 12
#define ECHO 10

const int moisturePin = A1;
const int pumpPin = 3;
const int flamePin = A3;

int seuilFlamme = 300;

// LEDs + buzzer
const int redLedPin = 2;     
const int greenLedPin = 4;   
const int yellowLedPin = 5;  
const int buzzerPin = 6;


// ===== VARIABLES =====
int moistureValue = 0;
int threshold = 400;

float tankHeight = 12.5;
float distance = 0;
float waterLevel = 0;

// ===== ULTRASONIC FUNCTION =====
float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);

  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);

  // Ultrasonic
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Pump
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  // Flame sensor
  pinMode(flamePin, INPUT);

  // LEDs + buzzer
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  // =========================
  // WATER LEVEL MEASUREMENT
  // =========================
  distance = getDistance();

  if (distance > 2 && distance < 400) {
    waterLevel = tankHeight - distance;

    if (waterLevel < 0) {
      waterLevel = 0;
    }
  }

  // =========================
  // SOIL MOISTURE
  // =========================
  moistureValue = analogRead(moisturePin);

  bool pumpState = false;

  // Pompe ON seulement si :
  // sol sec + niveau d'eau suffisant
  if (moistureValue > threshold && waterLevel >= 3) {
    digitalWrite(pumpPin, HIGH);
    pumpState = true;
  } 
  else {
    digitalWrite(pumpPin, LOW);
    pumpState = false;
  }

  // =========================
  // WATER LEVEL ALERT SYSTEM
  // =========================
  bool lowLevel = (waterLevel <= 3);
  bool highLevel = (waterLevel >= 8);
  bool normalLevel = (!lowLevel && !highLevel);

  // Reset LEDs/Buzzer
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
  noTone(buzzerPin);

  if (lowLevel) {
    digitalWrite(redLedPin, HIGH);

    // Slow beep
    tone(buzzerPin, 1000);
    delay(200);
    noTone(buzzerPin);
    delay(200);
  }
  else if (highLevel) {
    digitalWrite(yellowLedPin, HIGH);

    // Fast beep
    tone(buzzerPin, 2000);
    delay(100);
    noTone(buzzerPin);
    delay(100);
  }
  else if (normalLevel) {
    digitalWrite(greenLedPin, HIGH);
  }

  // =========================
  // FLAME DETECTION
  // =========================
  int flameValue = analogRead(flamePin);

  if (flameValue < seuilFlamme) {
    Serial.println("🔥 Flamme detectee");

    // Stop pump for safety
    digitalWrite(pumpPin, LOW);
    pumpState = false;

    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);

    tone(buzzerPin, 2000);
    delay(200);
    noTone(buzzerPin);
  }
  // =========================
  // SERIAL MONITOR
  // =========================
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Level: ");
  Serial.print(waterLevel);
  Serial.print(" cm | Moisture: ");
  Serial.print(moistureValue);
  Serial.print(" | Pump: ");
  Serial.println(pumpState ? "ON" : "OFF");

  delay(800);
}