#ifndef TEST_APPLYACTION_H
#define TEST_APPLYACTION_H

#include <QtTest>

class TEST_ApplyAction : public QObject
{
    Q_OBJECT

public:
    TEST_ApplyAction() {}
    ~TEST_ApplyAction() {}

private slots:
    void TestFirstActionNoDependencies();
    void TestDependencyFromPrevious();
    void TestIncrementVariable();
    void TestArrayDirectDependency();
    void TestArrayGeneralDependency();
    void TestErrorInvalidArrayDimension();
    void TestIncrementUninitializedVariable();
    void TestSelfAssignment();
    void TestArrayDifferentIndices();
    void TestUseUninitializedVariable();
    void TestIncrementExistingVariable();
    void TestReassignArrayElementConstant();
    void TestReassignArrayElementVariable();
    void TestWriteFixedIndexAfterVariable();
    void TestMixedArrayAndScalar();
    void TestSelfArrayAssignment();
    void TestComprehensive();
};

#endif // TEST_APPLYACTION_H
