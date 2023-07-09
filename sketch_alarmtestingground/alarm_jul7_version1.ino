#include <Servo.h>

#include <LiquidCrystal.h>
#include <Keypad.h>

// LCD pin mapping
const int rsPin = 6;
const int enablePin = 7;
const int d4Pin = 2;
const int d5Pin = 3;
const int d6Pin = 4;
const int d7Pin = 5;
//servo map and creation
const int servoPin = 8; // servo pin
Servo myServo;
// Keypad pin mapping
const byte ROWS = 4; // Number of rows on the keypad
const byte COLS = 4; // Number of columns on the keypad

// Buzzer pin
const int buzzerPin = 9;
const int toneFrequency1 = 1000; // frequncies
const int toneFrequency2 = 2000;
const int toneFrequency3 = 2500;
// motion sensor setup
const int MotionsensorPin = 11;
int MotionState = LOW;
int val=0;
//const unsigned long motionDeadZone = 450;
//unsigned long lastMotionTime;
//strike
// LED pin
const int ledPin = 10;
//keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {22, 23, 24, 25};    // Row pins of the keypad
byte colPins[COLS] = {26, 27, 28, 29};    // Column pins of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd(rsPin, enablePin, d4Pin, d5Pin, d6Pin, d7Pin);

const String correctPassword = "1334";
String inputPassword = "";
String userPassword = "1";
bool systemArmed = false;
bool alreadyDisplayedArmed = false;
bool systemTriggered=false;

enum KeypadState {
  STANDARD,
  ADDITIONAL,
  WRONG_CODE,
};

KeypadState keypadState = STANDARD;
unsigned long disarmTimeout = 0;
const unsigned long disarmTimeoutDuration = 5000;
unsigned long wrongCodeTimeout = 0;
const unsigned long wrongCodeTimeoutDuration = 2000;

const int maxWrongAttempts = 3;  // Maximum number of wrong attempts
int wrongAttempts = 0;  // Counter for wrong attempts

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("System Ready:");
  lcd.setCursor(0, 1);
  myServo.attach(servoPin);
  pinMode(buzzerPin, OUTPUT);
  pinMode(MotionsensorPin,INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
 int val = digitalRead(MotionsensorPin);
  char key = keypad.getKey();
  if (key) {
    chime();
    switch (key) {
      case 'A':
        if (inputPassword == correctPassword || inputPassword == userPassword)
         {
          resetWrongAttempts();
          systemArmed = true;
//          lastMotionTime = millis();
          lcd.clear();
          lcd.print("System Armed");
          digitalWrite(ledPin, HIGH);
          noTone(buzzerPin);
          armSystem(toneFrequency1);
          delay(1000);
          for (int i = 5; i > 0; i--)
           {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Leave in: ");
            lcd.print(i);
            delay(1000);
          }
          myServo.write(180);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("System Armed");
          inputPassword = "";
          alreadyDisplayedArmed = true;
        } else {
          handleWrongPassword();
        }
        break;
      case 'B':
        if (inputPassword == correctPassword || inputPassword == userPassword) {
          resetWrongAttempts();
          systemArmed = false;
          systemTriggered= false;
          
          lcd.clear();
          lcd.print("System Disarmed");
          digitalWrite(ledPin, LOW);
          disarmSystem(toneFrequency2);
          myServo.write(-180);
          delay(2000);
          lcd.clear();
          lcd.print("System Ready");
          lcd.setCursor(0, 1);
          alreadyDisplayedArmed = false;
          inputPassword = "";
        } else {
          handleWrongPassword();
        }
        break;
      case '*':
        if (!systemArmed && keypadState != ADDITIONAL) {
          inputPassword = "";
          lcd.clear();
          lcd.print("System Ready");
          lcd.setCursor(0, 1);
        }
        break;
      case '#':
        if (keypadState == ADDITIONAL) {
          saveUserPassword();
          lcd.clear();
          lcd.print("Password Saved");
          delay(5000);
          lcd.clear();
          lcd.print("System Ready");
          delay(2000);
          lcd.setCursor(0, 1);
          keypadState = STANDARD;
        }
        break;
      case 'C':
        if (inputPassword == correctPassword || inputPassword == userPassword) {
          keypadState = ADDITIONAL;
          inputPassword = "";
          lcd.clear();
          lcd.print("Enter New User Password");
          lcd.setCursor(0, 1);
        }
        else {
          handleWrongPassword();
        }
        break;
      default:
        handleInputPassword(key);
        break;
    }
  }
if (val == HIGH && systemArmed && !systemTriggered)	// check if the input is HIGH
  {            
     
	
      lcd.clear();
      lcd.print("System Motion");
      systemTriggered = true; 
      alarmSystem(toneFrequency3);
        // Update the last motion detection time
      
}





  if (systemArmed && !alreadyDisplayedArmed) {
    lcd.clear();
    lcd.print("System Armed");
    alreadyDisplayedArmed = true;
  }

  if (systemArmed && alreadyDisplayedArmed && (millis() - disarmTimeout <= disarmTimeoutDuration)) {
    clearCountdown(); // Clear previous time messages
    lcd.setCursor(0, 1);
    displayCountdown(disarmTimeout - millis());
  }


}
void handleInputPassword(char key) {
  if (keypadState == ADDITIONAL) {
    inputPassword += key;
    lcd.setCursor(inputPassword.length() - 1, 1);
    lcd.print('*');
  } else {
    inputPassword += key;
    lcd.setCursor(inputPassword.length() - 1, 1);
    lcd.print(key);
  }
}

void saveUserPassword() {
  userPassword = inputPassword;
  inputPassword = "";
}

void clearCountdown() {
  lcd.setCursor(0, 1);
  lcd.print("                "); // Clear the line
}

void displayCountdown(unsigned long remainingTime) {
  const int countdownLines = 4;
  String countdown[countdownLines] = {
    "Leave in:       ",
    "3 seconds",
    "2 seconds",
    "1 second "
  };

  if (remainingTime >= 4000) {
    lcd.print(countdown[0]);
  } else if (remainingTime >= 3000) {
    lcd.print(countdown[1]);
  } else if (remainingTime >= 2000) {
    lcd.print(countdown[2]);
  } else if (remainingTime >= 1000) {
    lcd.print(countdown[3]);
  }
}

void handleWrongPassword() {
  lcd.clear();
  lcd.print("Wrong Code!");
  inputPassword = "";
  delay(2000);
  lcd.clear();
  if (systemArmed) {
    lcd.print("System Armed");
  } else {
    lcd.print("System Ready");
  }
  lcd.setCursor(0, 1);

  wrongAttempts++;  // Increment the wrong attempts counter

 if (wrongAttempts >= 3) {
  lcd.clear();
  lcd.print("System Locked");
  delay(30000);  // Delay for 30 seconds
  resetWrongAttempts();  // Reset the wrong password counter
  lcd.clear();
  lcd.print("System Ready");
  lcd.setCursor(0, 1);
}
}

void resetWrongAttempts() {
  wrongAttempts = 0;  // Reset the wrong attempts counter
}
void armSystem(int frequency) {
  // Generate tone on the buzzer pin at the specified frequency
  tone(buzzerPin, frequency);
  delay(200);  // Adjust the duration of the tone as needed
  noTone(buzzerPin);  // Stop the tone
}
void alarmSystem(int frequency) {
  // Generate tone on the buzzer pin at the specified frequency
  
  for( int i= 0; i<20; i++)
  {
  tone(buzzerPin, frequency);
  delay(200);  // Adjust the duration of the tone as needed
  noTone(buzzerPin);
  
  }
  systemTriggered = false;
  
}
void disarmSystem(int frequency) {
  // Generate tone on the buzzer pin at the specified frequency
  tone(buzzerPin, frequency);
  delay(200);  // Adjust the duration of the tone as needed
  noTone(buzzerPin);  // Stop the tone
}
void chime() {
  // Activate the buzzer
  digitalWrite(buzzerPin, HIGH);
  delay(50);  // Adjust the duration of the beep as needed
  digitalWrite(buzzerPin, LOW);
}
