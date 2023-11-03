
#include <Arduino.h>
#include <LiquidCrystal.h>

#define _LCD_COLUMNS 16
#define _LCD_ROWS 2

struct KeyValue {
    int key;
    char value[15];
};

struct MenuItem {
    const KeyValue &interfaceItem;
    byte valueSerial[16];
    MenuItem* previous;
    MenuItem* next;
};

// Relle connection
#define _RED_LED    3 // PD3 // 3
#define _GREEN_LED  2 // PD2 // 2
#define _BLUE_LED   12 // PB2 // 12
#define _WHITE_LED  11 // PB3 // 11
// initialize the Relay
// with the arduino pin number it is connected to UNO
const unsigned int relayRed = _RED_LED, relayGreen = _GREEN_LED, relayBlue = _BLUE_LED, relayWhite = _WHITE_LED;

// with the arduino pin number it is connected to UNO
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // PB0; PB1; PD4; PD5; PD6; PD7
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
 
//Key message
char msgs[5][15] = {
                    "Right Key OK  ", 
                    "Up Key OK     ", 
                    "Down Key OK   ", 
                    "Left Key OK   ", 
                    "Select Key OK " };

KeyValue msgInterface[] = {
    {0,           "Source set    "},
    {_RED_LED,    "Red light     "},
    {_GREEN_LED,  "Green light   "},
    {_BLUE_LED,   "Blue light    "},
    {_WHITE_LED,  "White light   "},
    {5,           "Start         "},
    {6,           "Stop          "},
    {7,           "Send once     "},
    {8,           "Send continue "}
};

MenuItem menuItems[] = {
    {msgInterface[0], {0}, nullptr, nullptr},
    {msgInterface[1], {0}, nullptr, nullptr},
    {msgInterface[2], {0}, nullptr, nullptr},
    {msgInterface[3], {0}, nullptr, nullptr},
    {msgInterface[4], {0}, nullptr, nullptr},
    {msgInterface[5], {0}, nullptr, nullptr},
    {msgInterface[6], {0}, nullptr, nullptr},
    {msgInterface[7], {0}, nullptr, nullptr},
    {msgInterface[8], {0}, nullptr, nullptr}
};

MenuItem* currentMenuItem = &menuItems[0]; // Start from the first item

// variables
const unsigned int  adc_key_val[5] ={30, 150, 360, 535, 760};
int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;

// prototypes
void initRelayLed(void);
void initLcd(void);
void initSerial(void);
void serialCommunication();
int get_key(unsigned int input);
void keyPad(void);
void setLed(const int &menuItemKey);
void startLed(unsigned int ledPin);
void stopLed(unsigned int ledPin);
void stateM(const unsigned int &pressKey);
void configureNode();
//************************************************************
//************************************************************
void setup() {

  initRelayLed();
  initLcd();
  initSerial();

  //configureNode();
  
}

void loop() {
  keyPad();
  //serialCommunication();
  //delay(1000);
}
//************************************************************
//************************************************************


void initRelayLed(void) {
  pinMode(relayRed, OUTPUT);
  pinMode(relayGreen, OUTPUT);
  pinMode(relayBlue, OUTPUT);
  pinMode(relayWhite, OUTPUT);

  digitalWrite(relayRed, HIGH);
  digitalWrite(relayGreen, HIGH);
  digitalWrite(relayBlue, HIGH);
  digitalWrite(relayWhite, HIGH);
}

void initLcd(void) {
  lcd.begin(_LCD_COLUMNS, _LCD_ROWS);
  //optionally, now set up our application-specific display settings, overriding whatever the lcd did in lcd.init()
  //lcd.commandWrite(0x0F);//cursor on, display on, blink on.  (nasty!)
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Hi");
  lcd.setCursor(0,1);
  lcd.write("APDS-9960");
  delay(5000);
  
  lcd.clear();

  lcd.setCursor(0,1);
  lcd.write("Press a key");
}

void initSerial(void) {
  Serial.begin(9600); // initialize the serial communications
  Serial.print("Init serial");
}

void serialCommunication() {
   #if 0
    // when characters arrive over the serial port...
    if (Serial.available()) {
      // wait a bit for the entire message to arrive
      delay(100);
      // read all the available characters
      while (Serial.available() > 0) {
        // display each character to the LCD
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.write(Serial.read());
      }
    }
  #endif
}

void keyPad(void) {
	adc_key_in = analogRead(0);    // it takes about 100 microseconds (10kHz) A0 = digital pin = 23; 1,2,3,4,5 are free
	key = get_key(adc_key_in); //
 
	if (key != oldkey){    //
		delay(50);		//
		adc_key_in = analogRead(0);  //
		key = get_key(adc_key_in);	// 
   
		if (key != oldkey) {			
		oldkey = key;
			if (key >=0){
        stateM(key);
			}
		}
	}
}

//
int get_key(unsigned int input){
  int k;
  for (k = 0; k < NUM_KEYS; k++){
    if (input < adc_key_val[k]){
      return k;
    }
  }
  if (k >= NUM_KEYS) {
    k = -1;
  }
  return k;
}

void stateM(const unsigned int &pressKey) {
   lcd.clear();
   lcd.setCursor(0,1);  //line=2, x=0
   lcd.write(msgs[pressKey]);

   startLed(relayWhite);
   stopLed(relayWhite);

   delay(2000);
   startLed(_BLUE_LED);
   stopLed(_BLUE_LED);
}

void startLed(unsigned int ledPin) {
  setLed(ledPin);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

void stopLed(unsigned int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(1000);
}

void setLed(const int &menuItemKey) {
    lcd.setCursor(0, 0);
    for (unsigned int i = 0; i < sizeof(msgInterface) / sizeof(msgInterface[0]); i++) {
        if (msgInterface[i].key == menuItemKey) {
            lcd.print(msgInterface[i].value);
            Serial.println(msgInterface[i].value); // Print the value to Serial
            break;
        }
    }
}


void configureNode() {
  int itemCount = sizeof(menuItems) / sizeof(menuItems[0]);
  for (int i = 0; i < itemCount; i++) {
    menuItems[i].previous = &menuItems[(i - 1 + itemCount) % itemCount];
    menuItems[i].next = &menuItems[(i + 1) % itemCount];
  }
}
