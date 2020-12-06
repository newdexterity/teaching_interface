/*
  WiFi Web Server

 A simple web server that shows the value of the analog input pins.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * Analog inputs attached to pins A0 through A5 (optional)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe

 */

#include <SPI.h>
#include <WiFiNINA.h>


#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

// Constants used to set pin numbers:
const int x_axis = 2; // x axis button
const int y_axis = 3; // y axis button
const int z_axis = 4; // z axis button
const int r_angle = 5; // roll button
const int p_angle = 6; // pitch button
const int y_angle = 7; // yaw button
const int waypoint_button = 8; // waypoint button
const int gravity_button = 9; // gravity button
const int reference_switch = 10; // reference switch (end-effector or base)
const int direction_switch = 11; // direction switch (positive or negative)
const int pot = A0; // potentiometer
const int fsr_one = A1; // FSR right
const int fsr_two = A2; // FSR left

char *digital_buttons[]={"X_axis ","Y_axis ","Z_axis ","Roll ","Pitch ","Yaw ","Waypoint ","Gravity ","Reference ","Direction "};
char *analog_buttons[]={"Potentiometer ","FSR_1 ","FSR_2 "};

// Variables:
//int x_state = LOW; // pin initial state
//int y_state = LOW; // pin initial state
//int z_state = LOW; // pin initial state
//int roll_state = LOW; // pin initial state
//int pitch_state = LOW; // pin initial state
//int yaw_state = LOW; // pin initial state
//int waypoint_state = LOW; // pin initial state
//int gravity_state = LOW; // pin initial state
//int reference_state = LOW; // pin initial state
//int direction_state = LOW; // pin initial state
//int pot_value = 0; // pin initial state
//int fsr_one_value = 0; // pin initial state
//int fsr_two_value = 0; // pin initial state

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

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
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 0.1");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int digitalpin = 2; digitalpin < 12; digitalpin++) {
            int digitalReading = digitalRead(digitalpin);
            client.print(digital_buttons[digitalpin-2]);
            client.print(digitalReading);
            client.println("<br />");
          }
          for (int analogpin = 0; analogpin < 3; analogpin++) {
            int analogReading = analogRead(analogpin);
            client.print(analog_buttons[analogpin]);
            client.print(analogReading);
            client.println("<br />");
          }
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
