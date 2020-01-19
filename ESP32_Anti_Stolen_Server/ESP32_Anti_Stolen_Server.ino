#include "dweetESP32.h"
#include <WiFiClientSecure.h>
#define THIG_NAME  "anti_stolen_test_YourThingName"  // Put here your thing name
#define WIFISSID "YourWiFiSSID"
#define PASSWORD "YourWiFiPassword"
#define time_loop 60000   // Time per round
#define count_loop 2      // number of same response to start Notify
#define LINE_TOKEN "............Your.Line.Token................"

dweet client;

bool LINE_Notify(String message) {
  WiFiClientSecure clients;

  if (!clients.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return false;   
  }

  String payload = "message=" + message;
  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "User-Agent: ESP32\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(payload.length()) + "\r\n";
  req += "\r\n";
  req += payload;
  // Serial.println(req);
  clients.print(req);
    
  delay(20);

  // Serial.println("-------------");
  long timeOut = millis() + 30000;
  while(clients.connected() && timeOut > millis()) {
    if (clients.available()) {
      String str = clients.readString();
      // Serial.print(str);
    }
    delay(10);
  }
  // Serial.println("-------------");

  return timeOut > millis();
}

void setup() {
    Serial.begin(115200);
    delay(10);
    client.wifiConnection(WIFISSID, PASSWORD);
}

String value_c = "";
unsigned long m = millis();
int count = 1;
void loop() {
    // specifies the args of type "String"
    String value = client.getTimeDweet(THIG_NAME);
    Serial.println(value);
    if(value!="nan"){
      if(value_c=="") {
        value_c = value;
      } else {
        if(value_c==value && m-millis()>time_loop) {
          if(count>=count_loop) {
            Serial.println("Line Notify...");
            LINE_Notify("anti_stolen_test_12345678 Stolen");
          } else {
            count++;
          }
        } else {
          value_c = value;
          count = 0;
          m = millis();
        }
      }
    }
    delay(3000); 
}
