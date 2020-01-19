#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PRINTLN(...)  {if(useSerial) {Serial.println(__VA_ARGS__);}};
#define PRINTLNF(...)  {if(useSerial) {Serial.println(F(__VA_ARGS__));}};
#define PRINT(...)  {if(useSerial) {Serial.print(__VA_ARGS__);}};
#define PRINTF(...)  {if(useSerial) {Serial.print(F(__VA_ARGS__));}};

#define LCD_WIDTH 20
#define LCD_HEIGHT 4

//Displayed text. Change it to match your use case
#define GREETING "Hello <3"
#define GREETING_LENGTH 8

#define COUNTER_SUBJECT "You make me happy"
#define COUNTER_SUBJECT_LENGTH 17
#define SINCE "since..."
#define SINCE_LENGTH 8

#define THAT_IS "That's..."
#define THAT_IS_LENGTH 9

#define OR "or..."
#define OR_LENGTH 5

#define START_TIME_OF_COUNTER_TEXT "23.05.2020"
#define START_TIME_OF_COUNTER_TEXT_LENGTH 10

#define UNIT_SECONDS "seconds"
#define UNIT_SECONDS_LENGTH 7
#define UNIT_MINUTES "minutes"
#define UNIT_MINUTES_LENGTH 7
#define UNIT_HOURS "hours"
#define UNIT_HOURS_LENGTH 5
#define UNIT_DAYS "days"
#define UNIT_DAYS_LENGTH 4
#define UNIT_MONTH "months"
#define UNIT_MONTH_LENGTH 6
#define UNIT_YEAR "years"
#define UNIT_YEAR_LENGTH 5

bool useSerial = true;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// 1 - Show dates
// 2 - Configure time
byte state = 1;

//Time to show the same text in ms
unsigned int textDisplayTime = 5000;
unsigned long timeStampLastChangedText = 0;
byte displayedTextState = 250;

//Fields to cache display state, else it is flickering
bool resetDisplay = false;
byte lastCleanedState = 0;


void setup() {
    // put your setup code here, to run once:
    initLCD();
    printGreeting();
    initSerial();

    resetDisplay = true;

}

void loop() {
    if (resetDisplay) {
        lcd.clear();
        resetDisplay = false;
    }

    switch (state) {
        case 1:
            printDateState();
        default:
            printDateState();
    }

    delay(1000);
    PRINTLNF("----------------------------------");
}

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
}

/**
 * print the greeting message to the display
 */
void printGreeting() {
    prepareCursorCenteredText(GREETING_LENGTH, 0);
    lcd.print(GREETING);
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
 * print the date centered to the display in the given row
 * @param hour the given hour, should be in 24h mode
 * @param minute the minute that should be displayed
 * @param second the second that should be displayed
 * @param day the day that should be displayed
 * @param month the month that should be displayed
 * @param year the year that should be displayed
 * @param row in which the text should be displayed
 */
void printDateTime(byte hour, byte minute, byte second, byte day, byte month, unsigned int year, byte row) {
    lcd.setCursor(0, 3);
    prepareCursorCenteredText(19, row);

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
    lcd.print(year);

}

/**
 * handle the functions if the program is in the date state
 * Displays the passed time to the start date in different units
 */
void dateState() {
    PRINTF("Current state: ");
    PRINTLN(displayedTextState);

    //Divide by amount if case statements
    byte cleanedState = displayedTextState % 7;
    PRINTF("Cleaned state: ");
    PRINTLN(cleanedState);

    //Reset if state change occurred
    if (lastCleanedState != cleanedState) {
        PRINTLNF("Resetting the Display");
        lcd.clear();
    }

    switch (cleanedState) {
        case 0:
            printStartDateCounterLCD();
            break;
        case 1:
            printPassedTimeLCD(151654655, UNIT_SECONDS, UNIT_SECONDS_LENGTH, 1);
            break;
        case 2:
            printPassedTimeLCD(15165465, UNIT_MINUTES, UNIT_MINUTES_LENGTH, 1);
            break;
        case 3:
            printPassedTimeLCD(151654, UNIT_HOURS, UNIT_HOURS_LENGTH, 1);
            break;
        case 4:
            printPassedTimeLCD(15165, UNIT_DAYS, UNIT_DAYS_LENGTH, 1);
            break;
        case 5:
            printPassedTimeLCD(1516, UNIT_MONTH, UNIT_MONTH_LENGTH, 1);
            break;
        case 6:
            printPassedTimeLCD(1, UNIT_YEAR, UNIT_YEAR_LENGTH, 1);
            break;
        default:
            printStartDateCounterLCD();


    }

    printDateTime(9, 10, 59, 1, 1, 2020, 3);

    //Set new value if time has passed or and overflow happened
    if (timeStampLastChangedText + textDisplayTime <= millis() ||
        millis() < timeStampLastChangedText) {

        displayedTextState++;
        timeStampLastChangedText = millis();
    }

    lastCleanedState = cleanedState;

}

/**
 * print the counter start date to the lcd display with the defined reason
 */
void printStartDateCounterLCD() {
    prepareCursorCenteredText(COUNTER_SUBJECT_LENGTH, 0);
    lcd.print(COUNTER_SUBJECT);
    prepareCursorCenteredText(SINCE_LENGTH, 1);
    lcd.print(SINCE);
    prepareCursorCenteredText(START_TIME_OF_COUNTER_TEXT_LENGTH, 2);
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

void configState() {

}