# Creating i2c Device(using ATtiny85, TMP36 and Arduino)

It involves various steps and the steps goes as follows:

1. [Programming the Arduino(so that it can program ATtiny85)](https://github.com/HumberCampusExplorer/CampusExplorer#Step1-:-programming-the-arduino-,-to-program-the-attiny85)
2. [Programming the ATtiny85](https://github.com/six0four/MicroRover#2-bill-of-materials-and-required-tools)
3. [Wiring the circuit(Wiring Arduino + ATtiny85 + TMP36)](https://github.com/six0four/MicroRover#2-bill-of-materials-and-required-tools)
4. [Programming the Arduino- To act as a master device](https://github.com/six0four/MicroRover#3-instructions)
5. [Power up the circuit](https://github.com/six0four/MicroRover#2-bill-of-materials-and-required-tools)
6. [Test it](https://github.com/six0four/MicroRover#2-bill-of-materials-and-required-tools)

## Steps to program ATTiny85
[Follow this link](https://quadmeup.com/attiny85-light-sensor-i2c-slave-device/)

## Step1: Programming the Arduino, to program the ATtiny85
1. Stuff needed and used:-
- Arduino
- Breadboard
- ATtiny85 
- 220ohm 1/4 watt resistor
- LED
- hookup wires

2. Wire the circuit 

Connect the Arduino to the ATtiny as follows:

- Arduino +5V      --->  ATtiny Pin 8,

- Arduino Ground   --->  ATtiny Pin 4,

- Arduino Pin 10   --->  ATtiny Pin 1,

- Arduino Pin 11   --->  ATtiny Pin 5,

- Arduino Pin 12   --->  ATtiny Pin 6,

- Arduino Pin 13   --->  ATtiny Pin 7.

3. Program the Arduino, so that it can program the microcontroller(ATtiny85), using "Arduino" software. To do that 3 steps need to be follow:

->Select the "ArduinoISP" sketch from the "Examples" menu.
->Upload the sketch to Arduino.
->Arduino is now configured as a serial programmer that can program other chips.

4. Download the ATtiny85 core files.

[From this link](http://highlowtech.org/?p=1229)

Unzip the .zip folder and save it in Arduino>libraries>

5. Program the ATtiny85

Select from the top menu:

Tools --> Board --> ATtiny85 (w/ Arduino as ISP)

Then open the basic blink example and change the pin number from 13 to 0 and upload it.

6. Tested the circuit.

![blinkled](https://user-images.githubusercontent.com/43186746/48095922-f3dd6a00-e1e3-11e8-8093-7cb11453cb22.jpeg)
and it shows that now arduino can program the ATtiny 85.

## Step2: Programming the ATtiny85 
so that it can retrieve data(Analog value) from sensor(TMP36) and convert it to digital and send it to Raspberry pi, when requested.

First i am testing my circuit and program on Arduino(instead of Raspberry pi directly), to make troubleshooting easier.

For that i uploaded this code on ATtiny85
```
/*
 * Set I2C Slave address. You can have multiple sensors with different addresses
 */
#define I2C_SLAVE_ADDRESS 0x30

/*
 * How often measurement should be executed
 * One tick is more less 0.5s, so 1 minute is 120 ticks
 * This executes measurements every 2 minutes, so 120s, 120000 ms
 */
#define MAX_TICK 2

#define STATUS_PIN_1 4
#define ADC_PIN A3

//#define LPF_FACTOR 0.5

#include <TinyWireS.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

volatile uint8_t i2c_regs[] =
{
    0, //older 8
    0 //younger 8
};

volatile byte reg_position = 0;
const byte reg_size = sizeof(i2c_regs);

/*
 * 0=16ms
 * 1=32ms
 * 2=64ms
 * 3=128ms
 * 4=250ms
 * 5=500ms
 * 6=1 sec,
 * 7=2 sec,
 * 8=4 sec,
 * 9= 8sec
 */
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  
  // start timed sequence
  
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

/*
 * Watchdog Interrupt Service is executed when  watchdog timed out
 */
ISR(WDT_vect) {
  digitalWrite(STATUS_PIN_1, LOW);
}

volatile unsigned int lightMeter;

/**
 * This function is executed when there is a request to read sensor
 * To get data, 2 reads of 8 bits are required
 * First requests send 8 older bits of 16bit unsigned int
 * Second request send 8 lower bytes
 * Measurement is executed when request for first batch of data is requested
 */
void requestEvent()
{  
  TinyWireS.send(i2c_regs[reg_position]);

  reg_position++;
  if (reg_position >= reg_size)
  {
      reg_position = 0;
  }
}

void setup() {
  /*
   * Setup I2C
   */
  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onRequest(requestEvent);

  /*
   * Set pins
   */
  pinMode(STATUS_PIN_1, OUTPUT);
  pinMode(ADC_PIN, INPUT);

  /*
   * Start watchdog timer
   */
  setup_watchdog(6);
}

unsigned int tick = 0;
unsigned long lastReadout = 0;

int smooth(int data, float filterVal, float smoothedVal){

  if (filterVal > 1){      // check to make sure params are within range
    filterVal = .99;
  }
  else if (filterVal <= 0){
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;
}

void loop() {

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable(); 

  unsigned long currentMillis = millis();
  /*
   * On tick value 0, do measurements
   */
  if (abs(currentMillis - lastReadout) > MAX_TICK) {
    int raw_value = analogRead(ADC_PIN);

    lightMeter = smooth(raw_value, LPF_FACTOR, lightMeter);
    
    i2c_regs[0] = lightMeter >> 8;
    i2c_regs[1] = lightMeter & 0xFF;
    
    digitalWrite(STATUS_PIN_1, HIGH);
    lastReadout = currentMillis;
  }
}
```
# Step3: Wiring the circuit(Wiring Arduino + ATtiny85 + TMP36)

ATtiny 85 & TMP36
- Pin2 (ATtiny85) -> centre pin(tmp36)
- Pin 1(TMP36) -> 5v
- Pin3 (TMP36) -> Ground

To check pins of TMP36

![tmp36pinout](https://user-images.githubusercontent.com/43186746/49828583-461d2800-fd5a-11e8-84dd-abef1abda002.gif)


Arduino & ATtiny85
- Pin4(ATtiny85) - Ground
- Pin8(ATtiny85) - Vcc(5v)
- Pin5(ATtiny85) - SDA (Arduino)
- Pin7(ATtiny85) -SCL (Arduino)
We need to insert pull-up resistor (nearly 4.7k ohm ) between pin 5 (ATtiny85) and Vcc && between pin 7 (ATtiny85) and Vcc.

Then attach the auduino with USB cable type A/B with computer to power up Arduino.

![arduino attiny85](https://user-images.githubusercontent.com/43186746/49829572-c5136000-fd5c-11e8-8961-d992e243ce99.jpg)

# Step4: Programming the Arduino- To act as a master device

For this i just uploaded a sketch to arduino.

For that i changed the options from ARDUINO software=> 
- Tools-> Board:"Arduino/Genuino uno" and 
- Tools-> Programmer:"AVR ISP"

AND then uploaded from "sketch"

```
#include <Wire.h>

int i = 0;
unsigned int readout = 0;

void setup() {
  Serial.begin(9600);  // start serial for output
  Serial.println("starting");
  Wire.begin();        // join i2c bus (address optional for master)
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
  
 Serial.println(readout);
 delay(13);

}
```

# Step 5 : Power up the circuit
Now attach (USB cable type A/B) to computer, to power it and complete the circuit and test it. 

# Step6 : Test it
Then, after completing all the steps I checked the output at Serial Monitor. 

and it gave me this output(it is not the correct output, but it is in the pattern which we are looking for):

![output](https://user-images.githubusercontent.com/43186746/49823533-ea4ca200-fd4d-11e8-982f-1c77af3e9c27.png)

And now i am working on modifing the code so that it can show the correct and needed output.

