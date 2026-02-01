//
// Created by Kok on 2/1/26.
//

#include "tests.h"

#include <stdio.h>

#include "app_state.h"
#include "log.h"

#include "servo.h"

THANDLER_ResultTypeDef test_servo();
THANDLER_ResultTypeDef test_sensor();

static THANDLER_CaseTypeDef gTestCases[] = {
    {.Name = "Test_Servo_MG955", .TestFunc = test_servo},
    // {.Name = "Test_Sensor_VL53L1X", .TestFunc = test_sensor},
};

void TESTS_GetCases(THANDLER_CaseTypeDef **Cases, uint16_t *Len) {
    *Cases = gTestCases;
    *Len = sizeof(gTestCases) / sizeof(gTestCases[0]);
}

THANDLER_ResultTypeDef TESTS_Run() {
    LOGGER_Log(LOGGER_LEVEL_INFO, "------ START TESTING PERIPHERALS ------");

    THANDLER_RunTests(gTestCases, sizeof(gTestCases) / sizeof(gTestCases[0]));

    THANDLER_ResultTypeDef overall_result = TEST_RESULT_PASS;

    for (int i = 0; i < sizeof(gTestCases) / sizeof(gTestCases[0]); i++) {
        char msg[100];
        if (gTestCases[i].Result != TEST_RESULT_PASS) {
            overall_result = TEST_RESULT_FAIL;
            sprintf(msg, "%d - Test '%s' didn't pass!", i + 1, gTestCases[i].Name);
            LOGGER_Log(LOGGER_LEVEL_ERROR, msg);
        } else {
            sprintf(msg, "%d - Test '%s' passed successfully!", i + 1, gTestCases[i].Name);
            LOGGER_Log(LOGGER_LEVEL_INFO, msg);
        }
    }

    LOGGER_Log(LOGGER_LEVEL_INFO, "------ FINISHED TESTING PERIPHERALS ------");
    return overall_result;
}

THANDLER_ResultTypeDef test_servo() {
    // Set to 180 Deg and back to 0
    THANDLER_ASSERT(SERVO_SetPosition(app_state.pServoHandle, 180) == SERVO_OK);
    THANDLER_ASSERT(SERVO_SetPosition(app_state.pServoHandle, 0) == SERVO_OK);

    return TEST_RESULT_PASS;
}

THANDLER_ResultTypeDef test_sensor() {
    uint16_t value;

    // Make sure data can be read from the sensor
    THANDLER_ASSERT(VL53L1X_Read(app_state.pSensorHandle, &value) == VL53L1X_ERROR_OK);

    return TEST_RESULT_PASS;
}