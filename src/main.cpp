#include "DHT.h"
#include "WiFi.h"

DHT dht;
int dhtPin = 4;
int sleepSeconds = 5;
float currentTemp = 0.0;
float currentHumidity = 0.0;
char wifiSSID[] = "YoungGuest";
char wifiPassword[] = "aDayattherange69!";

void startSerial (int baudRate) {
  Serial.begin(baudRate);
  Serial.println("");
  Serial.println("");
  Serial.println("Booting...");
}
void startWiFi(char wifiSSID[], char wifiPassword[]) {
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {delay(1);}
  Serial.print("Network Status: ");
  Serial.println(WiFi.status());
  Serial.print("Connected to WiFi network: ");
  Serial.println(WiFi.SSID());
}
void checkDHT(int dhtPin) {
  dht.setup(dhtPin);
  delay(dht.getMinimumSamplingPeriod());
  currentTemp = dht.toFahrenheit(dht.getTemperature());
  currentHumidity = dht.getHumidity();
  //Print temp results for debug
  Serial.print("Current Temperature: ");
  Serial.println(currentTemp);
  Serial.print("Current Humidity: ");
  Serial.println(currentHumidity);
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
  startWiFi(wifiSSID, wifiPassword);
  //Check DHT sensor for temperature and humidity
  checkDHT(dhtPin);
  //Sleep to save power
  startSleep(sleepSeconds);    
}

void loop() {
  // put your main code here, to run repeatedly:
    
}