#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 

#define SDA_PIN 5 
#define SCL_PIN 4

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET -1 
#define SCREEN_ADDRESS 0x3C 

#define in1 16 // Define the pins where the magnets are located
#define in2 17 
#define in3 18 
#define in4 19 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

void setup() {
  Serial.begin(9600); 
  Wire.begin(SDA_PIN, SCL_PIN); 

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed")); 
    for (;;); 
  }

  display.clearDisplay(); 
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 10);
  display.display(); 
  display.clearDisplay(); 

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW); 
  digitalWrite(in3, LOW); 
  digitalWrite(in4, LOW);
}

int charCount = 0; // Variable to keep track of character count
int pos = 0; // Variable to store the current position of the motor
int speed = 0; // Variable to store the speed of the motor

void loop() {
  if (Serial.available()) { 
    String command = Serial.readStringUntil('\n'); // Reads the incoming command until a newline character

    if (command.startsWith("P:")) { // Checks if the command starts with "P:"
      int commaIndex = command.indexOf(','); // Finds the index of the comma in the command
      if (commaIndex > 0) { // Ensures the comma exists
        String positionStr = command.substring(2, commaIndex); // Extracts the position value from the command
        String speedStr = command.substring(command.indexOf("S:") + 2); // Extracts the speed value from the command

        int targetPosition = positionStr.toInt(); // Converts the position value to an integer
        int motorSpeed = speedStr.toInt(); 

        RotateMotor(motorSpeed, targetPosition);
      }
    } else {
      Serial.println("Invalid command format. Use P:<position>,S:<speed>"); // Prints an error message for invalid commands
    }
  }

  if (pos % 100 == 0) { // Only update display every 100 pos
    DisplayData(); 
  }
}

void RotateMotor(int stepDelay, int toPos) {
  int orgPos = pos; // Stores the original position

  if ((orgPos - toPos) > 0) { // Checks if the motor needs to move backward
    RampPos(stepDelay); // Gradually increases speed in the positive direction
  } else {
    RampNeg(stepDelay); 
  }

  while (pos != toPos) { // Loops until the motor reaches the target position
    for (int i = 0; i < abs(pos - orgPos); i++) { 
      if ((orgPos - toPos) > 0) {
        MovePos(); 
      } else {
        MoveNeg(); 
      }
      delay(stepDelay); 
    }
  }
}

void MovePos() { // Move motor
  int val = pos % 8; // Calculates the step phase based on the position
  digitalWrite(in1, val == 0 || val == 1 || val == 7 ? HIGH : LOW); // Sets in1 high for certain steps
  digitalWrite(in2, val == 1 || val == 2 || val == 3 ? HIGH : LOW); 
  digitalWrite(in3, val == 3 || val == 4 || val == 5 ? HIGH : LOW); 
  digitalWrite(in4, val == 5 || val == 6 || val == 7 ? HIGH : LOW); 
  pos++; 
}

void RampPos(int toSpeed) { // Ramp motor speed
  int val = pos % 8; 

  for (int i = 0; i < toSpeed; i++) { // Gradually increases the speed
    digitalWrite(in1, val == 0 || val == 1 || val == 7 ? HIGH : LOW); 
    digitalWrite(in2, val == 1 || val == 2 || val == 3 ? HIGH : LOW); 
    digitalWrite(in3, val == 3 || val == 4 || val == 5 ? HIGH : LOW);
    digitalWrite(in4, val == 5 || val == 6 || val == 7 ? HIGH : LOW);
    pos++;
    delay(i); // Delays based on the ramp speed
  }
}

void MoveNeg() {
  int val = pos % 8;
  digitalWrite(in4, val == 0 || val == 1 || val == 7 ? HIGH : LOW); 
  digitalWrite(in3, val == 1 || val == 2 || val == 3 ? HIGH : LOW); 
  digitalWrite(in2, val == 3 || val == 4 || val == 5 ? HIGH : LOW);
  digitalWrite(in1, val == 5 || val == 6 || val == 7 ? HIGH : LOW);
  pos--; // Decrements the position
}

void RampNeg(int toSpeed) {
  int val = pos % 8; 

  for (int i = 0; i < toSpeed; i++) {
    digitalWrite(in4, val == 0 || val == 1 || val == 7 ? HIGH : LOW);
    digitalWrite(in3, val == 1 || val == 2 || val == 3 ? HIGH : LOW);
    digitalWrite(in2, val == 3 || val == 4 || val == 5 ? HIGH : LOW);
    digitalWrite(in1, val == 5 || val == 6 || val == 7 ? HIGH : LOW);
    pos--; 
    delay(i); 
  }
}

void DisplayData() { // Display current pos & speed
  display.clearDisplay(); 
  display.setCursor(0, 0); 
  display.print("Current Pos: "); 
  display.println(pos); 
  display.print("Current Speed: ");
  display.println(speed);
  display.display();
}