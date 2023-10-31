#include <LiquidCrystal.h>

#define _LCD_COLUMNS 16
#define _LCD_ROWS 2

// Relle connection
#define _RED_LED    3 // PD3 // 3
#define _GREEN_LED  2 // PD2 // 2
#define _BLUE_LED   10 // PB2 // 10
#define _WHITE_LED  11 // PB3 // 11
// initialize the Relay
// with the arduino pin number it is connected to UNO
const unsigned int relayRed = _RED_LED, relayGreen = _GREEN_LED, relayBlue = _BLUE_LED, relayWhite = _WHITE_LED;

// initialize the library by associating any needed LCD interface pin
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
char msgInterface[9][15] = {
                    "Source set    ", 
                    "Red light     ", 
                    "Green light   ", 
                    "Blue light    ", 
                    "White light   ",
                    "Start         ",
                    "Stop          ",
                    "Send once     ",
                    "Send continue "};

// variables
const unsigned int  led_key_val[4] ={1, 2, 3, 4};
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
void setLed(const unsigned int &ledKey);
void runLed(unsigned int ledPin);


//************************************************************
//************************************************************
void setup() {

  initRelayLed();
  initLcd();
  initSerial();
  
}

void loop() {
	
  keyPad();
  serialCommunication();

  //delay(1000);
}
//************************************************************
//************************************************************
void initRelayLed(void) {
  pinMode(relayRed, OUTPUT);
  pinMode(relayGreen, OUTPUT);
  pinMode(relayBlue, OUTPUT);
  pinMode(relayWhite, OUTPUT);
}

void initLcd(void) {
  
  lcd.begin(_LCD_COLUMNS, _LCD_ROWS);
  //optionally, now set up our application-specific display settings, overriding whatever the lcd did in lcd.init()
  //lcd.commandWrite(0x0F);//cursor on, display on, blink on.  (nasty!)
  
  lcd.clear();
  lcd.setCursor(0,2);
  lcd.write("press a key");
  delay(1000);
  lcd.setCursor(0,2);
}

void initSerial(void) {
  Serial.begin(9600); // initialize the serial communications
}

void serialCommunication() {
   #if 0
    // when characters arrive over the serial port...
    if (Serial.available()) {
      // wait a bit for the entire message to arrive
      delay(100);
      // clear the screen
      lcd.clear();
      // read all the available characters
      while (Serial.available() > 0) {
        // display each character to the LCD
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
				lcd.setCursor(0,2);  //line=2, x=0
				lcd.write(msgs[key]);

        runLed(relayWhite);
        
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

void runLed(unsigned int ledPin) {
   digitalWrite(ledPin, LOW);
   delay(1000);
   digitalWrite(ledPin, HIGH);
   delay(1000);
}

void setLed(const unsigned int &ledKey) {
  
  if ( ledKey < sizeof(msgInterface)){
    lcd.setCursor(0,1);  //line=1, x=0
    lcd.write(msgInterface[ledKey]);
  }       
}
