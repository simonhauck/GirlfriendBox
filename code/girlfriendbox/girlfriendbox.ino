#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

#define PRINTLN(...)  {if(useSerial) {Serial.println(__VA_ARGS__);}};
#define PRINTLNF(...)  {if(useSerial) {Serial.println(F(__VA_ARGS__));}};
#define PRINT(...)  {if(useSerial) {Serial.print(__VA_ARGS__);}};
#define PRINTF(...)  {if(useSerial) {Serial.print(F(__VA_ARGS__));}};

#define LCD_WIDTH 20
#define LCD_HEIGHT 4

#define START_TIME_OF_COUNTER_TEXT "23.05.2020"
//If you are in a different timezone, you have to add those to the timestamp
//Timestamp 23.05.2020 +1h because of timezone
#define START_TIME_OF_COUNTER_UNIX_TIMESTAMP 1590195600

#define STATE_SHOW_DATE 0
#define STATE_CONFIG 1

//----------------------------------------------------------------------------------------------------------------------
// Displayed Text (Change for your language)
//----------------------------------------------------------------------------------------------------------------------

//Displayed text. Change it to match your use case
#define GREETING "Hello"

#define COUNTER_SUBJECT "You make me happy"
#define SINCE "since..."

#define THAT_IS "That's..."

#define OR "or..."

#define UNIT_SECONDS "seconds"
#define UNIT_MINUTES "minutes"
#define UNIT_HOURS "hours"
#define UNIT_DAYS "days"
#define UNIT_MONTH "months"
#define UNIT_YEAR "years"

//----------------------------------------------------------------------------------------------------------------------
// Fields & Variables
//----------------------------------------------------------------------------------------------------------------------

//Custom lcd Chars
byte lcdHeartChar[8] = {0x00, 0x0a, 0x1f, 0x1f, 0x0e, 0x04, 0x00};
byte lcdUpArrowChar[8] = {0x00, 0x00, 0x00, 0x00, 0x04, 0x0E, 0x1F, 0x1F};
byte lcdDownArrowChar[8] = {0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00, 0x00, 0x00};

//Pins for the buttons, configButton is an interrupt
const int configButtonPin = 3;
const int upButtonPin = 4;
const int downButtonPin = 5;

//Indicate if serial should be used
//If no usb device is connected, serial can not be opened
//Value is set automatically on startup
bool useSerial = true;

//LCD Display
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//Clock objects
DS3231 rtcClock;
RTClib rtcLibClock;

//Button flag
volatile bool configButtonPressed = false;

//DateTime values
byte hour;
byte minute;
byte second;
byte day;
byte month;
int year;
long currentUnixTime;

// 0 - Show dates
// 1 - Configure time
byte state = STATE_SHOW_DATE;

//Time to show the same text in ms
unsigned int textDisplayTime = 10000;
unsigned long timeStampLastChangedText = 0;
byte displayedTextState = 0;
//Fields to cache display state to prevent flickering
byte lastDisplayedTextState = 0;

//Config state variables
byte configDateTimeState = 0;
int downButtonLastState = HIGH;
int upButtonLastState = HIGH;


//----------------------------------------------------------------------------------------------------------------------
// Arduino functions
//----------------------------------------------------------------------------------------------------------------------

void setup() {
    // put your setup code here, to run once:

    //Set pins
    pinMode(configButtonPin, INPUT_PULLUP);
    pinMode(upButtonPin, INPUT_PULLUP);
    pinMode(downButtonPin, INPUT_PULLUP);

    initLCD();
    printGreeting();
    initSerial();

    //Initially reset lcd and display first text
    lcd.clear();
    timeStampLastChangedText = millis();

    //Attack interrupt
    attachInterrupt(digitalPinToInterrupt(configButtonPin), configButtonISR, FALLING);

}

void loop() {
    //Switch to config
    if (configButtonPressed && state != STATE_CONFIG) {
        PRINTLN("ConfigButton was pressed. Switch to config mode");
        configButtonPressed = false;
        state = STATE_CONFIG;
        configDateTimeState = 0;
    }

    switch (state) {
        case STATE_SHOW_DATE:
            dateState();
            break;
        case STATE_CONFIG:
            configState();
            break;
        default: PRINTLNF("WARNING: Default State in loop() used!");
            dateState();
    }

    PRINTLNF("----------------------------------");

    //Go to sleep / delay state = SHOW DATES
    if (state == STATE_SHOW_DATE) {
        delay(1000);
    }
}

/**
 * change the state to CONFIG and set the flag that the button was pressed and the display should be resetted
 */
void configButtonISR() {
    configButtonPressed = true;
}

//----------------------------------------------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------------------------------------------

/**
 * initialize the serial module if usb is connected
 */
void initSerial() {
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

/**
 * init the lcd display and clear all content
 */
void initLCD() {
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    lcd.clear();

    //Create custom chars
    lcd.createChar(1, lcdHeartChar);
    lcd.createChar(2, lcdUpArrowChar);
    lcd.createChar(3, lcdDownArrowChar);
}

/**
 * print the greeting message to the display
 */
void printGreeting() {
    prepareCursorCenteredText(strlen(GREETING) + 4, 1);
    lcd.write(1);
    lcd.print(" ");
    lcd.print(GREETING);
    lcd.print(" ");
    lcd.write(1);
}

/**
 * handle the functions if the program is in the date state
 * Displays the passed time to the start date in different units
 */
void dateState() {
    PRINTF("DateState() Function State: ");
    PRINTLN(displayedTextState);

    getClockValues(hour, minute, second, day, month, year, currentUnixTime);
    printDateTime(hour, minute, second, day, month, year, currentUnixTime);

    //Reset if state change occurred
    if (lastDisplayedTextState != displayedTextState) {
        PRINTLNF("Resetting the Display");
        lcd.clear();
    }

    switch (displayedTextState) {
        case 0:
            printStartDateCounterLCD();
            break;
        case 1:
            printPassedTimeLCD(151654655, UNIT_SECONDS, strlen(UNIT_SECONDS), 1);
            break;
        case 2:
            printPassedTimeLCD(15165465, UNIT_MINUTES, strlen(UNIT_MINUTES), 1);
            break;
        case 3:
            printPassedTimeLCD(151654, UNIT_HOURS, strlen(UNIT_HOURS), 1);
            break;
        case 4:
            printPassedTimeLCD(15165, UNIT_DAYS, strlen(UNIT_DAYS), 1);
            break;
        case 5:
            printPassedTimeLCD(1516, UNIT_MONTH, strlen(UNIT_MONTH), 1);
            break;
        case 6:
            printPassedTimeLCD(1, UNIT_YEAR, strlen(UNIT_YEAR), 1);
            break;
        default:
            printStartDateCounterLCD();


    }

    printDateTimeLCD(hour, minute, second, day, month, year, 3, true);

    //Save last state before updating
    lastDisplayedTextState = displayedTextState;

    //Set new value if time has passed or and overflow happened
    if (timeStampLastChangedText + textDisplayTime <= millis() ||
        millis() < timeStampLastChangedText) {

        //Handle overflow
        displayedTextState = (displayedTextState + 1) % 7;
        timeStampLastChangedText = millis();
    }
}

void configState() {
    PRINTF("ConfigState() Function, State: ");
    PRINTLN(configDateTimeState);

    bool printFullDate = true;

    // The start position of the lcd
    byte lcdStartPosition = (LCD_WIDTH - (printFullDate ? 19 : 17)) / 2;

    //Set headline
    lcd.clear();
    prepareCursorCenteredText(15, 0);
    lcd.print("Set Date & Time");

    switch (configDateTimeState) {
        case 0:
            //Set hour:
            printArrowsLCD(2, lcdStartPosition, 1, true);
            printArrowsLCD(2, lcdStartPosition, 3, false);
            printDateTimeLCD(hour, minute, second, day, minute, year, 2, printFullDate);
            PRINTLNF("Configure hour...");
            break;
        case 1:
            //Set minute:
            printArrowsLCD(2, lcdStartPosition + 3, 1, true);
            printArrowsLCD(2, lcdStartPosition + 3, 3, false);
            printDateTimeLCD(hour, minute, second, day, minute, year, 2, printFullDate);
            PRINTLNF("Configure minute...");
            break;
        case 2:
            //Set second:
            printArrowsLCD(2, lcdStartPosition + 6, 1, true);
            printArrowsLCD(2, lcdStartPosition + 6, 3, false);
            printDateTimeLCD(hour, minute, second, day, minute, year, 2, printFullDate);
            PRINTLNF("Configure second...");
            break;
        case 3:
            //Set day:
            printArrowsLCD(2, lcdStartPosition + 9, 1, true);
            printArrowsLCD(2, lcdStartPosition + 9, 3, false);
            printDateTimeLCD(hour, minute, second, day, minute, year, 2, printFullDate);
            PRINTLNF("Configure day...");
            break;
        case 4:
            //Set month:
            printArrowsLCD(2, lcdStartPosition + 12, 1, true);
            printArrowsLCD(2, lcdStartPosition + 12, 3, false);
            printDateTimeLCD(hour, minute, second, day, minute, year, 2, true);
            PRINTLNF("Configure month...");
            break;
        case 5:
            printArrowsLCD((printFullDate ? 4 : 2), lcdStartPosition + 15, 1, true);
            printArrowsLCD((printFullDate ? 4 : 2), lcdStartPosition + 15, 3, false);
            //Set year:
            printDateTimeLCD(hour, minute, second, day, minute, year, 2, true);
            PRINTLNF("Configure year...");
            break;
        default:
            state = STATE_SHOW_DATE;
            lcd.clear();
            //Restart counter on other state
            displayedTextState = 0;
            PRINTLNF("Config complete. Return to show date mode...");
    }


    if (configButtonPressed) {
        configButtonPressed = false;
        configDateTimeState++;
    }

    delay(250);
}

/**
 * set the cursor so, that the text width the given length will be centered
 * @param textLength the length of the text
 * @param row the row to which the cursor should be set
 */
void prepareCursorCenteredText(byte textLength, byte row) {
    if (textLength >= LCD_WIDTH) {
        lcd.setCursor(0, row);
    } else {
        byte position = (LCD_WIDTH - textLength) / 2;
        lcd.setCursor(position, row);
    }
}

/**
 * print the values of the date to the serial monitor
 * @param hour value that should be displayed
 * @param minute value that should be displayed
 * @param second value that should be displayed
 * @param day value that should be displayed
 * @param month value that should be displayed
 * @param year value that should be displayed
 * @param unixTimeStamp value that should be displayed
 */
void printDateTime(byte hour, byte minute, byte second, byte day, byte month, int year, long unixTimeStamp) {
    PRINT(hour);
    PRINT(":");
    PRINT(minute);
    PRINT(":");
    PRINT(second);
    PRINT(" ");
    PRINT(day);
    PRINT(".");
    PRINT(month);
    PRINT(".");
    PRINT(year);
    PRINT(", unixTimeStamp: ");
    PRINTLN(unixTimeStamp);
}

/**
 * print the date centered to the display in the given row
 * @param hour the given hour, should be in 24h mode
 * @param minute the minute that should be displayed
 * @param second the second that should be displayed
 * @param day the day that should be displayed
 * @param month the month that should be displayed
 * @param year the year that should be displayed
 * @param row in which the text should be displayed
 * @param printFullYear True if the year should be printed. Eg. true = 2019 false = 19
 * @param century20th True if the year is 20** or false if the year is 19**
 */
void printDateTimeLCD(byte hour, byte minute, byte second, byte day, byte month, int year, byte row,
                      bool printFullYear) {
    //Depending on the year the text has 19 or 17 chars
    prepareCursorCenteredText((printFullYear ? 19 : 17), row);

    if (hour < 10) {
        lcd.print(" ");
    }
    lcd.print(hour);

    lcd.print(":");
    if (minute < 10) {
        lcd.print("0");
    }
    lcd.print(minute);

    lcd.print(":");
    if (second < 10) {
        lcd.print("0");
    }
    lcd.print(second);

    lcd.print(" ");

    if (day < 10) {
        lcd.print("0");
    }
    lcd.print(day);

    lcd.print(".");
    if (month < 10) {
        lcd.print("0");
    }
    lcd.print(month);

    lcd.print(".");
    lcd.print((printFullYear ? year : year - 2000));
}

/**
 * print the counter start date to the lcd display with the defined reason
 */
void printStartDateCounterLCD() {
    prepareCursorCenteredText(strlen(COUNTER_SUBJECT), 0);
    lcd.print(COUNTER_SUBJECT);
    prepareCursorCenteredText(strlen(SINCE), 1);
    lcd.print(SINCE);
    prepareCursorCenteredText(strlen(START_TIME_OF_COUNTER_TEXT), 2);
    lcd.print(START_TIME_OF_COUNTER_TEXT);
}

/**
 * print the given time and unit to the display in the given row
 * @param value the time value that should be displayed
 * @param unit the unit of the time that should be displayed
 * @param textLengthUnit the length of the unit string
 * @param row the row in which the characters should be displayed
 */
void printPassedTimeLCD(unsigned long value, char unit[], byte textLengthUnit, byte row) {
    byte totalLength = 1 + getLengthOfNumber(value) + textLengthUnit;
    prepareCursorCenteredText(totalLength, row);
    lcd.print(value);
    lcd.print(" ");
    lcd.print(unit);
}

/**
 * get the length of the given number
 * @param value the number from which the length will be returned
 * @return the length of the number
 */
unsigned int getLengthOfNumber(unsigned long value) {
    unsigned int counter = 1;
    unsigned long divisionValue = 1;

    while (true) {
        divisionValue *= 10;
        unsigned long divisionResult = value / divisionValue;
        if (divisionResult >= 1) {
            counter++;
        } else {
            return counter;
        }
    }
}

/**
 * set the time of the rtc clock
 * @param hour the current hour value
 * @param minute the current minute value
 * @param second the current second value
 * @param day the current day value
 * @param month the current month value
 * @param year the current year value
 * @param clockMode the clock mode. EG true = 24h false = 12h clock
 */
void setClockTime(byte hour, byte minute, byte second, byte day, byte month, int year, bool clockMode) {
    rtcClock.setClockMode(false);
    //Needs 1 byte as value -> Lib adds 2000 + offset
    byte tmpYear = (year - 2000);
    rtcClock.setYear(tmpYear);
    rtcClock.setMonth(month);
    rtcClock.setDate(day);
    rtcClock.setHour(hour);
    rtcClock.setMinute(minute);
    rtcClock.setSecond(second);
}

/**
 * get the values from the rtc and write the values in the given parameters
 * @param hour write result hour value of the rtc into the parameter
 * @param minute write result minute value of the rtc into the parameter
 * @param second write result second value of the rtc into the parameter
 * @param day write result day value of the rtc into the parameter
 * @param month write result month value of the rtc into the parameter
 * @param year write result hour value from the rtc in parameter -> can only display values 20**
 * @param currentUnixTime write result unix timestamp value (in sec) of the rtc into the parameter
 */
void getClockValues(byte &hour, byte &minute, byte &second, byte &day, byte &month, int &year, long &currentUnixTime) {
    DateTime now = rtcLibClock.now();
    hour = now.hour();
    minute = now.minute();
    second = now.second();
    day = now.day();
    month = now.month();
    year = now.year();
    currentUnixTime = now.unixtime();
}

/**
 * print the arrows on the lcd at the given position
 * @param amount the amount of arrows that should be printed
 * @param xPos the x position on the lcd of the first char
 * @param yPos the y position on the lcd of the first char
 * @param up true if the arrows should face up
 */
void printArrowsLCD(byte amount, byte xPos, byte yPos, bool up) {
    lcd.setCursor(xPos, yPos);
    for (int i = 0; i < amount; i++) {
        lcd.write((up) ? 2 : 3);
    }
}