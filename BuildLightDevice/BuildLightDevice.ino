#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// Step 1. Create SECTRET_* defines
#include "Secrets.h"
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASSWORD;

// Step 2. Connect pins 27 & 26 to RED and GREEN on light strip
const int redPin = 27;
const int greenPin = 26;

const char* host = "buildlight";

WebServer server(80);

void handleIndex() {
  server.send(200, "text/plain", "hello chat room!");
}

void handleColor() {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  for (uint8_t i = 0; i < server.args(); i++) {
    auto name = server.argName(i);
    auto value = atoi(server.arg(i).c_str());
    if (name.length() > 0) {
      switch (name[0]) {
        case 'r':
          red = value;
          break;
        case 'g':
          green = value;
          break;
        case 'b':
          blue = value;
          break;
      }
    }
  }
  
  digitalWrite(redPin, red > 0 ? 0 : 1);
  digitalWrite(greenPin, green > 0 ? 0 : 1);
  
  Serial.printf("SET COLOR red=%d, green=%d, blue=%d\n",
    (int)red, (int)green, (int)blue);
  server.send(200, "text/plain", "color set");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(redPin, OUTPUT);
  digitalWrite(redPin, 1);
  pinMode(greenPin, OUTPUT);
  digitalWrite(greenPin, 1);

  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(host)) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleIndex);

  server.on("/color", HTTP_POST, handleColor);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
