#include "AzureIotHub.h"
#include "AZ3166WiFi.h"
#include "config.h"
#include "utility.h"
#include "iothub_client_sample_mqtt.h"

static const char iot_message[] = "{\"topic\":\"iot\"}";
static bool hasWifi = false;

void initWifi()
{
     Screen.print("Azure IoT DevKit\r\n \r\nConnecting...\r\n");

    if (WiFi.begin() == WL_CONNECTED)
    {
        IPAddress ip = WiFi.localIP();
        Screen.print(1, ip.get_address());
        hasWifi = true;
        Screen.print(2, "Running... \r\n");
    }
    else
    {
        Screen.print(1, "No Wi-Fi\r\n ");
    }
}

void setup()
{
    hasWifi = false;
    initWifi();
    if (!hasWifi)
    {
        Serial.println("Please make sure the wifi connected!");
        return;
    }

    Serial.begin(9600);
    sensorInit();
    iothubInit();
}

void loop()
{
    Serial.print("Humidity: ");
    Serial.println(readHumidity());

    Serial.print("Temperature: ");
    Serial.println(readTemperature());

    iothubSendMessage((const unsigned char *)iot_message);
    iothubLoop();
    delay(getInterval());
}
