//UPDATES TRACKING
//V1 = APPLICATION FOR LCD AND RFID
//V2 = INCLUSION OF LOGIC FOR 4X4 KEYPAD (DID NOT WORK PROPERLY: I CRY CRY) 
//V2.2 = CHANGING VOID LOOP/ CHANGING MAIN LOGIC OF IF-ELSE STATEMENTS 
//V3 = LOGIC CONDITION: KEYPAD & RFID (AND INCLUSION OF LOCK BUTTON IN KEYPAD)


#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the pins for the RFID module
#define SS_PIN 9
#define RST_PIN 10

// Create an instance of the MFRC522 class
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Keypad setup
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};


//CHANGE PIN CONFIGS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
byte rowPins[ROWS] = {2, 3, 4, 5}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; // Connect to the column pinouts of the keypad

// Create an instance of the Keypad class
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the I2C address to match your LCD

// Predefined correct RFID UID and PIN
String correctUID = "XX XX XX XX"; // UID
String correctPIN = "1234"; // PIN

// Flags to track successful RFID and PIN entries
bool rfidValidated = false;
bool pinValidated = false;

void setup() {
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();  // Turn on the backlight
  lcd.print("Place card and");  // Display initial message
  lcd.setCursor(0, 1);
  lcd.print("enter PIN:");

  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
}

void ACCESS_GRANTED(){
  lcd.setCursor(0, 1);
  lcd.print("Access Granted!");
  //digitalWrite() //solenoid lock = unlocks
}

void ACCES_DENIED(){
  
}



void loop() {
  // Check for an RFID card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();

    // Check if the scanned UID matches the predefined correct UID
    if (content.substring(1) == correctUID) {
      rfidValidated = true; // Mark RFID as validated
      lcd.clear();
      lcd.print("Card accepted");
    } else {
      lcd.clear();
      lcd.print("Access denied!");
      delay(2000);
      rfidValidated = false; // Reset validation
    }
    delay(1000); // Pause to read RFID
    lcd.clear();
    lcd.print("Place card and");
    lcd.setCursor(0, 1);
    lcd.print("enter PIN:");
  }

  // Check for keypad input
  char key = keypad.getKey();
  static String inputPIN = "";

   

  if (key) {
    if (key == '#') { // '#' is used to enter the PIN
      if (inputPIN == correctPIN) {
        pinValidated = true; // Mark PIN as validated
      } else {
        pinValidated = false;
        lcd.clear();
        lcd.print("Incorrect PIN!");
        delay(2000);
      }
      inputPIN = ""; // Clear input after checking

      // Check both validations
      if (rfidValidated && pinValidated) {
        lcd.clear();
        lcd.print("Access granted!");
        delay(2000); // Placeholder for action
        // Reset validations after access granted
        rfidValidated = false;
        pinValidated = false;
      } else if (!rfidValidated) {
        lcd.clear();
        lcd.print("Swipe card!");
        delay(2000);
      }

      lcd.clear();
      lcd.print("Place card and");
      lcd.setCursor(0, 1);
      lcd.print("enter PIN:");
    } 
    
    else if (key == '*') { // '*' is used to clear the input
      inputPIN = "";
      lcd.clear();
      lcd.print("Input cleared.");
      delay(1000);
      lcd.clear();
      lcd.print("Place card and");
      lcd.setCursor(0, 1);
      lcd.print("enter PIN:");
     } 
    
    else if (key == 'D'){ // 'D' is used to lock the door when leaving
      //digitalWrite(, ) 
      lcd.clear();
      lcd.print("Door is Locked");
      delay(2000);
     } 
    
    else {
      inputPIN += key;
      lcd.clear();
      lcd.print("PIN: ");
      lcd.print(inputPIN); // Display entered PIN
    }
  }
}