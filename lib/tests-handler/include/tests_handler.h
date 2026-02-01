//
// Created by Kok on 2/1/26.
//

#ifndef TESTS_HANDLER_H
#define TESTS_HANDLER_H

#include <stdint.h>

#define THANDLER_ASSERT(Cond)       do { if (!(Cond)) return TEST_RESULT_FAIL; } while(0)

typedef enum {
    TEST_RESULT_PASS,
    TEST_RESULT_FAIL
} THANDLER_ResultTypeDef;

typedef struct {
    char *Name;
    THANDLER_ResultTypeDef (*TestFunc)();
    THANDLER_ResultTypeDef Result;
} THANDLER_CaseTypeDef;

void THANDLER_RunTests(THANDLER_CaseTypeDef *TestCases, uint8_t Len);

#endif //TESTS_HANDLER_H
