#include <DHT.h>
#include "ESP8266WiFi.h"

//#include <WiFiManager.h>
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>

#include <WiFiUdp.h>
#include <math.h>

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include <SPI.h>

//for LED status
#include <Ticker.h>
Ticker ticker;

//TODO: Move all of these to wifi-configurable parameters
// and save to JSON config in EEPROM
const char* influxHost = "black-pearl";
const int influxPort = 8086;
String location = "Room3";
String measurement = "RoomSense";
const char* ssid = "SunloftAutomata";
const char* password = "laserpointer104";
const char* host = "Room3";

// DHT Sensor Object Here
DHT dht(D4, DHT22);

// Data logging - all time in Milliseconds
int lastDataLog = 0;
const int dataLogInterval = 10000;

void setup() {
  // initialize serial communications
  Serial.begin(115000);
  pinMode(BUILTIN_LED, OUTPUT);
  // Start ticker with AP mode and try to connect
  ticker.attach(0.6, tick);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); 

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  //WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  //wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  //if (!wifiManager.autoConnect(ssid,password)) {
  //  Serial.println("failed to connect and hit timeout");
  //  //reset and try again, or maybe put it to deep sleep
  //  ESP.reset();
  //  delay(1000);
  //}

  //if you get here you have connected to the WiFi
  Serial.println("connected... woot! :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, HIGH);

  dht.begin();

  OTASetup();
  
}

void loop() {
  ArduinoOTA.handle();
  double f = 0;
  double total = 0;
  double numberReadings = 10;

  String line;

  int i = 0;

  while (i<numberReadings) {
    // get only good readings
    f = dht.readTemperature(true);
    if (!isnan(f)) {
      i++;
      total += f;
    } else {
      Serial.println("Info: Reading nan from sensor");
      Serial.print("Reading is: ");
      Serial.println(f);
      delay(100);
    }
  }

  f = total / numberReadings;
  
  if (isnan(f)) {
    // Don't send temperature
    Serial.println("Something wrong with sensor!");
  } else {
    if ((millis()-lastDataLog) > dataLogInterval) {
      line = String(measurement + ",type=temperature,unit=degreesFarenheit,location=" + location + " value=" + String(f));
      WiFiUDP udpClient;
      if (!!!udpClient.beginPacket(influxHost, influxPort)) {
        Serial.println("Connection failed, retrying..."); 
      } else {
        Serial.print("Connected to ");
        Serial.println(influxHost);
        lastDataLog = millis();
        udpClient.print(line);
        udpClient.endPacket();
        blink();
      }
    }  
  }
}

void OTASetup() {
    // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(host);

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"grill");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    tick();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}

void blink()
{
  tick();
  delay(10);
  tick();
}

