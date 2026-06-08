#include <QtTest>
#include "TEST_ParseExpression.h"
#include "TEST_CompareIndices.h"
#include "TEST_DetermineDependency.h"
#include "TEST_ParseActions.h"
#include "TEST_ApplyAction.h"
#include "TEST_BuildGraph.h"

int main(int argc, char *argv[])
{
    int status = 0;
    status |= QTest::qExec(new TEST_ParseExpression, argc, argv);
    status |= QTest::qExec(new TEST_CompareIndices, argc, argv);
    status |= QTest::qExec(new TEST_DetermineDependency, argc, argv);
    status |= QTest::qExec(new TEST_ParseActions, argc, argv);
    status |= QTest::qExec(new TEST_ApplyAction, argc, argv);
    status |= QTest::qExec(new TEST_BuildGraph, argc, argv);
    return status;
}
