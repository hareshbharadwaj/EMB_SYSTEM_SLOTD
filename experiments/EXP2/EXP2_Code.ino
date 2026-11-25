#define GREEN1 6
#define GREEN2 5
#define YELLOW1 4
#define YELLOW2 3
#define RED1 2
#define RED2 1
void setup()
{
  pinMode(GREEN1, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  pinMode(YELLOW1, OUTPUT);
  pinMode(YELLOW2, OUTPUT);
  pinMode(RED1, OUTPUT);
  pinMode(RED2, OUTPUT);
}
void loop()
{
  // Green LEDs ON
  digitalWrite(GREEN1, HIGH);
  digitalWrite(GREEN2, HIGH);
  digitalWrite(YELLOW1, LOW);
  digitalWrite(YELLOW2, LOW);
  digitalWrite(RED1, LOW);
  digitalWrite(RED2, LOW);
  delay(5000);
  // Yellow LEDs ON
  digitalWrite(GREEN1, LOW);
  digitalWrite(GREEN2, LOW);
  digitalWrite(YELLOW1, HIGH);
  digitalWrite(YELLOW2, HIGH);
  delay(2000);
  // Red LEDs ON
  digitalWrite(YELLOW1, LOW);
  digitalWrite(YELLOW2, LOW);
  digitalWrite(RED1, HIGH);
  digitalWrite(RED2, HIGH);
  delay(5000);
// Turn all LEDs OFF
  digitalWrite(RED1, LOW);
  digitalWrite(RED2, LOW);
  delay(1000);
