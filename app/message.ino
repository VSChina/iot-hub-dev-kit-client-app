#include "HTS221Sensor.h"

#if SIMULATED_DATA

void initSensor()
{
    // use SIMULATED_DATA, no sensor need to be inited
}

float readTemperature()
{
    return random(20, 30);
}

float readHumidity()
{
    return random(30, 40);
}

#else

DevI2C *i2c;
HTS221Sensor *sensor;

void initSensor()
{
    i2c = new DevI2C(D14, D15);
    sensor = new HTS221Sensor(*i2c);
    sensor->init(NULL);
}

float readTemperature()
{
    sensor->enable();

    float temperature = 0;
    sensor->getTemperature(&temperature);

    sensor->disable();
    sensor->reset();

    return temperature;
}

float readHumidity()
{
    sensor->enable();

    float humidity = 0;
    sensor->getHumidity(&humidity);

    sensor->disable();
    sensor->reset();

    return humidity;
}

#endif
