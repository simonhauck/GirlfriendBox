# GirlfriendBox_Present
This is a self-made present for your girlfriend or wife.

<p align="center">
  <img src="img/Box_front_date_text.jpeg" width= 400>
  <img src="img/Box_front_seconds.jpeg" width= 400>
</p>

It is a box, that counts how long you have been together or married in different units. It cycles through seconds, minutes, hours, days, months, years as well as a customizable text, that shows your “Date”. Additionally, it also displays the current date & time at the bottom of the display. 

The project idea is inspired by this [video](https://www.youtube.com/watch?v=PbRNsSK7r4M), but the code as well as the box were created by myself.

## What you need
- 1 Arduino, which is the "brain". I used the Arduino Nano V3
- 1 Real-Time-Clock Module, which keeps track of the time. I used the DS3231
- 1 LCD-Dispaly with 20 * 4 characters
- 3 Buttons to set the current time
- 2 Switches. The first is for power and the second for the backlicht of the display
- Optionally
  - 1 Lipo-Battery, so that you don't require always a power cable. I used a model with 3.7V and 2600mAh
  - 1 Lipo-Charging-Circuit, so that you can charge the battery 
  - 1 Boost-Converter, which increases the battery voltage to 7 Volts
  - Some soldering gear, if you want to make a nice PCB

## Build the circuit
<p align="center">
  <img src="wiring_diagram/girlfriendbox_bb.png" width= 800>
</p>
To build the circuit gather all the components and create the wiring according to the scheme provided in the “wiring_diagram” folder. Configure the boost-converter to output 7V. You can do this with a multimeter. Connect the probes to the + Pin  and - Pin of the boost-converter and measure the voltage. Rotate the potentiometer on the boost-convert until the multimeter shows 7V.

Note that this diagram contains only one switch for the power. The second is connected to the backlight pins of the I2C-Display. And now you are already done with the circuit :D yeahh 

## Build the case
