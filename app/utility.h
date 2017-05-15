// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UTILITY_H
#define UTILITY_H

void parseTwinMessage(const char *);
float readHumidity(void);
float readTemperature(void);
void sensorInit(void);
void printScreen(void);
void blinkLED(void);
int getInterval(void);
#endif /* UTILITY_H */