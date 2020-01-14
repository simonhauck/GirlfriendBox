#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PRINTLN(...)  {if(useSerial) {Serial.println(__VA_ARGS__);}};
#define PRINTLNF(...)  {if(useSerial) {Serial.println(F(__VA_ARGS__));}};
#define PRINT(...)  {if(useSerial) {Serial.print(__VA_ARGS__);}};
#define PRINTF(...)  {if(useSerial) {Serial.print(F(__VA_ARGS__));}};

bool useSerial = true;


//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
    // put your setup code here, to run once:
    initLCD();
    initSerial();

}

void loop() {
    // put your main code here, to run repeatedly:
    Serial.println("Hello world");
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print(millis());
    delay(1000);
}

void initSerial() {
    lcd.setCursor(6, 0);
    lcd.print("Hello <3");
    lcd.setCursor(5, 2);
    lcd.print("<Starting>");

    Serial.begin(9600);
    lcd.setCursor(0, 3);

    //Wait for serial
    for (int i = 0; i < 20; i++) {
        lcd.print(".");
        delay(100);
    }

    //If Serial is not initialized, set flag to false
    if (!Serial) {
        useSerial = false;
    } else {
        useSerial = true;
        PRINTLN("Serial initialized.");
    }

}

void initLCD() {
    lcd.begin(20, 4);
    lcd.clear();
}
