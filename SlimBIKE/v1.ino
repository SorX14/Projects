#include <MsTimer2.h>
#include "LowPower.h"

// Pin definitions
#define REAR 5
#define FRONT 6
#define VOLTAGE A0
#define SWITCH 2
#define DEBUG 16

#define HOLD_TIME 2000

// Variables
volatile int state = 0; // The state of the device, wake, sleep, flash modes
volatile boolean blinker = false; // Control for flashing LED's
volatile unsigned long press_time = 0; // The press time of the switch
int button_value = 0; // The switch value
volatile int pressed_flag = 0; // A flag thats set when an interrupt is detected
unsigned long difference = 0; // The difference between first press and when its let go

void setup() {
  Serial.begin(115200);
  
  // Setup basic pins
  pinMode(FRONT, OUTPUT);
  pinMode(REAR, OUTPUT);
  pinMode(DEBUG, OUTPUT);
  pinMode(VOLTAGE, INPUT);
  pinMode(SWITCH, INPUT);
  
  // Setup analog reference
  analogReference(INTERNAL);
  
  // Setup the timer
  MsTimer2::set(150, FLASH);
 
  // Turn on the debug LED as we'll be on
  digitalWrite(DEBUG, HIGH);
  Serial.println("STARTING...");
  fadeBoth();

  // Start after we've done the fade
  MsTimer2::start();  
     
  // Attach the interrupts
  attachInterrupt(0, SWITCH_INT, FALLING);
}

void FLASH() {
  if (state == 1) {
    analogWrite(FRONT, 150);
    if (!blinker) {
      analogWrite(REAR, 100);
    } else {
      digitalWrite(REAR, LOW);
    }
  } else if (state == 2) {
    digitalWrite(FRONT, HIGH);
    digitalWrite(REAR, blinker);
  } else {
    digitalWrite(FRONT, LOW);
    digitalWrite(REAR, LOW);
  }
  blinker = !blinker;
}

void SWITCH_INT() {
  press_time = millis();
  pressed_flag = 1;
  
  state++;
  if (state > 2) {
    state = 0;
  }
}

void loop() { 
  button_value = digitalRead(SWITCH);

  // If we've just pressed the button
  if (pressed_flag == 1) {
    // If the button has stopped being pressed
    if (button_value == HIGH) {
      pressed_flag = 0;
    } else { // If the button is still being held
      difference = millis() - press_time;
      if (difference > HOLD_TIME) {
        // We're counting this as the button being pressed
        pressed_flag = 0;
        
        // Disable the button, the flashing, show voltage, then reenable everything
        detachInterrupt(0);
        MsTimer2::stop();
        showVoltage();
        state = 0; // Turn off after this
        MsTimer2::start();
        attachInterrupt(0, SWITCH_INT, FALLING);
      }
    }
  }

  // Flush the serial before the next loop, we get corruption otherwise
  if (state == 0 && pressed_flag == 0) {
    digitalWrite(FRONT, LOW);
    digitalWrite(REAR, LOW);
    
    Serial.flush();
    
    // Sleep until an interrupt
    digitalWrite(DEBUG, LOW); // Turn off the debug LED as we're going to sleep
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    digitalWrite(DEBUG, HIGH); // Turn on the debug LED as we're awake
  }
}

