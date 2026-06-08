#include <QtTest>
#include "TEST_ParseExpression.cpp"
#include "TEST_CompareIndices.cpp"
#include "TEST_DetermineDependency.cpp"

int main(int argc, char *argv[])
{
    int status = 0;
    status |= QTest::qExec(new TEST_ParseExpression, argc, argv);
    status |= QTest::qExec(new TEST_CompareIndices, argc, argv);
    status |= QTest::qExec(new TEST_DetermineDependency, argc, argv);
    return status;
}
