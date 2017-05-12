#include "AzureIotHub.h"
#include "AZ3166WiFi.h"
#include "config.h"

static int interval = INTERVAL;
static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

void initWifi()
{
    if (WiFi.begin() == WL_CONNECTED)
    {
        IPAddress ip = WiFi.localIP();
        Screen.print(1, ip.get_address());
        Screen.print(2, "Running... \r\n");
    }
    else
    {
        Screen.print(1, "No Wi-Fi\r\n ");
    }
}

void initIoThub() 
{
    if (platform_init() != 0)
    {
        LogInfo("Failed to initialize the platform.");
        while (1);
    }

    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(CONNECTION_STRING, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
        LogInfo("Failed on IoTHubClient_CreateFromConnectionString");
        while (1);
    }

    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
    {
        LogInfo("Failed to set option \"TrustedCerts\"");
        while (1);
    }

    if (IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL) != IOTHUB_CLIENT_OK)
    {
        LogInfo("Failed on IoTHubClient_LL_SetDeviceTwinCallback");
        while (1);
    }
}

void setup()
{
    Serial.begin(9600);

    initWifi();
    initSensor();
    initIoThub();
}

void loop()
{
    Serial.print("Humidity: ");
    Serial.println(readHumidity());

    Serial.print("Temperature: ");
    Serial.println(readTemperature());

    IoTHubClient_LL_DoWork(iotHubClientHandle);

    delay(interval);
}
