#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>

// AP mode credentials
const char* ap_ssid = "ESP8266_AP";
const char* ap_password = "ap_password";

// Relay pins
#define RELAY1_PIN 5
#define RELAY2_PIN 4
#define RELAY3_PIN 0
#define RELAY4_PIN 2

bool relay1Status = false;
bool relay2Status = false;
bool relay3Status = false;
bool relay4Status = false;

// Create an instance of the server
ESP8266WebServer server(80);

// WiFi credentials
String ssid;
String password;

// Network configuration
IPAddress local_IP;
IPAddress gateway;
IPAddress subnet;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize relay pins as outputs
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  setRelayPins();

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Load configuration
  loadConfig();

  // Attempt to connect to Wi-Fi using the loaded credentials
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid.c_str(), password.c_str());

  // Wait for connection
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  // Check if connected to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    // Start AP mode if not connected
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("Started AP mode");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  // Define the web server routes
  server.on("/", handleRoot);
  server.on("/relay/toggle/1", handleRelay1Toggle);
  server.on("/relay/toggle/2", handleRelay2Toggle);
  server.on("/relay/toggle/3", handleRelay3Toggle);
  server.on("/relay/toggle/4", handleRelay4Toggle);
  server.on("/relay/status/1", []() { server.send(200, "text/plain", relay1Status ? "ON" : "OFF"); });
  server.on("/relay/status/2", []() { server.send(200, "text/plain", relay2Status ? "ON" : "OFF"); });
  server.on("/relay/status/3", []() { server.send(200, "text/plain", relay3Status ? "ON" : "OFF"); });
  server.on("/relay/status/4", []() { server.send(200, "text/plain", relay4Status ? "ON" : "OFF"); });
  server.on("/config", handleConfig);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle client requests
  server.handleClient();
}

void handleRoot() {
  String html = "<html><head><title>ESP8266 Web Interface</title>";
  html += "<style>";
  html += "body {display: flex; justify-content: center; align-items: center; height: 100vh; background-color: #f0f0f0; margin: 0;}";
  html += ".container {text-align: center;}";
  html += ".card {background: white; padding: 20px; margin: 10px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);}";
  html += ".switch {position: relative; display: inline-block; width: 60px; height: 34px;}";
  html += ".switch input {opacity: 0; width: 0; height: 0;}";
  html += ".slider {position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s;}";
  html += ".slider:before {position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s;}";
  html += "input:checked + .slider {background-color: #2196F3;}";
  html += "input:checked + .slider:before {transform: translateX(26px);}";
  html += ".slider.round {border-radius: 34px;}";
  html += ".slider.round:before {border-radius: 50%;}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>ESP8266 Web Interface</h1>";
  html += "<div class='card'><p>Relay 1 Status: <span id='relay1'>" + String(relay1Status ? "ON" : "OFF") + "</span></p>";
  html += "<label class='switch'><input type='checkbox' id='relay1Toggle' onclick=\"toggleRelay(1)\" " + String(relay1Status ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "<div class='card'><p>Relay 2 Status: <span id='relay2'>" + String(relay2Status ? "ON" : "OFF") + "</span></p>";
  html += "<label class='switch'><input type='checkbox' id='relay2Toggle' onclick=\"toggleRelay(2)\" " + String(relay2Status ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "<div class='card'><p>Relay 3 Status: <span id='relay3'>" + String(relay3Status ? "ON" : "OFF") + "</span></p>";
  html += "<label class='switch'><input type='checkbox' id='relay3Toggle' onclick=\"toggleRelay(3)\" " + String(relay3Status ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "<div class='card'><p>Relay 4 Status: <span id='relay4'>" + String(relay4Status ? "ON" : "OFF") + "</span></p>";
  html += "<label class='switch'><input type='checkbox' id='relay4Toggle' onclick=\"toggleRelay(4)\" " + String(relay4Status ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "<p><a href='/config'>Configure WiFi and Network</a></p>";
  html += "</div>";
  html += "<script>";
  html += "function toggleRelay(relay) {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/relay/toggle/' + relay, true);";
  html += "  xhr.send();";
  html += "  xhr.onload = function() {";
  html += "    if (xhr.status == 200) {";
  html += "      document.getElementById('relay' + relay).innerText = xhr.responseText;";
  html += "      document.getElementById('relay' + relay + 'Toggle').checked = xhr.responseText === 'ON';";
  html += "    }";
  html += "  };";
  html += "}";
  html += "function updateStatus() {";
  html += "  for (var i = 1; i <= 4; i++) {";
  html += "    var xhr = new XMLHttpRequest();";
  html += "    xhr.open('GET', '/relay/status/' + i, true);";
  html += "    xhr.send();";
  html += "    xhr.onload = function() {";
  html += "      if (xhr.status == 200) {";
  html += "        document.getElementById('relay' + i).innerText = xhr.responseText;";
  html += "        document.getElementById('relay' + i + 'Toggle').checked = xhr.responseText === 'ON';";
  html += "      }";
  html += "    };";
  html += "  }";
  html += "}";
  html += "setInterval(updateStatus, 1000);";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}


void handleRelay1Toggle() {
  relay1Status = !relay1Status;
  digitalWrite(RELAY1_PIN, relay1Status ? HIGH : LOW);
  saveConfig();
  server.send(200, "text/plain", relay1Status ? "ON" : "OFF");
}

void handleRelay2Toggle() {
  relay2Status = !relay2Status;
  digitalWrite(RELAY2_PIN, relay2Status ? HIGH : LOW);
  saveConfig();
  server.send(200, "text/plain", relay2Status ? "ON" : "OFF");
}

void handleRelay3Toggle() {
  relay3Status = !relay3Status;
  digitalWrite(RELAY3_PIN, relay3Status ? HIGH : LOW);
  saveConfig();
  server.send(200, "text/plain", relay3Status ? "ON" : "OFF");
}

void handleRelay4Toggle() {
  relay4Status = !relay4Status;
  digitalWrite(RELAY4_PIN, relay4Status ? HIGH : LOW);
  saveConfig();
  server.send(200, "text/plain", relay4Status ? "ON" : "OFF");
}

void handleConfig() {
  if (server.method() == HTTP_POST) {
    ssid = server.arg("ssid");
    password = server.arg("password");
    local_IP.fromString(server.arg("ip"));
    gateway.fromString(server.arg("gateway"));
    subnet.fromString(server.arg("subnet"));
    saveConfig();
    // Restart to apply new credentials
    ESP.restart();
  } else {
    String html = "<html><body><h1>Configure WiFi and Network</h1>";
    html += "<form action='/config' method='POST'>";
    html += "SSID: <input type='text' name='ssid'><br>";
    html += "Password: <input type='text' name='password'><br>";
    html += "Local IP: <input type='text' name='ip'><br>";
    html += "Gateway: <input type='text' name='gateway'><br>";
    html += "Subnet: <input type='text' name='subnet'><br>";
    html += "<input type='submit' value='Save'>";
    html += "</form></body></html>";
    server.send(200, "text/html", html);
  }
}

void loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return;
  }

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.println("Failed to parse config file");
    return;
  }

  ssid = doc["ssid"].as<String>();
  password = doc["password"].as<String>();
  local_IP.fromString(doc["local_IP"].as<String>());
  gateway.fromString(doc["gateway"].as<String>());
  subnet.fromString(doc["subnet"].as<String>());
  relay1Status = doc["relay1Status"];
  relay2Status = doc["relay2Status"];
  relay3Status = doc["relay3Status"];
  relay4Status = doc["relay4Status"];
  setRelayPins();
}

void saveConfig() {
  DynamicJsonDocument doc(1024);
  doc["ssid"] = ssid;
  doc["password"] = password;
  doc["local_IP"] = local_IP.toString();
  doc["gateway"] = gateway.toString();
  doc["subnet"] = subnet.toString();
  doc["relay1Status"] = relay1Status;
  doc["relay2Status"] = relay2Status;
  doc["relay3Status"] = relay3Status;
  doc["relay4Status"] = relay4Status;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return;
  }

  serializeJson(doc, configFile);
}

void setRelayPins() {
  digitalWrite(RELAY1_PIN, relay1Status ? HIGH : LOW);
  digitalWrite(RELAY2_PIN, relay2Status ? HIGH : LOW);
  digitalWrite(RELAY3_PIN, relay3Status ? HIGH : LOW);
  digitalWrite(RELAY4_PIN, relay4Status ? HIGH : LOW);
}
