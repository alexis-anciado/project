//UPDATES TRACKING
//V1 = APPLICATION FOR LCD AND RFID
//V2 = INCLUSION OF LOGIC FOR 4X4 KEYPAD (DID NOT WORK PROPERLY: I CRY CRY) 
//V2.2 = CHANGING VOID LOOP/ CHANGING MAIN LOGIC OF IF-ELSE STATEMENTS 
//V3 = LOGIC CONDITION: KEYPAD & RFID (AND INCLUSION OF LOCK BUTTON IN KEYPAD)
//V4 = FIXED PIN CONFIGS
//V4.2 = SIMPLIFY CODE & ADDING SOLENOID LOGIC


#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define SS_PIN 10
#define RST_PIN 9
#define RELAY_PIN 8

String correctUID = "9B 89 DD 13"; 
String correctPIN = "1234ABC0"; 

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A2, A3, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

bool rfidValidated = false;
bool pinValidated = false;

void enterPINandCard(){
  lcd.clear();
  lcd.print("Place card and");
  lcd.setCursor(0, 1);
  lcd.print("enter PIN:");
}

void setup() {
  lcd.begin(16, 2); 
  lcd.backlight();  
  enterPINandCard();
  SPI.begin(); 
  mfrc522.PCD_Init(); 
  pinMode(RELAY_PIN, OUTPUT);
}

void ACCESS_GRANTED(){
  lcd.setCursor(0, 1);
  lcd.print("Access Granted!");
  //digitalWrite() 
}

void ACCES_DENIED(){
  
}



void loop() {
  // RFID VERIFICATION
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();

    if (content.substring(1) == correctUID) {
      rfidValidated = true; 
      lcd.clear();
      lcd.print("Card accepted");
    } else {
      lcd.clear();
      lcd.print("ACCESS DENIED!:(");
      delay(2000);
      rfidValidated = false; 
    }
    delay(1000); 
    enterPINandCard();
  }

  char key = keypad.getKey();
  static String inputPIN = "";

  if (key) {
    if (key == '#') { 
      if (inputPIN == correctPIN) {
        pinValidated = true; 
      } else {
        pinValidated = false;
        lcd.clear();
        lcd.print("INCORRECT PIN!");
        delay(2000);
      }
      inputPIN = ""; 

      if (rfidValidated && pinValidated) {
        lcd.clear();
        lcd.print("ACCESS GRANTED:)");
        delay(2000); 
        digitalWrite(RELAY_PIN, HIGH);
        rfidValidated = false;
        pinValidated = false;
      } else if (!rfidValidated) {
        lcd.clear();
        lcd.print("Please swipe card.");
        delay(2000);
      }      
      enterPINandCard();
    } 
    
    else if (key == '*') { 
      inputPIN = "";
      lcd.clear();
      lcd.print("Input cleared.");
      delay(1000);
      enterPINandCard();
     } 
    
    else if (key == 'D'){  
      lcd.clear();
      lcd.print("Door is Locked");
      digitalWrite(RELAY_PIN, LOW);
      delay(2000);
     } 
    
    else {
      inputPIN += key;
      lcd.clear();
      lcd.print("PIN: ");
      lcd.print(inputPIN); 
    }
  }
}