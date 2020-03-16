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

## Upload the code
After creating the circuit open the “girlfriendbox.ino” file with the Arduino IDE or something similar. Add the required libraries from the “lib” folder. In the Arduino IDE you can do this by clicking Sketch > Include Library > Add .ZIP Library. 
Now you should be able to upload the code to the Arduino and the program should run.

```C++
#define START_TIME_OF_COUNTER_TEXT "23.05.2019"
#define START_TIME_OF_COUNTER_UNIX_TIMESTAMP 1558569600
#define START_TIME_YEAR 2019
#define START_TIME_MONTH 5
#define START_TIME_DAY 23
```
The first one is the date shown on the display, the second is the same date as unix-timestamp, which is used to calculate the passed seconds, minutes, hours and days. You can use any online converter to get this timestamp. Note: You should add an offset for your timezone. So if you are in timezone +1 and your date is “23.05.2019 00:00” calculate the timestamp with “23.05.2019 01:00”. Last, add the date values for the day, month and year field. Personally I would prefer to calculate these values, but this would be a really big pain and soooo I ignored it... Anyways these are used to calculate the passed months and years, and of course, they only change on the monthly and yearly anniversary. 

Now you can change some more minor details, like the welcome text which will be shown initally and the units for seconds, month, ... , year if you prefer a different language. 

Upload the code to the arduino and set the current date and time. You can do this by pressing the config button (Pin 3). Increase and decrease the values with the buttons on Pin 4 and 5. If you are happy with a value press the config button again.

## Build the case
