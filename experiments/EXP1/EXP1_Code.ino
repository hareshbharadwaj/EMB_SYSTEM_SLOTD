#define GAS_PIN A0
#define PIR_PIN 2
#define TRIG_PIN 8
#define ECHO_PIN 9
#define BUZZER 10
#define LED 11

void setup()
{
  Serial.begin(9600);
  pinMode(GAS_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
}

void loop()
{
  int gas = analogRead(GAS_PIN);
  int motion = digitalRead(PIR_PIN);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  float distance = duration * 0.0343 / 2;

  Serial.print("Gas Level: ");
  Serial.println(gas);
  Serial.print("Motion: ");
  Serial.println(motion ? "DETECTED" : "NOT DETECTED");
  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.println(" cm");

  if (gas > 500)
    Serial.println("GAS ALERT!");
  else if (motion)
    Serial.println("OCCUPANCY DETECTED");
  else if (duration > 0 && distance <= 30)
    Serial.println("OBJECT DETECTED!");
  else
    Serial.println("SYSTEM SAFE");

  bool alert = gas > 500 || motion || (duration > 0 && distance <= 30);
  digitalWrite(BUZZER, alert ? HIGH : LOW);
  digitalWrite(LED, alert ? HIGH : LOW);

  Serial.println("--------------------");
  delay(1000);
}
