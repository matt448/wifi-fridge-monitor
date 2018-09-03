#include <SPI.h>
#include <WiFi101.h>

#include "secrets.h"
// Copy example_secrets.h to secrets.h
// Then enter values for all fields in secrets.h
char ssid[] = SECRET_SSID;        // wifi ssid
char pass[] = SECRET_PASS;        // wifi network password (use for WPA, or use as key for WEP)
char server[] = SERVER_HOST_NAME; //Host name ex: www.myserver.foo
char urlpath[] = URL_PATH;        //Path to api endpoint ex: /api/myfridge

int status = WL_IDLE_STATUS;      // the WiFi radio's status

WiFiSSLClient client;


void setup() {
  WiFi.setPins(8,7,4,2);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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

  printWiFiStatus();
  connectToServer();
 
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(5000);
  //printWifiRSSI();
  
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printWifiRSSI() {
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String rssiDataString() {
  String rssiString = String(WiFi.RSSI());
  String data = String("\"rssi\": \"" + rssiString + "\"");

  Serial.print("RSSI DATA: ");
  Serial.println(data);
  return data;
}

String buildDataString() {
  String rssiData = rssiDataString();
  String deviceidData = String("\"device-id\": \"123XYZ\"");
  String fridgeTempData = String("\"fridge-temp\": \"38\"");
  String freezerTempData = String("\"freezer-temp\": \"18\"");
  String data = String("{" + deviceidData + "," + rssiData + "," + fridgeTempData + "," + freezerTempData + "}");
  return data;
}

void connectToServer() {
  String data = buildDataString();
  Serial.print("\nDATA: ");
  Serial.println(data);
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
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
