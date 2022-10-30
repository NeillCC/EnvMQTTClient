#include "DHT.h"

DHT dht;
float currentTemp = 69.0;
float currentHumidity = 69.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.setup(4);
  Serial.println("Starting...");
}

void loop() {
  // put your main code here, to run repeatedly:
    delay(dht.getMinimumSamplingPeriod());
    currentTemp = dht.toFahrenheit(dht.getTemperature());
    currentHumidity = dht.getHumidity();

    Serial.println(currentTemp);
    Serial.println(currentHumidity);
    //Serial.println(dht.getTemperature());
}