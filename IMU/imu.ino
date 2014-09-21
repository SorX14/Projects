#include <Wire.h>

#define COMPASS_ADDRESS 0x1E

float gauss_scale = 0.92;		// Guass scaling
int16_t mag_x, mag_y, mag_z;     // Compass values

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  Wire.begin();

  setupCompass();
  Serial.println("Compass setup complete");
}

void loop() {
  readCompass();
  Serial.print("X: ");
  Serial.print(mag_x * gauss_scale);
  Serial.print("\tY: ");
  Serial.print(mag_y * gauss_scale);
  Serial.print("\tZ: ");
  Serial.println(mag_z * gauss_scale);

  delay(100);
}

void setupCompass() {
  // Sets the first (0x00) register to be 8 samples averaged, 15 hz, and normal measurement configuration
  writeDevice(COMPASS_ADDRESS, 0x00, 0x70);

  // Setup the scale to 1.3
  writeDevice(COMPASS_ADDRESS, 0x01, 0x20);

  // Set to continuous measurement mode
  writeDevice(COMPASS_ADDRESS, 0x02, 0x00);
}

void readCompass() {
  // We have to ask for a reading first, then get the result in one read. Order is definitely X then Z then Y
  int16_t* buffer = readDevice(COMPASS_ADDRESS, 0x03, 6);
  mag_x = ((int16_t)buffer[0] << 8) | buffer[1];
  mag_z = ((int16_t)buffer[2] << 8) | buffer[3];
  mag_y = ((int16_t)buffer[4] << 8) | buffer[5];
}

// Read a device after sending a data packet first
int16_t* readDevice (int I2CAddress, int address, int length) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(address);
  Wire.endTransmission();

  return readDevice(I2CAddress, length);
}

// Read a device
int16_t* readDevice (int I2CAddress, int length) {
  Wire.requestFrom(I2CAddress, length);

  int16_t buffer[length];
  if (Wire.available() == length) {
    for (uint8_t i = 0; i < length; i++) {
      buffer[i] = Wire.read();
    }
  }

  return buffer;
}

// Write to a device
void writeDevice (int I2CAddress, int address, int data) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

