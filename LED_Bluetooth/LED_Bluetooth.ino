#include <SoftwareSerial.h>
#define RX 8
#define TX 9
#define LED 2

char incomingByte = 0;
SoftwareSerial bluetooth(RX, TX);

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  bluetooth.begin(9600);
}

void loop() {
  if (bluetooth.available() > 0) {
    incomingByte = bluetooth.read();
    Serial.print(incomingByte);

    if (incomingByte == '0') {
      digitalWrite(2, LOW);
    }

    if (incomingByte == '1') {
      digitalWrite(2, HIGH);
    }
      
  }

  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.print(incomingByte);

    if (incomingByte == '0') {
      digitalWrite(2, LOW);
    }

    if (incomingByte == '1') {
      digitalWrite(2, HIGH);
    }
  }
  
}
