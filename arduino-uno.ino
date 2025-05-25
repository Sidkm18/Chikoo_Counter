#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

// Pin definitions
#define IR_SENSOR_PIN 2
#define BEEPER_PIN 8
#define SERVO_PIN 9
#define ESP_RX 0  // Arduino's RX pin connected to ESP32's TX
#define ESP_TX 1  // Arduino's TX pin connected to ESP32's RX

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {4, 5, 6, 7};       
byte colPins[COLS] = {10, 11, 12, 13};   
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD Display setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo setup
Servo doorServo;
bool doorIsOpen = false;

// Enum for operation modes
enum OperationMode {
  MAIN_MENU,
  COUNTING_MODE,
  TARGET_MODE
};

// Variables
OperationMode currentMode = MAIN_MENU;
int chikooCount = 0;
int targetCount = 0;
unsigned long lastChikooTime = 0;
const int debounceTime = 1000;  // 1 second debounce for IR sensor
bool newChikooDetected = false;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BEEPER_PIN, OUTPUT);
  
  // Initialize servo - attach but don't move yet
  doorServo.attach(SERVO_PIN);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Chikoo Counter");
  lcd.setCursor(0, 1);
  lcd.print("Ready to start");
  delay(2000);
  
  displayMainMenu();
}

void loop() {
  // Check for chikoo detection from ESP32
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == 'D') {  // 'D' for Detection
      newChikooDetected = true;
      chikooDetected();
    }
  }
  
  // Check IR sensor for chikoo passing
  checkIRSensor();
  
  // Check keypad input
  char key = keypad.getKey();
  if (key) {
    handleKeypadInput(key);
  }
  
  // State machine for different modes
  switch (currentMode) {
    case MAIN_MENU:
      // Main menu is handled in handleKeypadInput
      break;
      
    case COUNTING_MODE:
      // Basic counting mode operations
      break;
      
    case TARGET_MODE:
      // Target counting mode
      // Check if target has been reached
      if (chikooCount >= targetCount && targetCount > 0 && doorIsOpen) {
        beep(3);  // Triple beep to indicate target reached
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Target Reached!");
        lcd.setCursor(0, 1);
        lcd.print("Count: ");
        lcd.print(chikooCount);
        
        // Close door when target is reached
        closeDoor();
        
        delay(2000);
        updateDisplay();
      }
      break;
  }
}

void chikooDetected() {
  // ESP32 has detected a chikoo
  if (!doorIsOpen) {
    openDoor();
  }
  
  beep(1);  // Single beep for detection
}

void checkIRSensor() {
  // Check if IR sensor detects a chikoo passing
  if (digitalRead(IR_SENSOR_PIN) == LOW) {  // Assuming LOW = object detected
    unsigned long currentTime = millis();
    
    // Debounce to avoid multiple counts
    if (currentTime - lastChikooTime > debounceTime) {
      if (newChikooDetected) {
        chikooCount++;
        updateDisplay();
        newChikooDetected = false;  // Reset detection flag
        
        // Tell ESP32 the chikoo was counted
        Serial.println("C");  // 'C' for Counted
      }
      lastChikooTime = currentTime;
    }
  }
}

void handleKeypadInput(char key) {
  Serial.println(key);
  
  switch (currentMode) {
    case MAIN_MENU:
      switch (key) {
        case 'A':  // Switch to counting mode
          currentMode = COUNTING_MODE;
          chikooCount = 0;
          updateDisplay();
          openDoor();  // Open door when starting counting mode
          beep(1);
          break;
          
        case 'B':  // Switch to target mode
          currentMode = TARGET_MODE;
          chikooCount = 0;
          targetCount = 0;  // Reset target count for new input
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Target Mode");
          lcd.setCursor(0, 1);
          lcd.print("Target: _      ");
          break;
      }
      break;
      
    case COUNTING_MODE:
      switch (key) {
        case '*':  // Stop counting and close door
          closeDoor();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Counting Stopped");
          lcd.setCursor(0, 1);
          lcd.print("Count: ");
          lcd.print(chikooCount);
          delay(2000);
          updateDisplay();
          break;
          
        case '0':  // Restart counting
          chikooCount = 0;
          updateDisplay();
          if (!doorIsOpen) {
            openDoor();
          }
          break;
          
        case 'D':  // Return to main menu
          currentMode = MAIN_MENU;
          chikooCount = 0;
          closeDoor();
          displayMainMenu();
          break;
      }
      break;
      
    case TARGET_MODE:
      switch (key) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          // Handle numeric inputs for target setting
          targetCount = targetCount * 10 + (key - '0');
          lcd.setCursor(8, 1);
          lcd.print(targetCount);
          lcd.print("      ");
          break;
          
        case 'C':  // Backspace for target input
          targetCount = targetCount / 10;  // Remove last digit
          lcd.setCursor(8, 1);
          if (targetCount > 0) {
            lcd.print(targetCount);
          } else {
            lcd.print("_");
          }
          lcd.print("      ");
          break;
          
        case 'D':  // Reset target input
          targetCount = 0;
          lcd.setCursor(8, 1);
          lcd.print("_      ");
          break;
          
        case '*':  // Confirm target and start counting
          if (targetCount > 0) {
            updateDisplay();
            openDoor();  // Open door to start counting
          }
          break;
          
        case '#':  // Stop counting and close door
          closeDoor();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Counting Stopped");
          lcd.setCursor(0, 1);
          lcd.print("Count: ");
          lcd.print(chikooCount);
          lcd.print("/");
          lcd.print(targetCount);
          delay(2000);
          updateDisplay();
          break;
      }
      break;
  }
}

void displayMainMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Mode:");
  lcd.setCursor(0, 1);
  lcd.print("A:Count  B:Target");
  currentMode = MAIN_MENU;
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  switch (currentMode) {
    case COUNTING_MODE:
      lcd.print("Counting Mode");
      lcd.setCursor(0, 1);
      lcd.print("Count: ");
      lcd.print(chikooCount);
      break;
      
    case TARGET_MODE:
      lcd.print("Target Mode");
      lcd.setCursor(0, 1);
      lcd.print("Count: ");
      lcd.print(chikooCount);
      
      if (targetCount > 0) {
        lcd.print("/");
        lcd.print(targetCount);
      }
      break;
      
    case MAIN_MENU:
      // Main menu display handled in displayMainMenu()
      break;
  }
}

void openDoor() {
  // Simple open - 90 degree turn
  doorServo.write(90);
  doorIsOpen = true;
  delay(1000); // Wait for movement to complete
}

void closeDoor() {
  // Simple close - back to 0 degrees
  doorServo.write(0);
  doorIsOpen = false;
  delay(1000); // Wait for movement to complete
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BEEPER_PIN, HIGH);
    delay(100);  // 100ms beep
    digitalWrite(BEEPER_PIN, LOW);
    
    if (i < times - 1) {
      delay(100);  // 100ms pause between beeps
    }
  }
}
