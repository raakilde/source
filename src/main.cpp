#include <Arduino.h>
#include <TM1637Display.h>
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
// The amount of time (in milliseconds) between tests
#define TEST_DELAY 2000
const uint8_t SEG_DONE[] = {
SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, // d
SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
SEG_C | SEG_E | SEG_G, // n
SEG_A | SEG_D | SEG_E | SEG_F | SEG_G // E
};
TM1637Display display(CLK, DIO);
static uint8_t data[] = { 0x00, 0x00, 0x00, 0x00 };
static uint8_t value[] = { 0x00, 0x00, 0x00, 0x00 } ;

static uint_fast16_t countDownNumber = 100;
static unsigned long time;
static uint8_t runningCountdown = 0;


void setup()
{
  display.setBrightness(254);
  // tone(6, 5000, 5000); // Send 1KHz sound signal...
  Serial.begin(9600);
  display.setSegments(data);
}

void leftShiftDisplay(int newValue)
{
  if(data[1] != 0)
  {
    data[0] = data[1];
    value[0] = value[1];
  }
  if(data[2] != 0)
  {
    data[1] = data[2];
    value[1] = value[2];
  }
  if(data[3] != 0)
  {
    data[2] = data[3];
    value[2] = value[3];
  }

  data[3] = display.encodeDigit(newValue);
  value[3] = (uint8_t)newValue - 48;
  // Serial.println(value[3]);
}

void displayNumber(int toDisplay) 
{
  // Serial.println(toDisplay);
  int number = 0;
  int left = toDisplay;

  if(toDisplay > 999)
  {
    number = toDisplay / 1000;
    data[0] = display.encodeDigit(number);
    value[0] = number;
    //Serial.print(number);
    left -= number * 1000;
  }
  else
  {
    data[0] = 0;
    value[0] = 0;
  }
  

  if(toDisplay > 99)
  {
    number = left / 100;
    data[1] = display.encodeDigit(number);
    value[1] = number;
    // Serial.print(number);
    left -= number*100;
  }
  else
  {
    data[1] = 0;
    value[1] = 0;
  }

  if(toDisplay > 9)
  {
    number = left / 10;
    data[2] = display.encodeDigit(number);
    value[2] = number;
    // Serial.print(number);
    left -= number * 10;
  }
  else
  {
    data[2] = 0;
    value[2] = 0;
  }

  data[3] = display.encodeDigit(left);
  value[3] = left;
  // Serial.println(value[3]);
  display.setSegments(data);
}

void loop()
{
  
  char customKey = customKeypad.getKey();
  if (customKey)
  {
    // Serial.println(customKey);
    
    if(customKey == '#')
    {
      if (value[3] != 0)
      {
        tone(6, 1000, 100); // Send 1KHz sound signal...
        countDownNumber = (uint_fast16_t)
                          ((uint_fast16_t)value[0] * 1000) + 
                          ((uint_fast16_t)value[1] * 100) + 
                          ((uint_fast16_t)value[2] * 10) + 
                          ((uint_fast16_t)value[3]);
        countDownNumber += millis() / 1000;

        // Serial.println(countDownNumber);
        runningCountdown = 1;
      }
    }
    else if(customKey == '*')
    {
      data[0] = 0; data[1] = 0;data[2] = 0;data[3] = 0;
      value[0] = 0; value[1] = 0;value[2] = 0;value[3] = 0;
      display.setSegments(data);
      runningCountdown = 0;
    }
    else if (runningCountdown == 0)
    {
      leftShiftDisplay(customKey);
      display.setSegments(data);
    }
  }
  
  if(runningCountdown == 1)
  {
    //long startTime = millis();
    if((millis()/1000) < countDownNumber){
      displayNumber(countDownNumber -(millis()/1000));
    }
    else
    {
      displayNumber(0);
      runningCountdown = 2;
      countDownNumber += 3;
      tone(6, 1000, 3500);  
    }    
  }
  else if(runningCountdown == 2)
  {
      if((millis()/1000) > countDownNumber)
      {
        data[0] = 0; data[1] = 0;data[2] = 0;data[3] = 0;
        value[0] = 0; value[1] = 0;value[2] = 0;value[3] = 0;
        display.setSegments(data);
        runningCountdown = 0;
      } 
  }
  
}
