#include <QtTest>
#include "TEST_ParseExpression.cpp" // Прямой инклуд файла с кодом теста

int main(int argc, char *argv[])
{
    int status = 0;
    status |= QTest::qExec(new TEST_ParseExpression, argc, argv);
    return status;
}
