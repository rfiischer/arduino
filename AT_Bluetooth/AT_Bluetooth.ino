#include <SoftwareSerial.h>

// Macro para os pinos 8 e 9 como receptor e transmissor
#define RX 8
#define TX 9

// Cria uma interface por bit banging (software serial) nos pinos 8, receptor, e 9 transmissor
// O nome do objeto que representa a interface pode ser qualquer um, aqui, é bluet
SoftwareSerial bluet(RX, TX);

// Os pinos RX e TX nativos do Arduino (pinos 0 e 1 na plaquinha) estão roteados com a interface USB
//e são utilizados quando o Arduino está conectado ao computador, por isso usa-se bit banging.

void setup() {
  // Inicia com baud 9600 a interface serial com o computador
  Serial.begin(9600);
  Serial.println("Digite os comandos AT:");

  // Inicia a interface serial por bit banging
  bluet.begin(9600);

}

void loop() {
  // Se a interface dos pinos 8 e 9 recebeu um byte, escrever o bite recebido na interface USB
  if (bluet.available()) {
    Serial.write(bluet.read());
  }

  // Se algo foi escrito na interface USB, escrever usando os pinos 8 e 9
  if (Serial.available()) {
    bluet.write(Serial.read());
  }
}
