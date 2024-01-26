#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

void setup() {
  Serial.begin(115200);
  DEBUG("Setup ESP32 Start.");
  DEBUG("Enable DEBUG mode.");
  DEBUG("Setup ESP32 completed.");

  String str = "op1:op2";
  String substr = strsep(str, String(":"));
  DEBUG(substr);
}

void loop() {
  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('\n');
    DEBUG("COM Input: " + str);
    if (str.endsWith("\r")) {
      str.remove(str.length() - 1);
    }
    String substr = strsep(str, ":");
    DEBUG("str=" + str);
    DEBUG("substr=" + substr);
    if (substr == "a") {
      DEBUG("Alive");
    }
  }
}

String strsep(String &stringp, const String &delim) {
  if (!stringp || stringp == "") return "";

  String token;
  int delimIndex = stringp.indexOf(delim);

  if (delimIndex != -1) {
    token = stringp.substring(0, delimIndex);
    stringp = stringp.substring(delimIndex + delim.length());
  } else {
    token = stringp;
    stringp = "";
  }

  return token;
}
