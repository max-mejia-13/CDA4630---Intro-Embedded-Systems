#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROW_NUM = 4; // four rows
const byte COL_NUM = 4; // four columns

char keys[ROW_NUM][COL_NUM] = {
  {'A','B','C','D'},
  {'3','6','9','#'},
  {'2','5','8','0'},
  {'1','4','7','*'}
};

byte pin_rows[ROW_NUM] = {P1_0, P1_1, P1_2, P1_3}; 
byte pin_column[COL_NUM] = {P2_0, P2_1, P2_2, P2_3}; 
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COL_NUM);

LiquidCrystal lcd(P2_4, P2_5, P1_4, P1_5, P1_6, P1_7);

const String correctPassword = "1334";
String inputPassword = "";
bool systemArmed = false;

void setup() {
  lcd.begin(16, 2);
  lcd.print("System Ready");
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    lcd.setCursor(inputPassword.length(), 1);
    lcd.print('*');
    switch (key) {
      case 'A':
        if (inputPassword == correctPassword) {
          systemArmed = true;
          lcd.clear();
          lcd.print("System Armed");
        }
        else {
          lcd.clear();
          lcd.print("Wrong Code!");
        }
        inputPassword = "";
        delay(1000); // Delay to allow reading of system state
        break;
      case 'B':
        if (inputPassword == correctPassword) {
          systemArmed = false;
          lcd.clear();
          lcd.print("System Ready");
        }
        else {
          lcd.clear();
          lcd.print("Wrong Code!");
        }
        inputPassword = "";
        delay(1000); // Delay to allow reading of system state
        break;
      case '*':
        inputPassword = "";
        lcd.clear();
        lcd.print("System Ready");
        break;
      case '#':
        // Do nothing for '#' key
        break;
      default:
        inputPassword += key;
        break;
    }
  }
}
