int outputPIN = 10;

int Tserial = 1;

// the setup routine runs once when you press reset:
void setup() {

  Serial.begin(9600);
  pinMode(outputPIN , OUTPUT);
  
}

void loop() {
  
  if (Serial.available() > 0) {
    //N = Serial.readString().toInt(); // read the incoming byte (as int)
    Tserial = Serial.readString().toInt();
    // halfT = 60000 / N; // in micros ; HALF the period converted from N (rpm) (as int)
    Serial.print("Change of frequency - period = "); Serial.print( 3 * Tserial ); Serial.print(" ; N = "); Serial.print(60000/(3*Tserial)); Serial.print('\n');
    Serial.flush();
  }
  

  //delay(floor(3 * halfT / 2));
  delay(2 * Tserial);
  
  digitalWrite(outputPIN, HIGH);
  digitalWrite(13, HIGH);
  

  delay(Tserial);
  

  digitalWrite(outputPIN, LOW);
  digitalWrite(13, LOW);
}
