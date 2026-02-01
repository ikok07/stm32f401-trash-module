//
// Created by Kok on 1/23/26.
//

#ifndef POWER_H
#define POWER_H

#include <stdint.h>

uint8_t PWR_CheckDeviceWasSTBY();
void PWR_EnterStandbyMode();
void PWR_EnterSleepMode();

#endif //POWER_H
