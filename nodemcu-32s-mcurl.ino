#define __DEBUG__

#ifdef __DEBUG__
// #define DEBUG(...) Serial.println(__VA_ARGS__)
#define DEBUG(...) \
  Serial.println(__VA_ARGS__); \
  Serial.print(" @ [SRC]:      "); \
  Serial.println(__FILE__); \
  Serial.print(" @ [LINE]:     "); \
  Serial.println(__LINE__); \
  Serial.print(" @ [FUNCTION]: "); \
  Serial.println(__func__);
#else
#define DEBUG(...)
#endif

#include <WiFiClientSecure.h>

#define RXD2 16
#define TXD2 17
#define rxSIZE (8 * 1024)
#define rxSIZES (4 * 1024)

char rxBuf[rxSIZES + 1];
char dbgBuf[rxSIZES + 1];

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
  sprintf(dbgBuf, "Serial2:%d, %d, %d, %d", 115200 * 1, SERIAL_8N1, RXD2, TXD2);
  DEBUG(dbgBuf);

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
      Serial2.println("Alive");
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
      // FileSize = 248028;  // 248028
      FileSize = 564261;
      // server_ascii = "139.162.197.165";
      server_ascii = "www.bitdove.net";
      device_security_token_ascii = "abcde12345";
      // mcurlOU("GET /DEFAULT/controller/v1/dev01/softwaremodules/6/artifacts/APP.BIN");
      mcurlOU("GET /document/bitdove-ota-solution-dm-en-1.4.pdf");
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
  // client.setInsecure();  // skip verification

  // HTTPS
  // bitdove.net
  // ISRG Root X1.crt
  // R3.crt
  static const char ca[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw
WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg
RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK
AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP
R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx
sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm
NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg
Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG
/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB
Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA
FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw
AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw
Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB
gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W
PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl
ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz
CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm
lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4
avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2
yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O
yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids
hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+
HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv
MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX
nLRbwHOoq7hHwg==
-----END CERTIFICATE-----
)KEY";

  client.setCACert(ca);

  client.setTimeout(30000);
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
  int skipc = 0;
  int resc = 0;
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
    DEBUG(lenacc);
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

    // Debug
    // if (resc >= 1) {
    //   continue;
    // }

    if (ridx == rmax && FileSize >= rmax) {
      Serial2.write((const uint8_t *)sbuf, (size_t)rmax);
      Serial2.flush();
      resc++;
      FileSize -= rmax;
      lenacc += rmax;
      ridx = 0;
    } else if (rlen >= 1 && FileSize < rmax) {
      FileSize -= rlen;
      lenacc += rlen;
      if (FileSize == 0) {
        Serial2.write((const uint8_t *)sbuf, (size_t)rlen);
        Serial2.flush();
        resc++;
        ridx = 0;
      }
    } else {
      skipc++;
    }

    delay(1000);
  }

  client.stop();

  DEBUG("==END==");
  DEBUG("skipc=" + skipc);
  DEBUG("resc=" + resc);
  DEBUG("lenacc=" + lenacc);
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
