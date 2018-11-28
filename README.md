# Steps to program ATTiny85

[Follow this link](https://quadmeup.com/attiny85-light-sensor-i2c-slave-device/)

# Step1: Programming the Arduino, to program the ATtiny85
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
- Arduino Pin 13   --->  ATtiny Pin 7
3. Program the Arduino, so that it can program the microcontroller(ATtiny85), using "Arduino" software. To do that 3 steps need to be follow: 
->Select the "ArduinoISP" sketch from the "Examples" menu.
->Upload the sketch to Arduino.
->Arduino is now configured as a serial programmer that can program other chips.
4. Download the ATtiny85 core files
[From this link](http://highlowtech.org/?p=1229)
Unzip the .zip folder and save it in Arduino>libraries>
5. Program the ATtiny85
Select from the top menu:
Tools --> Board --> ATtiny85 (w/ Arduino as ISP)
Then open the basic blink example and change the pin number from 13 to 0 and upload it.
6. Tested the circuit.
![blinkled](https://user-images.githubusercontent.com/43186746/48095922-f3dd6a00-e1e3-11e8-8093-7cb11453cb22.jpeg)
and it shows that now arduino can program the ATtiny 85.

# Step2: Program ATtiny85 so that it can retrieve data(Analog) from sensor(TMP36) and convert it to digital and send it to Raspberry pi, 

# Still needed for week 15
Introduction using a system diagram
Bill of Materials/Budget
Time Commitment
Mechanical Assembly
PCB / Soldering
Power Up
Unit Testing
Production Testing
