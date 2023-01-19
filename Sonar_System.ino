#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

// Screen Variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
int width = SCREEN_WIDTH;
int height = SCREEN_HEIGHT;

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Color Variables
char c  = SSD1306_WHITE;
char c0 = SSD1306_BLACK;

// Servo pin 
int servoPin = 7; 
// Create a servo object 
Servo Servo1; 
int angle = 0;

// Distance Metric
int d1 = 20; // Min Distance
int d2 = 34;
int d3 = 46;
int d4 = 64; // Max Distance
// Circle Center
int xc = (width/2)-1;
int yc = height-1;
// Radius Metric
int r1 = yc;
int r2 = (width-width*0.27)/2;
int r3 = (width-width*0.479)/2;
int r4 = (width-width*0.687)/2;

// Ultrasonic pins
const int trigPin = 22;
const int echoPin = 24;
// Ultrasonic variables
long duration;
int distance;

// Data points Array
int dataPoint [182][2] = {0};

// Initilize variables
unsigned long timeold;
int distance0;
int theta0;

// Setup
void setup() {
  Serial.begin(9600);       // Begin serial monitor at 9600 baud
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  Servo1.attach(servoPin);  // Attach servo to servo pin
  // Check Availability of SSD1306 Display
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);} // Don't proceed, loop forever
  display.clearDisplay();   // Clear buffer
  introScreen();            // Draw Intro Screen
  display.display();        // Initilize display draw 
  delay(500);               // Wait for initilization
}


// Main Loop
void loop() {
  drawSonar();                    // Draw Sonar Backdrop
  if (millis() - timeold >= 10) { // Set timer to 50ms
    distance0 = ultrasonic();     // Get Distance from ultrasonic
    theta0 =  moveServo();        // Move servo and get current angle
    timeold = millis();           // Reset Timer
  }
  drawObject(distance0, theta0);  // Draw detected object on screen
  drawInfo(distance0, theta0);    // Draw Information on screen
  display.display();              // Draw Screen
  display.clearDisplay();         // Clear the buffer
}

// Sonar Backdrop Function
void drawSonar(void) {
  // draws the arc lines
  display.drawCircle(xc,yc,r1,c);   // Outermost Circle
  display.drawCircle(xc,yc,r2,c);
  display.drawCircle(xc,yc,r3,c);
  display.drawCircle(xc,yc,r4,c);   // Innermost Circle
  display.fillCircle(xc,yc,r4/4,c); // Detector Circle

  // draws the angle lines
  display.drawLine(xc , yc , xc + r1*cos(radians(30)) ,yc - r1*sin(radians(30)), c);
  display.drawLine(xc , yc , xc + r1*cos(radians(60)) ,yc - r1*sin(radians(60)), c);
  display.drawLine(xc , yc , xc + r1*cos(radians(90)) ,yc - r1*sin(radians(90)), c);
  display.drawLine(xc , yc , xc + r1*cos(radians(30)) ,yc - r1*sin(radians(30)), c);
  display.drawLine(xc , yc , xc + r1*cos(radians(120)) ,yc - r1*sin(radians(120)), c);
  display.drawLine(xc , yc , xc + r1*cos(radians(150)) ,yc - r1*sin(radians(150)), c);
  display.drawLine(0, yc, width, yc, c);
}


// Function to draw detected obstacles
void drawObject(int d, int theta){
  // Calculate x and y
  int x = d*cos(radians(theta));
  int y = d*sin(radians(theta));
  
  // Assign values to array
  int i = theta;
  dataPoint[i][1] = x;
  dataPoint[i][2] = y;

  // Draw data points
  for(int j=0; j<= 180; ++j){
    display.fillCircle(xc + dataPoint[j][1], yc - dataPoint[j][2], 1, c);
  }
  // Draw Scan Line
  display.drawLine(xc , yc  , xc + r1*cos(radians(theta+1)) ,yc - r1*sin(radians(theta+1)), c );
}


// Function to move servo
int moveServo(){
  if (angle == 180){      //Reset Servo
    angle = 0;
    Servo1.write(angle);
    delay(1000);
  }
  else{                  // Increment Servo
    angle+=1;
    Servo1.write(angle);
  }

  return angle;
}


// Function to collect data from ultrasonic sensor
int ultrasonic(){
 // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  return distance;
}


// Function to draw info on screen
void drawInfo(int distance, int theta){
  display.setTextColor(c); display.setTextSize(1);       // Text Settings
  display.setCursor(0, 0); display.println(distance);    // Detected Distance
  display.setCursor(110, 0); display.println(theta);     // Current Angle
  //Serial Print Info
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.print(" cm | Theta = ");
  Serial.print(theta);
  Serial.println(" degree");
}


//Function to print Intro Screen
void introScreen(){
  // Embedded Systems Project MCTE 4342
  display.clearDisplay();
  display.setTextColor(c); display.setTextSize(2);  
  display.setCursor(0, 0);
  display.println("Embedded  Systems   Project   MCTE 4342");
  display.display();
  delay(5000);
  // Yaman Aburaneh 1720907
  display.clearDisplay();
  display.setTextColor(c); display.setTextSize(1);  
  display.setCursor(0, 0);
  display.println("Yaman Aburaneh");
  display.setCursor(0, height/4);
  display.println("1720907");
  // Muhammad Syahir
  display.setTextColor(c); display.setTextSize(1);  
  display.setCursor(0, height/2);
  display.println("Muhammad Syahir");
  display.setCursor(0, height-15);
  display.println("1817425");
  display.display();
  delay(5000);
  display.clearDisplay();
  // Sonar System
  display.clearDisplay();
  display.setTextColor(c); display.setTextSize(2);  
  display.setCursor(width/4, 15);
  display.println("Sonar");
  display.setCursor(width/4-5, 35);
  display.println("System");
  display.display();
  delay(5000);
}