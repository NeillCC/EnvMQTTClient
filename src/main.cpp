#include "DHT.h"
#include "WiFi.h"
#include <PubSubClient.h>


#pragma region User Settings
//WiFi
char wifiSSID[] = "iot7";                   //WiFi Name
char wifiPassword[] = "aDayattherange69!";  //WiFi Password
char wifiCustomHostname[] = "officeTemp";  //Name to give dhcp server. This just needs be unique to your MQTT broker. Defaults to mac address
bool useMacForHostname = true;              //Whether to use MAC Address as hostname. Must change to false for wifiCustomHostname to matter
//MQTT Settings
char mqttServer[] = "10.0.1.11";            //DNS or IP for MQTT Broker
int  mqttPort = 2883;                       //Port for MQTT Broker. 1883 is standard
std::string mqttPrefix = "homeassistant/sensor/";                //Prefix for MQTT name. Useful if you want to add homeassistant/
//Power Settings
//TODO sleepSeconds can be overriden by declaring a cycleTime topic on your MQTT Broker
int sleepSeconds = 60;                       //How often to boot and update MQTT Broker. Higher number means less frequent updates, but better battery life. Default 60
#pragma endregion


#pragma region declaration                  //Global Variables
DHT dht;
WiFiClient wifiClient;
PubSubClient mqttClient;
#pragma endregion
#pragma region DHT Variables
int dhtPin = 4;                             //DHT connection pin
float currentTemperatureFarnehit;
float currentTemperatureCelsius;
float currentHumidity;
char currentTemperatureFarnehitString[7];
char currentTemperatureCelsiusString[7];
char currentHumidityString[7];
#pragma endregion
void startSerial (int baudRate) {
  Serial.begin(baudRate);
  Serial.println("");
  Serial.println("");
  Serial.println("Booting...");
}
std::string startWiFi(char wifiSSID[], char wifiPassword[], bool useMacForHostname = true, std::string hostname = wifiCustomHostname) {  
  Serial.println("Attempting WiFi connection...");
  int customName = strlen(wifiCustomHostname);
  Serial.println(customName);
  switch (customName != 0) {
    case true:
      WiFi.setHostname(hostname.c_str());
      break;
    default:
      WiFi.setHostname(WiFi.macAddress().c_str());
      break;
  } 
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {delay(1);}
  Serial.println();
  Serial.print("Network Status: ");
  Serial.println(WiFi.status());
  Serial.print("Connected to WiFi network: ");
  Serial.println(WiFi.SSID());
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());
  Serial.print("IPv4: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Can resolve and reach google? [bool]: ");
  Serial.println(wifiClient.connect("google.com", 80));
  wifiClient.stop();
  Serial.println();
  
  return WiFi.getHostname();
  
}
void checkDHT(int dhtPin) {
  dht.setup(dhtPin);
  delay(dht.getMinimumSamplingPeriod());
  //Get readings
  currentTemperatureCelsius = dht.getTemperature();
  currentTemperatureFarnehit = dht.toFahrenheit(currentTemperatureCelsius);
  currentHumidity = dht.getHumidity();
  //Make string versions
  dtostrf(currentTemperatureFarnehit, 5, 2, currentTemperatureFarnehitString);
  dtostrf(currentTemperatureCelsius, 5, 2, currentTemperatureCelsiusString);
  dtostrf(currentHumidity, 5, 2, currentHumidityString);
  //Print temp results for debug
  Serial.print("Current Temperature F: ");
  Serial.println(currentTemperatureFarnehit);
  Serial.print("Current Temperature C: ");
  Serial.println(currentTemperatureCelsius);
  Serial.print("Current Humidity %: ");
  Serial.println(currentHumidity);
}
void startMQTT(WiFiClient wifiClient, PubSubClient mqttClient, std::string hostname, boolean retain = true, std::string mqttPrefix = mqttPrefix, char mqttUsername[] = NULL, char mqttPassword[] = NULL) {
  //Supports MQTT auth by disabled by default
  //wifiClient.connect(mqttServer, mqttPort);
  char wifiHostnameChar[hostname.length() + 1];
  strcpy(wifiHostnameChar, hostname.c_str());
  mqttClient.setClient(wifiClient);
  mqttClient.setServer(mqttServer, mqttPort);
  char blankChar[] = "/";

  //Try to connect 10 times
  int i = 0;
  while (!mqttClient.connected() and i < 10) {
    i++;
    Serial.print("MQTT connection attempt to ");
    Serial.print(mqttServer);
    Serial.print(" number ");
    Serial.println(i);
    
    // Attempt to connect
    if (mqttClient.connect(wifiHostnameChar, mqttUsername, mqttPassword)) {
      Serial.print("MQTT connection to ");
      Serial.print(mqttServer);
      Serial.println(" succesful!");
      //Build MQTT Topic char[]
      std::string tempFarenheitTxt = "/Farenheit";
      std::string tempCelsiusText = "/Celsius";
      std::string humidTxt = "/HumidityPercent";
      int l1 = mqttPrefix.length() + hostname.length() + tempFarenheitTxt.length() + 1;
      int l2 = mqttPrefix.length() + hostname.length() + tempCelsiusText.length() + 1;
      int l3 = mqttPrefix.length() + hostname.length() + humidTxt.length() + 1;
      char farenheitTopic[l1 + 1];
      char celsiusTopic[l2 + 1];
      char humidityTopic[l3 + 1];
      std::string tempFarenheit = mqttPrefix + wifiHostnameChar + tempFarenheitTxt;
      std::string tempC = mqttPrefix + wifiHostnameChar + tempCelsiusText;
      std::string humid = mqttPrefix + wifiHostnameChar + humidTxt;
      strcpy(farenheitTopic, tempFarenheit.c_str());
      strcpy(celsiusTopic, tempC.c_str());
      strcpy(humidityTopic, humid.c_str());
      //Publish to MQTT Broker
      /* int lenF = currentTemperatureFarnehitString.length() + 1;
      int lenC = currentTemperatureCelsiusString.length() + 1;
      int lenH = currentHumidityString.length() + 1;
      char msgF[lenF];
      char msgC[lenC];
      char msgH[lenH];
      strcpy(msgF, currentTemperatureFarnehitString.c_str());
      strcpy(msgC, currentTemperatureCelsiusString.c_str());
      strcpy(msgH, currentHumidityString.c_str()); */
      mqttClient.publish(farenheitTopic, currentTemperatureFarnehitString, retain);
      mqttClient.publish(celsiusTopic, currentTemperatureCelsiusString, retain);
      mqttClient.publish(humidityTopic, currentHumidityString, retain);
      //Serial.println(.c_str());
      Serial.println("Published MQTT data"); 
      //TODO override sleepSeconds with data from this subscription if data is returned
      //mqttClient.subscribe("cycleTime");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 1 second...");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}
void startSleep(int sleepSeconds) {
  int sleepMilliseconds = sleepSeconds * 1000000;
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_enable_timer_wakeup(sleepMilliseconds);
  WiFi.disconnect();
  Serial.print("Sleeping for ");
  Serial.print(sleepSeconds);
  Serial.println(" seconds");
  esp_deep_sleep_start();
}
void setup() {
  //Start serial connection
  startSerial(9600);
  //Connect to WiFi
  std::string hostname = startWiFi(wifiSSID, wifiPassword);
  //Check DHT sensor for temperature and humidity
  checkDHT(dhtPin);
  //Publish DHT data to MQTT broker
  startMQTT(wifiClient, mqttClient, hostname);
  //Sleep to save power
  startSleep(sleepSeconds);    
}

void loop() {
  // put your main code here, to run repeatedly:
    
}