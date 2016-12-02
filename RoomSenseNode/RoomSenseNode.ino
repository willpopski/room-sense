// DHT Sensor library for the DHT22 temperature and humidity sensor
#include <DHT.h>

// Networking libraries for the data-link layer (Wi-Fi) and transport and session layers (UDP)
#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// Arduino Over The Air allow you to send code updates over WiFi
// NOTE: If your code update breaks WiFi functionality, then your node will go dark and you will have to physically access the node
#include <ArduinoOTA.h>


// Update the following section to match your installation
// ========================================================

// Set a unique, friendly Node name
String nodeName = "LivingRoom";

// Hostname and UDP port number for the server running influxDB. Can be local or remote, IP Address or DNS name.
const char* influxHost = "black-pearl";
const int influxPort = 8086;

// This is your 
const char* ssid = "RoomSenseWiFi";
const char* password = "********";

// TODO: Move all of these to wifi-configurable parameters and save to JSON config in EEPROM
// =======================================================

// Setup Sensor Pin and Library object
// NOTE: The Wemos DHT22 Shield is hardwired to pin Digital 4. Confirm this setting if you are not using the shield.
const int DHTSensorPin = D4;
DHT dht(DHTSensorPin, DHT22);

// Data logging values - all time in Milliseconds
const int sendInterval = 10000;
int lastSendTimeMillis = 0;

void setup() {

  setupHardware();
  connectWiFi();

  // Solid LED means we're connected
  digitalWrite(BUILTIN_LED, HIGH);

  OTASetup();
  
}

void loop() {
  // Allow the ArduinoOTA library to periodically monitor and handle firmware updates
  ArduinoOTA.handle();

  // During each loop, the node reads DHT sensor temperature. Sometimes the reading is bad and these readings are skipped
  // Variable holding temperature reading, accumulator for average, desired number of readings, and a counter of readings
  double temperatureReading = 0;
  double averageReading = 0;
  double total = 0;
  double desiredReadings = 10;
  int validReadingsCounter = 0;

  // Loop until we have desired number of readings
  while (validReadingsCounter < desiredReadings) {
    // get only good readings
    temperatureReading = dht.readTemperature(true);
    if (!isnan(temperatureReading)) {
      validReadingsCounter++;
      total += temperatureReading;
    } else {
      Serial.println("Info: Reading nan from sensor");
    }
    // Wait for sensor reading to steady
    delay(100);
  }

  // Calculate average reading
  averageReading = total / validReadingsCounter;


  if ((millis()-lastSendTimeMillis) > sendInterval) {
    String line = String("RoomSense,type=temperature,unit=degreesFarenheit,location=" + nodeName + " value=" + String(averageReading));

    // Here is our udpClient which needs a host, port number, and payload before sending
    WiFiUDP udpClient;

    // This attempts to connect and sends data to InfluxDB server
    if (!udpClient.beginPacket(influxHost, influxPort)) {
      Serial.println("Connection failed"); 
    } else {
      Serial.print("Connected to: ");
      Serial.println(influxHost);
      lastSendTimeMillis = millis();

      // Write packet payload
      udpClient.print(line);

      // Finish off packet and send
      udpClient.endPacket();
      blink();
    }  
  }
}

void setupHardware() {
  // Initialize serial communications
  Serial.begin(115000);

  // Configure the pins we will be using
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(DHTSensorPin, INPUT);

  // Connect to DHT sensor
  dht.begin();

}

void connectWiFi() {
  // BUILTIN_LED is a macro for the builtin LED pin. In Wemos this is D3, but using BUILTIN_LED provides code portability
  int ledState = digitalRead(BUILTIN_LED);

  // Set ESP8266 mode to act as a Wi-Fi client and attempt to connect with credentials
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Fast LED blink while waiting to connect
  // NOTE: This approach doesn't have have a retry limit and could drain battery if unattended
   while (WiFi.status() != WL_CONNECTED) {
     digitalWrite(BUILTIN_LED, ledState);
     ledState = !ledState;
     delay(300);
   }

  //If you get here, then you have connected to the WiFi
  Serial.println("connected... woot! :)");
}


void OTASetup() {
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(nodeName.c_str());

  // Basic authentication
  ArduinoOTA.setPassword((const char *)"password");

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

