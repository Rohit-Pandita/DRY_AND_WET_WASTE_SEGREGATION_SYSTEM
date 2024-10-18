# DRY_AND_WET_WASTE_SEGREGATION_SYSTEM
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD initialization with columns and rows
Servo servo1;

const int trigPin = 12;
const int echoPin = 11;
const int moistureSensorPin = A0;  // Moisture sensor pin
long duration;
int distance = 0;
int moisture = 0;
int filteredMoisture = 0;
const int maxDryValue = 20;        // Threshold for classifying wet waste
const int ultraDistance = 15;      // Distance limit for ultrasonic sensor in cm

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);  // Initialize LCD with 16 columns and 2 rows
    lcd.backlight();
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    servo1.attach(8);

    lcd.print("DRY or WET ?? ");
    delay(1000);
}

void loop() {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Dry Wet Waste");
    lcd.setCursor(3, 1);
    lcd.print("Segregator");

 
    int tempDistance = 0;
    for (int i = 0; i < 2; i++) {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH, 30000);  // Timeout set to prevent hanging
        tempDistance += duration * 0.034 / 2;
        delay(10);
    }
    distance = tempDistance / 2;

    if (distance < ultraDistance && distance > 1) {
        delay(1000);

        // Calculate moisture level
        int tempMoisture = 0;
        for (int i = 0; i < 3; i++) {
            moisture = analogRead(moistureSensorPin);
            moisture = constrain(moisture, 485, 1023);
            tempMoisture += map(moisture, 485, 1023, 100, 0);
            delay(75);
        }
        filteredMoisture = tempMoisture / 3;

        // Display and action based on moisture level
        Serial.print("Moisture: ");
        Serial.print(filteredMoisture);
        Serial.print("%    Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Garbage Detected!");
        
        if (filteredMoisture > maxDryValue) {
            lcd.setCursor(6, 1);
            lcd.print("WET");
            Serial.println("==> WET Waste");
            servo1.write(180);
        } else {
            lcd.setCursor(6, 1);
            lcd.print("DRY");
            Serial.println("==> Dry Waste");
            servo1.write(0);
        }
        delay(3000);
        servo1.write(90);  // Return the servo to the neutral position
    }

    // Reset distance and moisture values for the next loop iteration
    distance = 0;
    filteredMoisture = 0;
    delay(1000);
}
