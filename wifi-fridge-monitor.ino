#include <SPI.h>
#include <WiFi101.h>
#include "LCD.h"
#include "pin_definitions.h"
#include "secrets.h" // Copy example_secrets.h to secrets.h

// Then enter values for all fields in secrets.h
char ssid[] = SECRET_SSID;        // wifi ssid
char pass[] = SECRET_PASS;        // wifi network password (use for WPA, or use as key for WEP)
char server[] = SERVER_HOST_NAME; //Host name ex: www.myserver.foo
char urlpath[] = URL_PATH;        //Path to api endpoint ex: /api/myfridge

int status = WL_IDLE_STATUS;      // the WiFi radio's status

WiFiSSLClient client;

//Global temperature variables
float fridgeF, fridgeC;
float freezerF, freezerC;
float ambientF, ambientC;
long rssi;

//Timers
const int lcdInterval=1000;
unsigned long previousLcdMillis=0;

const int dataInterval=300000;
unsigned long previousDataMillis=0;


void setup() {
  WiFi.setPins(WIFI_CS, WIFI_IRQ, WIFI_RST, WIFI_EN);
  
  Serial.begin(115200);

  lcdSetup();
  lcdBlankScreen();
  lcdDrawStartScreen();
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to wifi network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to wifi");

  gatherData();
  printWiFiStatus();
}



////////////////////////////////////
// START MAIN LOOP
////////////////////////////////////
void loop() {
  // Get latest sensor data on every loop
  gatherData();

  // Update LCD Screen on interval
  unsigned long currentLcdMillis = millis();
  if ((unsigned long)(currentLcdMillis - previousLcdMillis) >= lcdInterval) {
    lcdDrawScreen1();
    previousLcdMillis = currentLcdMillis;
  }


  // Upload data to API on interval
  unsigned long currentDataMillis = millis();
  if ((unsigned long)(currentDataMillis - previousDataMillis) >= dataInterval) {
    dataUploadToAPI();
    previousDataMillis = currentDataMillis;
  }
  
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println();
    //Serial.println("disconnecting from server.");
    client.stop();
  }
}
////////////////////////////////////
// END MAIN LOOP
////////////////////////////////////

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printWifiRSSI() {
  // print the received signal strength:
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String batteryDataString () {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  String vbatString = String(measuredvbat);
  String data = ("\"battery-voltage\": \"" + vbatString + "\"");
  return data;
}

float readTempSensor(byte pin) {
  int reading = analogRead(pin);
  float voltage = reading * 3.3;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  return temperatureC;
}

float convertCtoF(float temperatureC) {
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  return temperatureF;
}

void gatherData() {
  fridgeC = readTempSensor(FRIDGE_TEMP_PIN);
  fridgeF = convertCtoF(fridgeC);
  delay(30);
  freezerC = readTempSensor(FREEZER_TEMP_PIN);
  freezerF = convertCtoF(freezerC);
  delay(30);
  ambientC = readTempSensor(AMBIENT_TEMP_PIN);
  ambientF = convertCtoF(ambientC);
  rssi = WiFi.RSSI();
}

String fridgeTempDataString() {
  String tempFString = String(fridgeF);
  String data = ("\"fridge-temp\": \"" + tempFString + "\"");
  return data;
}

String freezerTempDataString() {
  String tempFString = String(freezerF);
  String data = ("\"freezer-temp\": \"" + tempFString + "\"");
  return data;
}

String rssiDataString() {
  String rssiString = String(rssi);
  String data = String("\"rssi\": \"" + rssiString + "\"");
  return data;
}

String buildDataString() {
  String rssiData = rssiDataString();
  String deviceidData = String("\"device-id\": \"123XYZ\"");
  //fridgeTempDataString();
  String fridgeTempData = fridgeTempDataString();
  String freezerTempData = freezerTempDataString();
  String ambientTempData = String("\"ambient-temp\": \"92\"");
  String batteryData = batteryDataString();
  String data = String("{" + deviceidData + "," + rssiData + "," + fridgeTempData + "," + freezerTempData + "," + batteryData + "," + ambientTempData + "}");
  Serial.print("DATA: ");
  Serial.println(data);
  return data;
}

void dataUploadToAPI() {
  String data = buildDataString();
  Serial.print("\nDATA: ");
  Serial.println(data);
  Serial.println("\nStarting connection to server...");
  if (client.connect(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("POST ");
    client.print(urlpath);
    client.println(" HTTP/1.1");
    
    client.print("Host: ");
    client.println(server);
    
    client.println("Content-Type: application/json");
    
    client.print("Content-Length: ");
    client.println(data.length());
    
    client.println();
    client.print(data);
    //client.println("Connection: close");
    client.println();
  }
  
}



