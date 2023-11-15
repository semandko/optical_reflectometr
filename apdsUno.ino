
#include <Arduino.h>
#include <LiquidCrystal.h>

#define _LCD_COLUMNS 16
#define _LCD_ROWS 2

enum E_BoardKey {
  E_RIGHT_KEY   = 0,
  E_UP_KEY      = 1,
  E_DOWN_KEY    = 2,
  E_LEFT_KEY    = 3,
  E_SELECT_KEY  = 4
};

enum E_BtnKey {
  E_BtnKey_DEFAULT =  0,
  E_BtnKey_MAIN =     1,
  E_BtnKey_LEDS =     2,
  E_BtnKeyEND
};

struct KeyValue {
    int key;
    char value[15];
    E_BtnKey btnKey;
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
 
//Key message = E_BoardKey
char msgs[5][15] = {
                    "Right Key OK  ", 
                    "Up Key OK     ", 
                    "Down Key OK   ", 
                    "Left Key OK   ", 
                    "Select Key OK " };

KeyValue msgInterface[] = {
    {0,           "Source set    ", E_BtnKey_MAIN},
    {relayRed,    "Red light     ", E_BtnKey_LEDS},
    {relayGreen,  "Green light   ", E_BtnKey_LEDS},
    {relayBlue,   "Blue light    ", E_BtnKey_LEDS},
    {relayWhite,  "White light   ", E_BtnKey_LEDS},
    {5,           "Start         ", E_BtnKey_MAIN},
    {6,           "Stop          ", E_BtnKey_MAIN},
    {7,           "Send once     ", E_BtnKey_MAIN},
    {8,           "Send continue ", E_BtnKey_MAIN},
    {9,           "Calibration   ", E_BtnKey_MAIN}
};

MenuItem menuItems[] = {
    {msgInterface[0], {0}, nullptr, nullptr}, // "Source set    "
    {msgInterface[1], {0}, nullptr, nullptr}, // "Red light     "
    {msgInterface[2], {0}, nullptr, nullptr}, // "Green light   "
    {msgInterface[3], {0}, nullptr, nullptr}, // "Blue light    "
    {msgInterface[4], {0}, nullptr, nullptr}, // "White light   "
    {msgInterface[5], {0}, nullptr, nullptr}, // "Start         "
    {msgInterface[6], {0}, nullptr, nullptr}, // "Stop          "
    {msgInterface[7], {0}, nullptr, nullptr}, // "Send once     "
    {msgInterface[8], {0}, nullptr, nullptr}, // "Send continue "
    {msgInterface[9], {0}, nullptr, nullptr}  // "Calibration "
};

MenuItem* currentMenuItem = &menuItems[0]; // Start from the first item

// Create an array to store links to menu items
MenuItem* menuHistory[2] = {currentMenuItem, nullptr}; // Adjust the size as needed

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

  configureNode();
  
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
   lcd.write(msgs[pressKey]); // E_BoardKey name

  const E_BoardKey boardKey = (const E_BoardKey)pressKey;

  switch (boardKey) {
    
    case (E_RIGHT_KEY):
      
    break;
    
    case(E_UP_KEY):
      updateCurrentMenuItem(currentMenuItem->previous);
    break;
    
    case (E_DOWN_KEY):
      updateCurrentMenuItem(currentMenuItem->next);
    
    break;
    
    case (E_LEFT_KEY):
    
    break;
    
    case (E_SELECT_KEY):
      if (currentMenuItem->interfaceItem.btnKey == E_BtnKey_LEDS) {
        setLed(currentMenuItem->interfaceItem.key);
      }
    break;
    
    default:
    break;
  }

  lcd.setCursor(0, 0);
  lcd.print(currentMenuItem->interfaceItem.value);
  Serial.println(currentMenuItem->interfaceItem.btnKey);
   
}

void startLed(unsigned int ledPin) {
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

// When you want to update the current menu item and store the history:
void updateCurrentMenuItem(MenuItem* newMenuItem) {
    if (menuHistory[0] != nullptr) {
        // Move the current menu item to the history
        menuHistory[1] = menuHistory[0];
    }
  
    // Set the new current menu item
    menuHistory[0] = newMenuItem;
    
    // Update the currentMenuItem pointer
    currentMenuItem = newMenuItem;
}
