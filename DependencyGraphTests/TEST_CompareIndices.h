#ifndef TEST_COMPAREINDICES_H
#define TEST_COMPAREINDICES_H

#include <QtTest>

class TEST_CompareIndices : public QObject
{
    Q_OBJECT

public:
    TEST_CompareIndices() {}
    ~TEST_CompareIndices() {}

private slots:
    void TestBothNull();
    void TestEqualNumbers();
    void TestDifferentNumbers();
    void TestEqualVariables();
    void TestDifferentVariables();
    void TestConstantAndVariable();
    void TestEqual1dArray();
    void TestDifferentConstants1d();
    void TestConstantAndVariable1d();
    void TestEqual2dArray();
    void TestDifferentFirstIndex2d();
    void TestDifferentSecondIndex2d();
    void TestVariableFirstIndex2d();
    void TestVariableSecondIndex2d();
    void TestNullAndVar();
    void TestNullAndNumber();
    void TestNullAndArray();
    void TestDeep5dEqual();
    void TestDeep5dVariableIndex();
    void TestDeep5dDifferentConstant();
    void TestSameTypeOperators();
};

#endif // TEST_COMPAREINDICES_H
