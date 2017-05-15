// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UTILITY_H
#define UTILITY_H

void parseTwinMessage(const char *);
void readMessage(int, char *);
void sensorInit(void);
void blinkLED(void);
void blinkSendConfirmation(void);
int getInterval(void);

#endif /* UTILITY_H */