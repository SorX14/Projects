#include <Wire.h>
#include <MCP342x.h>
#include "LowPower.h"

// XRF node ID
#define NODE_ID 40

// Voltage divider ratio
// R2 / (R1 + R2) = 120000 / (390000 + 120000)
#define R_RATIO 0.2352941176

// Pins
#define VOLTAGE A0 // http://www.electronics2000.co.uk/calc/potential-divider-calculator.php R1 390 R2 120
#define XRF_SLEEP A2 // XRF sleep pin
#define SCALE_PWR 7 // Pin that controls the power to the scales

// Setup the ADC object
uint8_t address = 0x69;
MCP342x adc = MCP342x(address);

// Configure the ADC
MCP342x::Config config(MCP342x::channel1, MCP342x::oneShot, MCP342x::resolution18, MCP342x::gain8);
MCP342x::Config status;

// Whether we should be reading
volatile bool start_read = false;
bool start_conversion = false;

// Weight array
#define ARRAY_SIZE 20
int weight_array[ARRAY_SIZE];
int weight_itt = 0;

// Number of checks to make
int well_count = 0;
int items_to_check = 10;

// Averaging
float total_reading = 0;
float final_reading = 0;

// The reading with no weight applied
#define UNLOADED_VAL 168;
#define READING_PER_G 90.5

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Setup pins and analog reference
  pinMode(VOLTAGE, INPUT);
  pinMode(XRF_SLEEP, OUTPUT);
  pinMode(SCALE_PWR, OUTPUT);
  analogReference(INTERNAL);
  
  // We want the XRF to send a startup packet
  xrfWake();
  Serial.print(NODE_ID);
  Serial.println("|d=startup");
  xrfSleep();
  
  // Enable power to the scales
  digitalWrite(SCALE_PWR, HIGH);

  // Setup the MCP; restart and wait for it to settle
  MCP342x::generalCallReset();
  delay(1);

  // Attach the interrupt for the backlight
  attachInterrupt(1, BLIGHT_ON_INT, LOW);
}

// Backlight on interrupt
void BLIGHT_ON_INT() {
  detachInterrupt(1);
  attachInterrupt(1, BLIGHT_OFF_INT, HIGH);
  start_read = true;
}

// Backlight off interrupt
void BLIGHT_OFF_INT() {
  detachInterrupt(1);
  attachInterrupt(1, BLIGHT_ON_INT, LOW);
  start_read = false;
}

void xrfWake() {
  Serial.flush();
  delay(200);
  digitalWrite(XRF_SLEEP, LOW);
  delay(150);
}

void xrfSleep() {
   Serial.flush();
   delay(250);
   digitalWrite(XRF_SLEEP, HIGH);
}

unsigned int getVoltage() {
  delay(10);
  analogRead(VOLTAGE);
  delay(5);
  
  float cal = (analogRead(VOLTAGE) * (1100/1024.0)) / R_RATIO;
  
  return cal;
}

void loop() {
  // Once the serial output has been sent, go to sleep
  Serial.println("DEBUG SLEEP");
  Serial.flush();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  Serial.println("DEBUG WAKE");
  // We wake up here
  
  // Reset the iterator
  weight_itt = 0;
  
  // Reset values
  total_reading = 0;
  well_count = 0;
  memset(weight_array, 0, ARRAY_SIZE);

  // While we should read the input, do so
  while (start_read) {
    uint8_t err;
    long value = 0;    

    if (start_conversion) {
      err = adc.convert(config);
      start_conversion = false;
    }
    
    err = adc.read(value, status);
    if (!err && status.isReady()) {
      // Validate the reading
      int weight_cal = abs(value) - UNLOADED_VAL;
      
      // If the weight_cal is valid, add it
      if (weight_cal > 10) {
        // Add to array
        weight_array[weight_itt] = weight_cal;
             
        // Check the previous values
        for (int x = 0; x <= items_to_check; x++) {
          // Check previous entries, and compensate for looping
          int arr_pos = weight_itt - x;
          if (arr_pos < 0) {
            arr_pos += ARRAY_SIZE;
          }
          
          Serial.print("Live:\t");
          Serial.print(weight_cal);
          Serial.print("\tPrevious:\t");
          Serial.print(weight_array[arr_pos]);
          Serial.print("\tItt: (");
          Serial.print(weight_itt);
          Serial.print(",");
          Serial.print(arr_pos);
          Serial.print(")\t Well: ");
          Serial.println(well_count);
          
          // If the reading is between the last previous readings within +/- 2
          if (weight_cal >= weight_array[arr_pos] - 2 && weight_cal <= weight_array[arr_pos] + 2) {
            well_count++;
            total_reading += weight_cal;
            
            // If we have enough readings, break and send
            if (well_count >= items_to_check) {
              final_reading = (total_reading / items_to_check);
              goto bailout;
            }
          } else {
            // If its not within bounds, reset the count
            well_count = 0;
            total_reading = 0;
          }
        }
                        
        // Increase the iterator
        weight_itt++;
        
        // If the iterator is greater than array size, set back to 0
        if (weight_itt >= ARRAY_SIZE) {
          weight_itt = 0;
        }
      }
      // Approx 235ms between readings     
      // Reset the conversion flag
      start_conversion = true;
    }
  }
  
  bailout:

 
  // We only get here once the backlight has been turned off or we have a value   
  // Send the result
  if (final_reading > 0) {
    xrfWake();
    Serial.print(NODE_ID);
    Serial.print("|d=");
    Serial.print(final_reading);
    Serial.print(",");
    Serial.print(final_reading * READING_PER_G);
    Serial.print(",");
    Serial.print(getVoltage());
    Serial.print(",");
    Serial.print(millis());
    Serial.println();     
    xrfSleep();
  }
  
  // Don't sleep until the backlight has turned off
  while (start_read) {}
}

