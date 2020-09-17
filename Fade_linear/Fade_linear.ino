/*
  Fade

  This example shows how to fade an LED on pin 9 using the analogWrite()
  function.

  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin. On most Arduino, the PWM pins
  are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/

int led = 9;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeCount = 0;     // value to be squared and outputed as brightness
int fadeInc = 1;       // how many will fadeCount increment

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // set the brightness of pin 9:
  analogWrite(led, brightness);

  // increments fadeCount
  fadeCount = fadeCount + fadeInc;

  // change the brightness for next time through the loop:
  brightness = pow(fadeCount, 2);

  // reverse the direction of the fading at the ends of the fade:
  if (brightness >= 255) {
    fadeCount = -fadeCount;
    brightness = 255;
  } else if (brightness <=0) {
    fadeCount = -fadeCount;
    brightness = 0;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(60);
}
