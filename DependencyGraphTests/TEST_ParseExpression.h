#ifndef TEST_PARSEEXPRESSION_H
#define TEST_PARSEEXPRESSION_H

#include <QtTest>

class TEST_ParseExpression : public QObject
{
    Q_OBJECT

public:
    TEST_ParseExpression() {}
    ~TEST_ParseExpression() {}

private slots:
    void TestOperandVariable();
    void TestOperandNumber();
    void TestOperandNegativeNumber();
    void TestBinaryPlus();
    void TestBinaryMultiply();
    void TestUnaryIncrement();
    void TestUnaryDecrement();
    void TestArrayAccess();
    void TestMultidimensionalArray();
    void TestNotEnoughOperandsPlus();
    void TestNotEnoughOperandsIncrement();
    void TestExtraOperands();
    void TestArrayInExpression();
    void TestIncrementArrayElement();
    void TestComplexExpression();
    void TestComplexWithErrors();
    void TestComplexIncrementDecrement();

    void TestEmptyExpression();
    void TestVariableNameTooLong();
    void TestArrayAccessNotEnough();
    void TestInvalidNumberFormat();
    void TestNestingDepthExceeded();
};

#endif // TEST_PARSEEXPRESSION_H
