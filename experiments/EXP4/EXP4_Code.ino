#define LM35_PIN A0
#define PWM_PIN 9

float temperature;
int pwmValue;
int pwmPercent;

void setup()
{
  pinMode(PWM_PIN, OUTPUT);

  Serial.begin(9600);

  Serial.println("LM35 TEMPERATURE MONITORING SYSTEM");
  Serial.println("----------------------------------");
  Serial.println("Temperature: C | PWM: %");
}

void loop()
{
  // Read LM35 sensor
  int sensorValue = analogRead(LM35_PIN);

  // Convert analog reading into voltage
  float voltage = sensorValue * (5.0 / 1023.0);

  // LM35 output: 10 mV per degree Celsius
  temperature = voltage * 100.0;

  // Set PWM according to temperature
  if (temperature < 25.0)
  {
    pwmPercent = 0;
  }
  else if (temperature < 30.0)
  {
    pwmPercent = 20;
  }
  else if (temperature < 33.0)
  {
    pwmPercent = 40;
  }
  else if (temperature < 36.0)
  {
    pwmPercent = 60;
  }
  else if (temperature < 39.0)
  {
    pwmPercent = 80;
  }
  else
  {
    pwmPercent = 100;
  }

  // Convert percentage into Arduino PWM value
  pwmValue = map(pwmPercent, 0, 100, 0, 255);

  // Control transistor output
  analogWrite(PWM_PIN, pwmValue);

  // Display results in Proteus Virtual Terminal
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print(" C   PWM: ");
  Serial.print(pwmPercent);
  Serial.println("%");

  delay(1000);
}
