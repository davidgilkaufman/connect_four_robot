#include <Servo.h>

// Sensor config.
const int numSensors = 7;
const int sensorPins[numSensors] = {
  0, 1, 2, 3, 4, 5, 6};

// Servo config.
const int colSelectorServoPin = 49;
const int colDegree = 6;
const int dispenserServoPin = 51;

// Detection parameters.
const int checkLoopMS = 0;
const int debounceDelayMS = 0;
const int thresholdPerSensor[numSensors] = {
  850, 975, 125, 975, 800, 800, 850};

// Ramp physics parameters.
const int rampRotateMS = 1000;
const int coinRollMS = 1000;
const int dispenseMS = 1000;

// Private state for debouncing.
int lastReading[numSensors];
int lastConfirmedState[numSensors];
long lastReadingChangeMS[numSensors];

// Private state for ramp servo.
Servo colSelectorServo;
Servo dispenserServo;

void setup() {
  // Zero debounce state.
  for (int i = 0; i < numSensors; i++) {
    lastReading[i] = 0;
    lastConfirmedState[i] = 0;
    lastReadingChangeMS[i] = 0L;
  }

  colSelectorServo.attach(colSelectorServoPin);
  colSelectorServo.write(0);
  dispenserServo.attach(dispenserServoPin);
  dispenserServo.write(0);

  Serial.begin(9600);
}

void loop() {
  commandRead();
  boardRead();
  delay(checkLoopMS);
}

void commandRead() {
  if (Serial.available()) {
    int inCol = Serial.read() - 48;
    if (inCol >= 0 && inCol <= 6) {
      dispenseInCol(inCol);
    }
  }
}

void dispenseInCol(int col) {
  colSelectorServo.write((col + 1) * colDegree);
  delay(rampRotateMS);
  dispenserServo.write(90);
  delay(dispenseMS);
  delay(coinRollMS);
  dispenserServo.write(0);
  colSelectorServo.write(0);
  delay(rampRotateMS);
}

void boardRead() {
  for (int i = 0; i < numSensors; i++) {
    int reading = getReading(i);
    int newState = confirmState(i, reading, lastConfirmedState[i]);
    if(newState != lastConfirmedState[i]) {
      lastConfirmedState[i] = newState;
      if (newState == 1) {
        Serial.println(i);
      }
    }
  }
//  Serial.println("");
}

int confirmState(int i, int reading, int lastConfirmedState) {
  if (reading != lastReading[i]) {
    lastReadingChangeMS[i] = millis();
  }
  int confirmedState = lastConfirmedState;
  // A reading is only confirmed when it's been in that state for more than debounceDelayMS.
  if ((millis() - lastReadingChangeMS[i]) > debounceDelayMS) {
    confirmedState = reading;
  }
  lastReading[i] = reading;
  return confirmedState;
}

int getReading(int i) {
  int sensorPin = sensorPins[i];
  int val = analogRead(sensorPin);
//  Serial.print(i);
//  Serial.print("=");
//  Serial.print(val);
//  Serial.print(" ");
  int reading = (int)(val > thresholdPerSensor[i]);
  return reading;
}

