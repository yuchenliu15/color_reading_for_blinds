#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200); 
  Serial.println("Color View Test!");

  //Ensure arduino is ready before connecting to color sensor
  while(!Serial){
  }
//  
  if (tcs.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // use these three pins to drive an LED
#if defined(ARDUINO_ARCH_ESP32)
  ledcAttachPin(redpin, 1);
  ledcSetup(1, 12000, 8);
  ledcAttachPin(greenpin, 2);
  ledcSetup(2, 12000, 8);
  ledcAttachPin(bluepin, 3);
  ledcSetup(3, 12000, 8);
#else
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
#endif

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
  }
}

void loop() {

  float red, green, blue; // [0-255] RGB value.
  
  tcs.setInterrupt(false);  // turn on LED
  delay(500);  // read every 500 ms (half sec)
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true);  // turn off LED
  red = (int)red;
  green = (int)green;
  blue = (int)blue;
  /*
    *Access the RGB variable red, green, blue HERE
    *Put the code after this comment
  */

  unsigned long hex_color = ((int(red) & 0xff) << 16) + ((int(green) & 0xff) << 8) + (int(blue) & 0xff); //easy to categorize when it's in hex
  
  //color array: red0, orange1, yellow2, green3, blue4, purple5, white6, black7 
  
  Serial.print("R:\t"); Serial.print(red); 
  Serial.print("\tG:\t"); Serial.print(green); 
  Serial.print("\tB:\t"); Serial.print(blue);
  Serial.print("\n");
  Serial.print(hex_color,HEX);
  Serial.print("\n");

  

#if defined(ARDUINO_ARCH_ESP32)
  ledcWrite(1, gammatable[(int)red]);
  ledcWrite(2, gammatable[(int)green]);
  ledcWrite(3, gammatable[(int)blue]);
#else
  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);
#endif
}
