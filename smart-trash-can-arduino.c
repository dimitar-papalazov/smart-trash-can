#include <Servo.h>
#include <math.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library

#define TURN_TIME 175 // Time for servo to make 90 degrees

Servo myservo;
#define echoPinFront 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPinFront 3 //attach pin D3 Arduino to pin Trig of HC-SR04
#define echoPinInside 4 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPinInside 5 //attach pin D3 Arduino to pin Trig of HC-SR04
#define insideLength 20
#define TFT_CS    10  // define chip select pin
#define TFT_DC     7  // define data/command pin
#define TFT_RES    8  // define reset pin, or set to -1 and connect to Arduino RESET pin

long duration;
long durationInside; // variables for the duration of sound wave travel
int distance;
int distanceInside; // variables for the distance measurement
int isClosed = 1; // variable for closed or opened cover
int monitorState = 0; // variable for the state of the monitor

// Declaring the display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RES);

void setup() {
  pinMode(trigPinFront, OUTPUT); // Sets the trigPinFront as an OUTPUT
  pinMode(echoPinFront, INPUT); // Sets the echoPinFront as an INPUT
  pinMode(trigPinInside, OUTPUT); // Sets the trigPinInside as an OUTPUT
  pinMode(echoPinInside, INPUT); // Sets the echoPinInside as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  myservo.attach(6); // Servo is attached to pin 6
  // Initially the servo must be stopped
  // Servo has 3 options
  // 0 to rotate clockwise
  // 90 to stop
  // 180 to rotate counterclockwise
  myservo.write(90);
  // Initialzing the display to 240x240 resolution
  tft.init(240, 240, SPI_MODE3);
}

void loop() {
  readOutsideSensor();
  readInsideSensor();
}

// Method that rotates the servo motor
void rotateServo(int rotationSide) {
  if (rotationSide == 1) {
    if (isClosed != 0) {
      for (int i = 0; i < 15; i++) {
        myservo.write(180);
        delay(TURN_TIME);
      }
      isClosed = 0;
    }

  }
  else {
    if (isClosed != 1) {
      for (int i = 0; i < 11; i++) {
        myservo.write(0);
        delay(TURN_TIME);
      }
      isClosed = 1;
    }
  }
  myservo.write(90);
}

// Method that reads the values of the outside ultrasonic sensor
void readOutsideSensor() {
  digitalWrite(trigPinFront, LOW);
  digitalWrite(trigPinFront, HIGH);
  delay(1000);
  digitalWrite(trigPinFront, LOW);
  duration = pulseIn(echoPinFront, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance outside: ");
  Serial.print(distance);
  Serial.println(" cm");
  if (distance <= 20) {
    rotateServo(1);
    delay(5000);
  }
  else {
    rotateServo(0);
  }
}

// Method that reads the values of the inside ultrasonic sensor
void readInsideSensor() {
  digitalWrite(trigPinInside, LOW);
  digitalWrite(trigPinInside, HIGH);
  delay(1000);
  digitalWrite(trigPinInside, LOW);
  durationInside = pulseIn(echoPinInside, HIGH);
  distanceInside = durationInside * 0.034 / 2;
  Serial.print("Distance inside: ");
  Serial.print(distanceInside);
  Serial.println(" cm");
  float filledPercent = (1 - ((distanceInside * 1.0) / (insideLength * 1.0))) * 100;
  changeColorOnMonitor((int)filledPercent);
}

// Method that changes the color of the display
void changeColorOnMonitor(int percent) {
  Serial.println(percent);
  if (percent >= 0 && percent <= 33 && monitorState != 1) {
    monitorState = 1;
    tft.fillScreen(ST77XX_GREEN);
  }
  else if (percent > 33 && percent <= 67 && monitorState != 2) {
    monitorState = 2;
    tft.fillScreen(ST77XX_YELLOW);
  }
  else if ((percent < 0 || percent > 67) && monitorState != 3) {
    monitorState = 3;
    tft.fillScreen(ST77XX_RED);
  }
}