// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "Arduino.h"
#include "AzureIotHub.h"
#include "config.h"
#include "utility.h"
#include "iothub_client_sample_mqtt.h"

static int receiveContext = 0;
static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
static bool messagePending = false;
static bool messageSending = true;

void executeCommand(const char *command)
{
    if (strstr(command, "blink") != NULL)
    {
        blinkLED();
    }
    else if (strstr(command, "print") != NULL)
    {
        printScreen();
    }
    else if (strstr(command, "stop") != NULL)
    {
        messageSending = false;
    }
}

static IOTHUBMESSAGE_DISPOSITION_RESULT c2dMessageCallback(IOTHUB_MESSAGE_HANDLE message, void *userContextCallback)
{
    const char *buffer;
    size_t size;

    if (IoTHubMessage_GetByteArray(message, (const unsigned char **)&buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        Serial.println("unable to retrieve the message data\r\n");
        return IOTHUBMESSAGE_REJECTED;
    }
    else
    {
        char *temp = (char *)malloc(size + 1);
        if (temp == NULL)
        {
            Serial.println("Failed to execute command");
            return IOTHUBMESSAGE_REJECTED;
        }
        memcpy(temp, buffer, size);
        temp[size] = '\0';
        Serial.print("Receive message: ");
        Serial.println(temp);
        executeCommand(temp);
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
        Serial.println("Failed to initialize the platform.\r\n");
        return;
    }

    if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(DEVICE_CONNECTION_STRING, MQTT_Protocol)) == NULL)
    {
        Serial.println("ERROR: iotHubClientHandle is NULL!\r\n");
        return;
    }

    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
    {
        Serial.println("failure to set option \"TrustedCerts\"\r\n");
        return;
    }

    if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, c2dMessageCallback, &receiveContext) != IOTHUB_CLIENT_OK)
    {
        Serial.println("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!\r\n");
        return;
    }
    if (IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL) != IOTHUB_CLIENT_OK)
    {
        LogInfo("Failed on IoTHubClient_LL_SetDeviceTwinCallback");
        while (1)
            ;
    }
}

static void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
{
    if (IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        Serial.println("Confirm of message received from IoT Hub");
    }
    else
    {
        Serial.println("Failed to send message to Azure IoT Hub");
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
            Serial.println("ERROR: iotHubMessageHandle is NULL!\r\n");
            return;
        }

        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendConfirmationCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            Serial.println("ERROR: IoTHubClient_LL_SendEventAsync......FAILED!\r\n");
            return;
        }
        Serial.print("message ");
        Serial.print((const char *)text);
        Serial.println(" sent to iothub");
        IoTHubMessage_Destroy(messageHandle);
        messagePending = true;
    }
}

void iothubLoop(void)
{
    IoTHubClient_LL_DoWork(iotHubClientHandle);
}
