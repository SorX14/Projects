int getVoltage() {
  analogRead(VOLTAGE);
  delay(5);
  double voltage = analogRead(VOLTAGE) * (1.1 / 1023.0);
  double cal_voltage = voltage * (5.13 / 0.4l);
  
  return cal_voltage * 100;
}

void voltBlink(int val) {
  for (int i = 0; i < (val / 5); i++) {
    blinkSlow();
  }
  
  delay(300);
  
  for (int i = 0; i < (val % 5); i++) {
    blinkFast();
  }
}

void showVoltage() {
  digitalWrite(FRONT, LOW);
  digitalWrite(REAR, LOW);
  delay(500);
  int voltage = getVoltage();
  
  int whole = voltage / 100;
  int fraction = voltage % 100;
  
  Serial.print(whole);
  Serial.print(".");
  Serial.println(fraction);
  
  voltBlink(whole);
  fadeFront();
  voltBlink(fraction);
}


void blinkSlow() {
  digitalWrite(FRONT, HIGH);
  delay(500);
  digitalWrite(FRONT, LOW);
  delay(300);
}

void blinkFast() {
  digitalWrite(FRONT, HIGH);
  delay(300);
  digitalWrite(FRONT, LOW);
  delay(200);
}

void fadeFront() {
  delay(200);
  float in, out;
  for (in = 4.712; in < 10.995; in = in + 0.001) {
    out = sin(in) * 127.5 + 127.5;
    analogWrite(FRONT, out);
  }
  delay(200);
}

void fadeBoth() {
  float in, out;
  for (in = 4.712; in < 10.995; in = in + 0.001) {
    out = sin(in) * 127.5 + 127.5;
    analogWrite(FRONT, out);
    analogWrite(REAR, out);
  }
}
  
