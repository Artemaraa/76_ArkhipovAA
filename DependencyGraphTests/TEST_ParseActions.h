#ifndef TEST_PARSEACTIONS_H
#define TEST_PARSEACTIONS_H

#include <QtTest>

class TEST_ParseActions : public QObject
{
    Q_OBJECT

public:
    TEST_ParseActions() {}
    ~TEST_ParseActions() {}

private slots:
    void TestSimpleAssignment();
    void TestWithArray();
    void TestWithIncrement();
    void TestEmptyLeftPart();
    void TestEmptyRightPart();
    void TestNoEqualSign();
    void TestMultipleLines();
    void TestEmptyLines();
    void TestNegativeNumber();
    void TestBinaryMinus();
    void TestBinaryDivide();
    void TestUnaryDecrement();
    void TestMultidimensionalLeft();
    void TestMultidimensionalRight();
    void TestComplexExpression();
    void TestInvalidVariableName();
    void TestNumberOutOfRange();
    void TestUnsupportedOperator();
    void TestNotEnoughOperands();
    void TestExtraOperands();
    void TestComprehensive();
    void TestComprehensiveWithErrors();
    void TestBoundaryValues();

    void TestTooManyActions();
};

#endif // TEST_PARSEACTIONS_H
