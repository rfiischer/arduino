#include <SoftwareSerial.h>

#define RX 8
#define TX 9

SoftwareSerial bluet(RX, TX);

bool rd = false;
String comm = "";

void setup() {
  Serial.begin(9600);
  Serial.println("Digite os comandos AT:");

  bluet.begin(9600);

}

void loop() {
  if (bluet.available()) {
    if (!rd) {
      if (bluet.read() == ";"){
        rd = !rd;
      } else {
        comm += (char)bluet.read();
      }
    }
  }

  if (rd) {
    
  }

  if (Serial.available()) {
    bluet.write(Serial.read());
  }
}
