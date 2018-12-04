#include <Wire.h>

int i = 0;
unsigned int readout = 0;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("starting");
}

void loop() {
  Wire.requestFrom(0x30, 2);    // request 2 bytes from slave device #0x13

  int i = 0;
  unsigned int readout = 0;
  while (Wire.available()) { // slave may send less than requested
    byte byteRecieved = Wire.read(); // receive a byte as character

    if (i == 0) {
      readout = byteRecieved;
    } else {
      readout = readout << 8;
      readout = readout + byteRecieved;
    }

    i++;
  }
 delay(100);

}
