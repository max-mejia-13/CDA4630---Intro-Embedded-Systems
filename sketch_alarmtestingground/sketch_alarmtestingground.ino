#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROW_NUM = 4;
const byte COL_NUM = 4;

char keys[ROW_NUM][COL_NUM] = {
  {'A','3','2','1'},
  {'B','6','5','4'},
  {'C','9','8','7'},
  {'D','#','0','*'}
};

byte pin_rows[ROW_NUM] = {P2_0, P2_1, P2_2, P2_3}; 
byte pin_column[COL_NUM] = {P1_0, P1_1, P1_2, P1_3};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COL_NUM);

LiquidCrystal lcd(P2_4, P2_5, P1_4, P1_5, P1_6, P1_7);

const String correctPassword = "1334";
String inputPassword = "";
String userPassword = "1";
bool systemArmed = false;
bool alreadyDisplayedArmed = false;

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

void setup() {
  lcd.begin(16, 2);
  lcd.print("System Ready");
  lcd.setCursor(0, 1);
  pinMode(P1_0, INPUT_PULLUP);
  pinMode(P1_1, INPUT_PULLUP);
  pinMode(P1_2, INPUT_PULLUP);
  pinMode(P1_3, INPUT_PULLUP);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case 'A':
        if (inputPassword == correctPassword || inputPassword == userPassword) {
          systemArmed = true;
          lcd.clear();
          lcd.print("System Armed");
          delay(1000);
          for (int i = 5; i > 0; i--) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Leave in: ");
            lcd.print(i);
            delay(1000);
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("System Armed");
          inputPassword = "";
          alreadyDisplayedArmed = true;
        }
        //... rest of your code

        else {
          lcd.clear();
          lcd.print("Wrong Code!");
          inputPassword = "";
          delay(2000);
          lcd.clear();
          if(systemArmed){
            lcd.print("System Armed");
          } else {
            lcd.print("System Ready");
          }
          lcd.setCursor(0, 1);
        }
        break;
      case 'B':
        if (inputPassword == correctPassword || inputPassword == userPassword) {
          systemArmed = false;
          lcd.clear();
          lcd.print("System Disarmed");
          delay(2000);
          lcd.clear();
          lcd.print("System Ready");
          lcd.setCursor(0, 1);
          alreadyDisplayedArmed = false;
          inputPassword = "";
        }
        else {
          lcd.clear();
          lcd.print("Wrong Code!");
          inputPassword = "";
          delay(2000);
          lcd.clear();
          if(systemArmed){
            lcd.print("System Armed");
          } else {
            lcd.print("System Ready");
          }
          lcd.setCursor(0, 1);
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
          lcd.clear();
          lcd.print("Enter Correct Password First");
          delay(2000);
          inputPassword = "";
          lcd.setCursor(0, 1);
        }
        break;
      default:
        handleInputPassword(key);
        break;
    }
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
