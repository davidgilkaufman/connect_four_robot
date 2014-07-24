// Sensor config.
const int numSensors = 1;
int sensorPins[numSensors] = {0};

// Detection parameters.
int checkLoopMS = 1;
int debounceDelayMS = 50;
int detectionThreshold = 865;

// Private state for debouncing.
int lastReading[numSensors];
int lastConfirmedState[numSensors];
long lastReadingChangeMS[numSensors];

void setup() {
  // Zero debounce state.
  for (int i = 0; i < numSensors; i++) {
    lastReading[i] = 0;
    lastConfirmedState[i] = 0;
    lastReadingChangeMS[i] = 0L;
  }
  
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    int sensorPin = sensorPins[i];
    int reading = getReading(sensorPin);
    int newState = confirmState(i, reading, lastConfirmedState[i]);
    if(newState != lastConfirmedState[i]) {
      lastConfirmedState[i] = newState;
      
      if (newState == 0) {
        Serial.println(i);
      }
    }
  }
  delay(checkLoopMS);
}

int confirmState(int i, int reading, int lastConfirmedState) {
  if (reading != lastReading[i]) {
    lastReadingChangeMS[i] = millis();
  }
  int confirmedState = lastConfirmedState;
  if ((millis() - lastReadingChangeMS[i]) > debounceDelayMS) {
    confirmedState = reading;
  }
  lastReading[i] = reading;
  return confirmedState;
}

int getReading(int sensorPin) {
  int val = analogRead(sensorPin);
  int reading = (int)(val > detectionThreshold);
  return reading;
}

