#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#include <WiFiClientSecure.h>

#define RXD2 16
#define TXD2 17
#define rxSIZE (8 * 1024)
#define rxSIZES (4 * 1024)

char rxBuf[rxSIZES + 1];

String wifi_ssid;
String wifi_password;

uint32_t FileSize = 0;
char *server_ascii;
char tenant_ascii[32];
char device_target_id_ascii[32];
char *device_security_token_ascii;

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  DEBUG("Setup ESP32 Start.");
  DEBUG("Enable DEBUG mode.");

  Serial2.begin(115200 * 1, SERIAL_8N1, RXD2, TXD2);
  sprintf(rxBuf, "size:%d;", 123);
  DEBUG(rxBuf);
  Serial2.write((const uint8_t *)rxBuf, (size_t)9);
  DEBUG("Setup ESP32 completed.");
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
    } else if (substr == "r") {
      DEBUG("Reboot");
      ESP.restart();
    } else if (substr == "wc") {
      substr = strsep(str, ",");
      wifi_ssid = substr;
      DEBUG(wifi_ssid);
      substr = strsep(str, ",");
      wifi_password = substr;
      DEBUG(wifi_password);
      //
      // Conncect
      //
      int retry = 0;
      WiFi.begin(wifi_ssid, wifi_password);
      while (WiFi.status() != WL_CONNECTED && retry <= 10) {
        delay(1000);
        retry++;
      }
    } else if (substr == "dw") {
      FileSize = 248028;  // 248028
      server_ascii = "139.162.197.165";
      device_security_token_ascii = "abcde12345";
      mcurlOU("GET /DEFAULT/controller/v1/dev01/softwaremodules/6/artifacts/APP.BIN");
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

int mcurlOU(const char *murl) {
  T2ConnectAndSendRequest(murl);
  DEBUG("[TWaitDownload]--");
  TWaitDownload();
  return 0;
}

int T2ConnectAndSendRequest(const char *murl) {
  //
  // Connect to Server
  //
  Serial.println();
  Serial.println(String("Attempting to connect to Server:") + String(server_ascii));
  client.setInsecure();  // skip verification
  client.setTimeout(30);
  while (client.connect(server_ascii, 443) == 0) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println(String("Connect to Server ") + String(server_ascii));
#if 1
  Serial.println(String(murl));
  Serial.println("Authorization: TargetToken " + String(device_security_token_ascii));
#endif
  client.println(String(murl) + " HTTP/1.1");
  DEBUG(String(murl) + " HTTP/1.1");
  client.println(String("Host: ") + String(server_ascii));
  client.println("Authorization: TargetToken " + String(device_security_token_ascii));
  client.println("Accept: */*");
  client.println("Accept-Encoding: gzip, deflate, br");
  client.println("Connection: keep-alive");
  client.print("\r\n\r\n");
  return 0;
}

int TWaitDownload(void) {
  while (client.connected()) {
    DEBUG("[TWaitDownload]1");
    if (client.available()) {
      DEBUG("[TWaitDownload]2");
      String s = client.readStringUntil('\n');
#if 1
      Serial.println(s);
#endif
      if (s == "\r") {
#if 1
        Serial.println("headers received");
#endif
        break;
      }
    }
    delay(10);
  }

  DEBUG("-----");
  uint32_t lenacc = 0;
  uint32_t len;
  uint32_t rmax = 4096;
  uint32_t ridx = 0;
  char sbuf[4096];

  while (FileSize > 0) {
    DEBUG("=====");
    uint32_t rlen = 0;
    len = client.available();
    rlen = min(len, rmax);
    rlen = min(rlen, FileSize);
    DEBUG("FileSize=" + String(FileSize));
    DEBUG(rlen);
    DEBUG(ridx);

    if (rlen + ridx >= rmax) {
      rlen = rmax - ridx;
    }
    DEBUG(rlen);

    client.readBytes(rxBuf, rlen);

    for (int i = ridx; i < (ridx + rlen); i++) {
      // DEBUG(i);
      sbuf[i] = rxBuf[i - ridx];
    }

    ridx += rlen;

    if (ridx == rmax && FileSize >= rmax) {
      Serial2.write((const uint8_t *)sbuf, (size_t)rmax);
      FileSize -= rmax;
      lenacc += rmax;
      ridx = 0;
    } else if (rlen >= 1 && FileSize < rmax) {
      FileSize -= rlen;
      lenacc += rlen;
      if (FileSize == 0) {
        Serial2.write((const uint8_t *)sbuf, (size_t)rlen);
        ridx = 0;
      }
    }

    delay(50);
  }

  client.stop();

  DEBUG(lenacc);
  return 0;
}

int TWaitDownload2(void) {
  while (client.connected()) {
    DEBUG("[TWaitDownload]1");
    if (client.available()) {
      DEBUG("[TWaitDownload]2");
      String s = client.readStringUntil('\n');
#if 1
      Serial.println(s);
#endif
      if (s == "\r") {
#if 1
        Serial.println("headers received");
#endif
        break;
      }
    }

    delay(10);
  }

  DEBUG("-----");
  uint32_t lenacc = 0;
  uint32_t len;
  uint32_t ri = 0;
  DEBUG("1-----");
  while (FileSize > 0) {
    DEBUG("2-----");
    if (FileSize >= rxSIZE) {
      DEBUG("-----while FileSize=" + String(FileSize));
      len = client.available();
      DEBUG("-----while len=" + String(len));
      if (len >= rxSIZE) {
        client.readBytes(rxBuf, rxSIZE);
        Serial.write((const uint8_t *)rxBuf, (size_t)rxSIZE);
        FileSize -= rxSIZE;
#if 1
        Serial.println(FileSize);
#endif

        lenacc += rxSIZE;

        //
        // 1/(115200*32)*10*8*1024=22.22ms
        //
        delay(50);
      } else {
        client.readBytes(rxBuf, len);
        Serial.write((const uint8_t *)rxBuf, len);
        FileSize -= len;

        lenacc += len;
        delay(50);
      }
    } else if (FileSize > 0) {
      DEBUG("-----while FileSize2=" + String(FileSize));
      len = client.available();
      DEBUG("-----while len FileSize22=" + String(len));
      if (len >= FileSize) {
        DEBUG("-----while FileSize22=" + String(len));
        client.readBytes(rxBuf, len);
        Serial.write((const uint8_t *)rxBuf, (size_t)len);
        FileSize -= len;
#if 1
        Serial.println(FileSize);
#endif
        lenacc += len;
      } else {
        client.readBytes(rxBuf, len);
        Serial.write((const uint8_t *)rxBuf, (size_t)len);
        FileSize -= len;
#if 1
        Serial.println(len);
#endif
        lenacc += len;
      }
    }

    delay(50);
  }

  client.stop();

  DEBUG(lenacc);
  return 0;
}
