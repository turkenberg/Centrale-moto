int outputPIN = 10;

unsigned long elapsed = 0;
unsigned long previousTick = 0;

//int frequency = 31;
int N = 1200;
int T;
int halfT;

// the setup routine runs once when you press reset:
void setup() {

  Serial.begin(9600);

  elapsed = 0;

  T = 1000000 * 60/N; // in micros
  halfT = static_cast<int>(T/2);
  
}

// the loop routine runs over and over again forever:
void loop() {

  if (Serial.available() > 0) {
    N = Serial.read(); // read the incoming byte:
    T = 1000000 * 60/N; // in micros
    halfT = static_cast<int>(T/2);
    Serial.println("Change of frequency");
  }

  delayMicroseconds(halfT);
  
  digitalWrite(outputPIN, HIGH);

  delayMicroseconds(halfT);

  digitalWrite(outputPIN, LOW);
}
