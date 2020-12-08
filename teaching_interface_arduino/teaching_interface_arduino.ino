#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h" 

// Extract SSID and password from arduino_secrets.h
char ssid[] = SECRET_SSID;        // Network SSID (name)
char pass[] = SECRET_PASS;        // Network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // Network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

// Pin numbers
const int x_axis = 2;             // x axis button
const int y_axis = 3;             // y axis button
const int z_axis = 4;             // z axis button
const int r_angle = 5;            // roll button
const int p_angle = 6;            // pitch button
const int y_angle = 7;            // yaw button
const int waypoint_button = 8;    // waypoint button
const int gravity_button = 9;     // gravity button
const int reference_switch = 10;  // reference switch (end-effector or base)
const int direction_switch = 11;  // direction switch (positive or negative)
const int pot = A0;               // potentiometer
const int fsr_one = A1;           // FSR right
const int fsr_two = A2;           // FSR left

char *digital_buttons[]={"X_axis ","Y_axis ","Z_axis ","Roll ","Pitch ","Yaw ","Waypoint ","Gravity ","Reference ","Direction "};
char *analog_buttons[]={"Potentiometer ","FSR_1 ","FSR_2 "};


void setup() {
  // Initialise serial port
  Serial.begin(9600);

  // Wait for port to open
  while (!Serial) continue;

  // Set pin modes
  pinMode(x_axis, INPUT_PULLUP);
  pinMode(y_axis, INPUT_PULLUP);
  pinMode(z_axis, INPUT_PULLUP);
  pinMode(r_angle, INPUT_PULLUP);
  pinMode(p_angle, INPUT_PULLUP);
  pinMode(y_angle, INPUT_PULLUP);
  pinMode(waypoint_button, INPUT_PULLUP);
  pinMode(gravity_button, INPUT_PULLUP);
  pinMode(reference_switch, INPUT_PULLUP);
  pinMode(direction_switch, INPUT_PULLUP);
  
  // Check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // Don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  
  // Print out the status:
  printWifiStatus();
}


void loop() {
  // Listen for incoming clients
  WiFiClient client = server.available();

  // Skip loop if there is no client
  if (!client) return;

  // Client found
  Serial.println("New client");
  
  // Read the request and ignore the content
  while (client.available()) client.read();

  // Allocate a temporary JsonDocument (arduinojson.org/v6/assistant for capacity)
  StaticJsonDocument<500> doc;

  // Write digital pin values
  doc["x"] = digitalRead(2);
  doc["y"] = digitalRead(3);
  doc["z"] = digitalRead(4);
  doc["roll"] = digitalRead(5);
  doc["pitch"] = digitalRead(6);
  doc["yaw"] = digitalRead(7);
  doc["wp"] = digitalRead(8);
  doc["conf"] = digitalRead(9);
  doc["reference"] = digitalRead(10);
  doc["direction"] = digitalRead(11);

  // Write analog pin values
  doc["dial"] = analogRead(0);
  doc["fsr1"] = analogRead(1);
  doc["fsr2"] = analogRead(2);
  
  Serial.print(F("Sending: "));
  serializeJson(doc, Serial);
  Serial.println();

  // Write response headers
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  // Write JSON document
  serializeJsonPretty(doc, client);

  // Disconnect
  client.stop();
}


void printWifiStatus() {
  // Print the SSID of the network
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print the board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
