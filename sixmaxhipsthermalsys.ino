#include <Adafruit_MAX31865.h>
#include <SPI.h>

// -------------------------------
// Output mode
// -------------------------------
#define MODE_DEBUG   0
#define MODE_PLOTTER 1
#define MODE_CSV     2

// Choose one mode here:
// MODE_CSV     = output Max1 to Max6 for Python/Excel
// MODE_PLOTTER = show six curves in Serial Plotter
// MODE_DEBUG   = detailed Serial Monitor output
#define OUTPUT_MODE 2

// -------------------------------
// CS pins for the 6 MAX31865 chips
// Sensor 1 -> CS 10
// Sensor 2 -> CS 9
// Sensor 3 -> CS 8
// Sensor 4 -> CS 7
// Sensor 5 -> CS 6
// Sensor 6 -> CS 5
// -------------------------------
const int CS_PINS[6] = {10, 9, 8, 7, 6, 5};

// PT100 settings
#define RREF      430.0
#define RNOMINAL 100.0

// -------------------------------
// Two-point calibration values
// -------------------------------
float refLow = 10.8;
float refHigh = 92.8;

float rawLow[6] = {
  11.66,  // Max1 raw low temp
  11.55,  // Max2 raw low temp
  11.62,  // Max3 raw low temp
  11.66,  // Max4 raw low temp
  11.72,  // Max5 raw low temp
  11.89   // Max6 raw low temp
};

float rawHigh[6] = {
  92.75,  // Max1 raw high temp
  92.65,  // Max2 raw high temp
  92.78,  // Max3 raw high temp
  92.89,  // Max4 raw high temp
  92.27,  // Max5 raw high temp
  92.58   // Max6 raw high temp
};

// Calibration equation:
// calibratedTemp = slope * rawTemp + intercept
float slope[6];
float intercept[6];

// -------------------------------
// Create 6 MAX31865 objects
// -------------------------------
Adafruit_MAX31865 max1 = Adafruit_MAX31865(CS_PINS[0]);
Adafruit_MAX31865 max2 = Adafruit_MAX31865(CS_PINS[1]);
Adafruit_MAX31865 max3 = Adafruit_MAX31865(CS_PINS[2]);
Adafruit_MAX31865 max4 = Adafruit_MAX31865(CS_PINS[3]);
Adafruit_MAX31865 max5 = Adafruit_MAX31865(CS_PINS[4]);
Adafruit_MAX31865 max6 = Adafruit_MAX31865(CS_PINS[5]);

Adafruit_MAX31865* sensors[6] = {
  &max1, &max2, &max3, &max4, &max5, &max6
};

// -------------------------------
// Calculate calibration constants
// -------------------------------
void calculateCalibration() {
  for (int i = 0; i < 6; i++) {
    float denominator = rawHigh[i] - rawLow[i];

    if (denominator > -0.001 && denominator < 0.001) {
      slope[i] = 1.0;
      intercept[i] = 0.0;
    } else {
      slope[i] = (refHigh - refLow) / denominator;
      intercept[i] = refLow - slope[i] * rawLow[i];
    }
  }
}

// -------------------------------
// Calibration function
// -------------------------------
float calibrateTemperature(int index, float rawTemp) {
  return slope[index] * rawTemp + intercept[index];
}

// -------------------------------
// Read calibrated temperature from one MAX31865
// -------------------------------
float readCalibratedTemperature(int index) {
  float rawTemp = sensors[index]->temperature(RNOMINAL, RREF);
  float calTemp = calibrateTemperature(index, rawTemp);
  return calTemp;
}

// -------------------------------
// Fault printing function
// -------------------------------
void printFault(uint8_t fault) {
  if (fault & MAX31865_FAULT_HIGHTHRESH) {
    Serial.print("RTD High Threshold; ");
  }
  if (fault & MAX31865_FAULT_LOWTHRESH) {
    Serial.print("RTD Low Threshold; ");
  }
  if (fault & MAX31865_FAULT_REFINLOW) {
    Serial.print("REFIN- > 0.85 x Bias; ");
  }
  if (fault & MAX31865_FAULT_REFINHIGH) {
    Serial.print("REFIN- < 0.85 x Bias or FORCE- open; ");
  }
  if (fault & MAX31865_FAULT_RTDINLOW) {
    Serial.print("RTDIN- < 0.85 x Bias or FORCE- open; ");
  }
  if (fault & MAX31865_FAULT_OVUV) {
    Serial.print("Under/Over voltage; ");
  }
}

// -------------------------------
// Debug function
// Detailed Serial Monitor output
// -------------------------------
void readOneSensorDebug(int index) {
  Adafruit_MAX31865* sensor = sensors[index];

  uint16_t rtd = sensor->readRTD();

  float ratio = rtd;
  ratio /= 32768.0;

  float resistance = RREF * ratio;
  float rawTemp = sensor->temperature(RNOMINAL, RREF);
  float calTemp = calibrateTemperature(index, rawTemp);

  Serial.print("Max");
  Serial.print(index + 1);

  Serial.print(" | Raw RTD: ");
  Serial.print(rtd);

  Serial.print(" | Ratio: ");
  Serial.print(ratio, 8);

  Serial.print(" | Resistance: ");
  Serial.print(resistance, 3);
  Serial.print(" ohm");

  Serial.print(" | Raw Temp: ");
  Serial.print(rawTemp, 2);
  Serial.print(" C");

  Serial.print(" | Cal Temp: ");
  Serial.print(calTemp, 2);
  Serial.print(" C");

  Serial.print(" | slope: ");
  Serial.print(slope[index], 6);

  Serial.print(" | intercept: ");
  Serial.print(intercept[index], 6);

  uint8_t fault = sensor->readFault();

  if (fault) {
    Serial.print(" | FAULT 0x");
    Serial.print(fault, HEX);
    Serial.print(" | ");
    printFault(fault);
    sensor->clearFault();
  }

  Serial.println();
}

// -------------------------------
// Plot function
// Use this for Arduino Serial Plotter
// -------------------------------
void plotOneSetOfData() {
  for (int i = 0; i < 6; i++) {
    float calTemp = readCalibratedTemperature(i);

    Serial.print("Max");
    Serial.print(i + 1);
    Serial.print(":");
    Serial.print(calTemp, 2);

    if (i < 5) {
      Serial.print("\t");
    } else {
      Serial.println();
    }
  }
}

// -------------------------------
// CSV function
// Use this for Python/Excel
// Output sequence: Max1, Max2, Max3, Max4, Max5, Max6
// -------------------------------
void printCSVHeader() {
  Serial.println("Max1,Max2,Max3,Max4,Max5,Max6");
}

void storeOneSetOfDataCSV() {
  for (int i = 0; i < 6; i++) {
    float calTemp = readCalibratedTemperature(i);

    Serial.print(calTemp, 2);

    if (i < 5) {
      Serial.print(",");
    } else {
      Serial.println();
    }
  }
}

// -------------------------------
// Setup
// -------------------------------
void setup() {
  Serial.begin(9600);
  delay(1000);

  // Set every CS pin HIGH before starting
  for (int i = 0; i < 6; i++) {
    pinMode(CS_PINS[i], OUTPUT);
    digitalWrite(CS_PINS[i], HIGH);
  }

  SPI.begin();

  // Use this for 3-wire PT100 sensors
  for (int i = 0; i < 6; i++) {
    sensors[i]->begin(MAX31865_3WIRE);
  }

  calculateCalibration();

  if (OUTPUT_MODE == MODE_DEBUG) {
    Serial.println();
    Serial.println("Six MAX31865 PT100 calibrated test starting...");
    Serial.println("Baud rate: 9600");
    Serial.println("Debug mode");
    Serial.println("Setup complete.");
    Serial.println();
  }

  if (OUTPUT_MODE == MODE_CSV) {
    printCSVHeader();
  }
}

// -------------------------------
// Main loop
// -------------------------------
void loop() {
  if (OUTPUT_MODE == MODE_CSV) {
    storeOneSetOfDataCSV();
    delay(500);
  }

  else if (OUTPUT_MODE == MODE_PLOTTER) {
    plotOneSetOfData();
    delay(500);
  }

  else if (OUTPUT_MODE == MODE_DEBUG) {
    static int readingNumber = 0;
    readingNumber++;

    Serial.print("Reading ");
    Serial.println(readingNumber);

    for (int i = 0; i < 6; i++) {
      readOneSensorDebug(i);
      delay(100);
    }

    Serial.println();
    delay(1000);
  }
}