#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "wifi ssid";
const char* password = "wifi password";
int sensorValue = 0;        // value read from the pot
float Voltage = 0;
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 13; // Analog output pin that the LED is attached to
static char outstr[15];
int visit_count=0;          //Web visitor counter

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  visit_count = visit_count + 1;
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  int32_t rssi = WiFi.RSSI();           // Read RSSI from Wifi
  Serial.println(rssi);                 // Send RSSI to serial port
  sensorValue = analogRead(analogInPin);  //Take an A-D reading on Pin A0
  Voltage = sensorValue; // * 0.006698;       // Multiply by 0.006698 to convert to Volts (note 3.3V supply)
  dtostrf(Voltage,4, 2, outstr);          // Convert float to string (1 digit efore DP, two decimal places)
  Serial.println(outstr);                 // Send Voltage to serial port
  String Message="<http><h1>WeMos D1 as IOT web server<br><br>";
  Message+="Analogue input:    ";
  Message+=outstr;
  Message+=" Volts.<br>";                     // Construct outut message
  Message+="Wifi access point: ";
  Message+=ssid;
  Message+="<br>";                           // Construct outut message
  Message+="Wifi signal level: ";
  Message+=rssi;
  Message+=" dBm.";
  Message+="<br><br>";                           // Construct outut message
  Message+="This page has been viewed ";
  Message+=visit_count;
  Message+=" times since the last reset.</h1></html>";
  
  server.send(200, "text/html", Message); // Send output message back to client browser
  delay(200);
  digitalWrite(led, 0);

  
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
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

  if (MDNS.begin("wifi-voltmeter")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
