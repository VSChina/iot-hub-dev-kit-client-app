// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "AzureIotHub.h"
#include "Arduino.h"
#include "config.h"
#include "utility.h"
#include "iothub_client_sample_mqtt.h"
#include <ArduinoJson.h>

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
static bool messagePending = false;
static bool messageSending = true;

static IOTHUBMESSAGE_DISPOSITION_RESULT c2dMessageCallback(IOTHUB_MESSAGE_HANDLE message, void *userContextCallback)
{
    const char *buffer;
    size_t size;

    if (IoTHubMessage_GetByteArray(message, (const unsigned char **)&buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        LogInfo("unable to IoTHubMessage_GetByteArray");
        return IOTHUBMESSAGE_REJECTED;
    }
    else
    {
        char *temp = (char *)malloc(size + 1);
        if (temp == NULL)
        {
            LogInfo("Failed to malloc for command");
            return IOTHUBMESSAGE_REJECTED;
        }
        memcpy(temp, buffer, size);
        temp[size] = '\0';
        LogInfo("Receive C2D message: %s", temp);
        blinkLED();
        free(temp);
        return IOTHUBMESSAGE_ACCEPTED;
    }
}

static void twinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, size_t size, void *userContextCallback)
{
    char *temp = (char *)malloc(size + 1);
    for (int i = 0; i < size; i++)
    {
        temp[i] = (char)(payLoad[i]);
    }
    temp[size] = '\0';
    parseTwinMessage(temp);
    free(temp);
}

void iothubInit()
{
    srand((unsigned int)time(NULL));

    if (platform_init() != 0)
    {
        LogInfo("Failed to initialize the platform.");
        return;
    }

    if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(DEVICE_CONNECTION_STRING, MQTT_Protocol)) == NULL)
    {
        LogInfo("iotHubClientHandle is NULL!");
        return;
    }

    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
    {
        LogInfo("failure to set option \"TrustedCerts\"");
        return;
    }

    if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, c2dMessageCallback, NULL) != IOTHUB_CLIENT_OK)
    {
        LogInfo("IoTHubClient_LL_SetMessageCallback FAILED!");
        return;
    }
    if (IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL) != IOTHUB_CLIENT_OK)
    {
        LogInfo("Failed on IoTHubClient_LL_SetDeviceTwinCallback");
        return;
    }
}

static void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
{
    if (IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        LogInfo("Message sent to Azure IoT Hub");
        blinkSendConfirmation();
    }
    else
    {
        LogInfo("Failed to send message to Azure IoT Hub");
    }
    messagePending = false;
}

void iothubSendMessage(const unsigned char *text)
{
    if (messageSending && !messagePending)
    {
        IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(text, strlen((const char *)text));
        if (messageHandle == NULL)
        {
            LogInfo("unable to create a new IoTHubMessage");
            return;
        }

        LogInfo("Sending message: %s", text);
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendConfirmationCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            LogInfo("Failed to hand over the message to IoTHubClient");
            return;
        }
        LogInfo("IoTHubClient accepted the message for delivery");
        messagePending = true;
        IoTHubMessage_Destroy(messageHandle);
        delay(getInterval());
    }
}

void iothubLoop(void)
{
    IoTHubClient_LL_DoWork(iotHubClientHandle);
}
