#ifndef TEST_BUILDGRAPH_H
#define TEST_BUILDGRAPH_H

#include <QtTest>

class TEST_BuildGraph : public QObject
{
    Q_OBJECT

public:
    TEST_BuildGraph() {}
    ~TEST_BuildGraph() {}

private slots:
    void TestSingleAction();
    void TestTwoActionsWithDependency();
    void TestThreeActions();
    void TestFullExampleFromSpec();
    void TestArrayDirectAndGeneral();
    void TestIncrement();
    void TestVariableRedefinition();
    void TestNoTransitiveEdges();
    void TestErrorInvalidArrayDimension();
    void TestEmptyInputFile();
    void TestMixedArraysIncrementRedefinition();
    void TestComprehensive();
};

#endif // TEST_BUILDGRAPH_H
