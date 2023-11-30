#include <AccelStepper.h>
#include <RTClib.h>
#include <TimeLib.h>

RTC_DS1307 rtc;
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1
int feed1hour = 16;
int feed1min = 33;
int feed2hour = 17;
int feed2min = 00;
AccelStepper stepper(motorInterfaceType, stepPin, dirPin);

bool feeding = false;
unsigned long feedStartTime = 0;
const unsigned long feedDuration = 5000; // 5 seconds in milliseconds
const unsigned long feedCooldown = 60000; // 60 seconds in milliseconds

void setup() {
  Serial.begin(57600);  // Initialize Serial communication for debugging
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  stepper.setMaxSpeed(2000);  // Increase the maximum speed

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  //stepper.setSpeed(300);  // Set the speed
  //stepper.runSpeed();  // Run the stepper continuously

  DateTime now = rtc.now();
  Serial.print("Current Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // Check if it's time to start feeding for the first time slot
  if (now.hour() == feed1hour && now.minute() == feed1min && now.second() == 0 && !feeding) {
    startFeeding();
  }

  // Check if it's time to start feeding for the second time slot
  if (now.hour() == feed2hour && now.minute() == feed2min && now.second() == 0 && !feeding) {
    startFeeding();
  }

  // Check if feeding is in progress
  if (feeding) {
    unsigned long elapsedTime = millis() - feedStartTime;

    if (elapsedTime < feedDuration) {
      stepper.runSpeed();  // Run the stepper continuously
    } else {
      stopFeeding();
    }
  }
}

void startFeeding() {
  feeding = true;
  stepper.enableOutputs();  // Enable stepper motor outputs
  feedStartTime = millis();
  stepper.setSpeed(300);  // Set the speed
}

void stopFeeding() {
  feeding = false;
  stepper.stop();  // Stop the motor
  stepper.disableOutputs();  // Disable stepper motor outputs
  delay(feedCooldown);  // Wait for the cooldown period
}
