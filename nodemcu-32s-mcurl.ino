#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Setup ESP32 Start. ");
  DEBUG("Enable DEBUG mode.");
  Serial.println("Setup ESP32 completed.");
}

void loop() {
  // put your main code here, to run repeatedly:
}
