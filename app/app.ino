#include "config.h"

static int interval = INTERVAL;

void setup()
{
    Serial.begin(9600);
    initSensor();
}

void loop()
{
    Serial.print("Humidity: ");
    Serial.println(readHumidity());

    Serial.print("Temperature: ");
    Serial.println(readTemperature());

    delay(interval);
}
