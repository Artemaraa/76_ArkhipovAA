#ifndef TEST_DETERMINEDEPENDENCY_H
#define TEST_DETERMINEDEPENDENCY_H

#include <QtTest>

class TEST_DetermineDependency : public QObject
{
    Q_OBJECT

public:
    TEST_DetermineDependency() {}
    ~TEST_DetermineDependency() {}

private slots:
    void TestNotFound();
    void TestScalarDirect();
    void TestArrayDifferentConstants();
    void TestArrayConstantAndVariable();
    void TestArraySameVariables();
    void TestMultidimensionalSame();
    void TestMultidimensionalVariableIndex();
    void TestNumberOperand();
    void TestArrayFirstConstantSecondVariable();
    void TestArrayFirstVariableSecondConstant();
    void TestDifferentVariableName();
    void TestDeep5dSame();
    void TestDeep5dVariable();
    void TestComplexA2iA2j();
};

#endif // TEST_DETERMINEDEPENDENCY_H
