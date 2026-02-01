//
// Created by Kok on 2/1/26.
//

#include "tests_handler.h"

void THANDLER_RunTests(THANDLER_CaseTypeDef *TestCases, uint8_t Len) {
    for (int i = 0; i < Len; i++) {
        TestCases[i].Result = TestCases[i].TestFunc();
    }
}
