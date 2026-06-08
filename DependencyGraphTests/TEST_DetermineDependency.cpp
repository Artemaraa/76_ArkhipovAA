#include <QtTest>
#include "dependencygraph.h"
#include "exprnode.h"
#include "functions.h"

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


// ============================================================
// Тест 1: Переменная не найдена
// ============================================================
void TEST_DetermineDependency::TestNotFound()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    ExprNode* varNode = new ExprNode(Var, "x");
    Action* dependencyAction = nullptr;

    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)NoDependency);
    QVERIFY(dependencyAction == nullptr);

    delete varNode;
}

// ============================================================
// Тест 2: Скалярная переменная - найдена
// ============================================================
void TEST_DetermineDependency::TestScalarDirect()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "x");
    varTable["x"] = action1;

    ExprNode* varNode = new ExprNode(Var, "x");
    Action* dependencyAction = nullptr;

    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)Direct);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 3: Массив - разные константы (нет зависимости)
// ============================================================
void TEST_DetermineDependency::TestArrayDifferentConstants()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "3");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = a2;
    varNode->rightOperand = idx2;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)NoDependency);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 4: Массив - константа и переменная (общая зависимость)
// ============================================================
void TEST_DetermineDependency::TestArrayConstantAndVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Var, "i");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = a2;
    varNode->rightOperand = idx2;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)General);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 5: Массив - одинаковые переменные в индексах (прямая)
// ============================================================
void TEST_DetermineDependency::TestArraySameVariables()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Var, "i");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Var, "i");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = a2;
    varNode->rightOperand = idx2;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)Direct);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 6: Многомерный массив - все индексы совпадают
// ============================================================
void TEST_DetermineDependency::TestMultidimensionalSame()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = inner1;
    target1->rightOperand = idx1_2;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Number, "2");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "3");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = inner2;
    varNode->rightOperand = idx2_2;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)Direct);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 7: Многомерный массив - один индекс переменная
// ============================================================
void TEST_DetermineDependency::TestMultidimensionalVariableIndex()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = inner1;
    target1->rightOperand = idx1_2;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Var, "i");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "3");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = inner2;
    varNode->rightOperand = idx2_2;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)General);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 8: Операнд - число (не переменная)
// ============================================================
void TEST_DetermineDependency::TestNumberOperand()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    ExprNode* numNode = new ExprNode(Number, "5");
    Action* dependencyAction = nullptr;

    DependencyType result = graph.determineDependency(numNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)NoDependency);
    QVERIFY(dependencyAction == nullptr);

    delete numNode;
}

// ============================================================
// Тест 9: Массив - первый индекс константа, второй переменная
// ============================================================
void TEST_DetermineDependency::TestArrayFirstConstantSecondVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = inner1;
    target1->rightOperand = idx1_2;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Number, "2");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Var, "i");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = inner2;
    varNode->rightOperand = idx2_2;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)General);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 10: Массив - первый индекс переменная, второй константа
// ============================================================
void TEST_DetermineDependency::TestArrayFirstVariableSecondConstant()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = inner1;
    target1->rightOperand = idx1_2;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Var, "i");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "3");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = inner2;
    varNode->rightOperand = idx2_2;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)General);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 11: Другое имя переменной
// ============================================================
void TEST_DetermineDependency::TestDifferentVariableName()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "b");
    varTable["b"] = action1;

    ExprNode* varNode = new ExprNode(Var, "a");
    Action* dependencyAction = nullptr;

    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)NoDependency);
    QVERIFY(dependencyAction == nullptr);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 12: Глубокий массив (5 уровней) - все индексы совпадают
// ============================================================
void TEST_DetermineDependency::TestDeep5dSame()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* i1 = new ExprNode(Number, "1");
    ExprNode* l1 = new ExprNode(ArrayAccess);
    l1->leftOperand = a1;
    l1->rightOperand = i1;
    ExprNode* i2 = new ExprNode(Number, "2");
    ExprNode* l2 = new ExprNode(ArrayAccess);
    l2->leftOperand = l1;
    l2->rightOperand = i2;
    ExprNode* i3 = new ExprNode(Number, "3");
    ExprNode* l3 = new ExprNode(ArrayAccess);
    l3->leftOperand = l2;
    l3->rightOperand = i3;
    ExprNode* i4 = new ExprNode(Number, "4");
    ExprNode* l4 = new ExprNode(ArrayAccess);
    l4->leftOperand = l3;
    l4->rightOperand = i4;
    ExprNode* i5 = new ExprNode(Number, "5");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = l4;
    target1->rightOperand = i5;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* j1 = new ExprNode(Number, "1");
    ExprNode* m1 = new ExprNode(ArrayAccess);
    m1->leftOperand = a2;
    m1->rightOperand = j1;
    ExprNode* j2 = new ExprNode(Number, "2");
    ExprNode* m2 = new ExprNode(ArrayAccess);
    m2->leftOperand = m1;
    m2->rightOperand = j2;
    ExprNode* j3 = new ExprNode(Number, "3");
    ExprNode* m3 = new ExprNode(ArrayAccess);
    m3->leftOperand = m2;
    m3->rightOperand = j3;
    ExprNode* j4 = new ExprNode(Number, "4");
    ExprNode* m4 = new ExprNode(ArrayAccess);
    m4->leftOperand = m3;
    m4->rightOperand = j4;
    ExprNode* j5 = new ExprNode(Number, "5");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = m4;
    varNode->rightOperand = j5;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)Direct);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 13: Глубокий массив - один индекс переменная
// ============================================================
void TEST_DetermineDependency::TestDeep5dVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* i1 = new ExprNode(Number, "1");
    ExprNode* l1 = new ExprNode(ArrayAccess);
    l1->leftOperand = a1;
    l1->rightOperand = i1;
    ExprNode* i2 = new ExprNode(Number, "2");
    ExprNode* l2 = new ExprNode(ArrayAccess);
    l2->leftOperand = l1;
    l2->rightOperand = i2;
    ExprNode* i3 = new ExprNode(Number, "3");
    ExprNode* l3 = new ExprNode(ArrayAccess);
    l3->leftOperand = l2;
    l3->rightOperand = i3;
    ExprNode* i4 = new ExprNode(Number, "4");
    ExprNode* l4 = new ExprNode(ArrayAccess);
    l4->leftOperand = l3;
    l4->rightOperand = i4;
    ExprNode* i5 = new ExprNode(Number, "5");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = l4;
    target1->rightOperand = i5;
    action1->targetRoot = target1;
    varTable["a"] = action1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* j1 = new ExprNode(Number, "1");
    ExprNode* m1 = new ExprNode(ArrayAccess);
    m1->leftOperand = a2;
    m1->rightOperand = j1;
    ExprNode* j2 = new ExprNode(Number, "2");
    ExprNode* m2 = new ExprNode(ArrayAccess);
    m2->leftOperand = m1;
    m2->rightOperand = j2;
    ExprNode* j3 = new ExprNode(Var, "i");
    ExprNode* m3 = new ExprNode(ArrayAccess);
    m3->leftOperand = m2;
    m3->rightOperand = j3;
    ExprNode* j4 = new ExprNode(Number, "4");
    ExprNode* m4 = new ExprNode(ArrayAccess);
    m4->leftOperand = m3;
    m4->rightOperand = j4;
    ExprNode* j5 = new ExprNode(Number, "5");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = m4;
    varNode->rightOperand = j5;

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)General);
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

// ============================================================
// Тест 14: Комплексный - a[2][i] vs a[2][j] (разные переменные во 2-м индексе)
// ============================================================
void TEST_DetermineDependency::TestComplexA2iA2j()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;

    // varTable: {"a" -> Action(1)} с targetRoot = a[2][i]
    Action* action1 = new Action(1);
    ExprNode* aBase  = new ExprNode(Var, "a");
    ExprNode* idx2   = new ExprNode(Number, "2");
    ExprNode* inner  = new ExprNode(ArrayAccess);
    inner->leftOperand = aBase;  inner->rightOperand = idx2;       // a[2]
    ExprNode* idxI   = new ExprNode(Var, "i");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = inner; target1->rightOperand = idxI;    // a[2][i]
    action1->targetRoot = target1;
    varTable["a"] = action1;

    // varNode = a[2][j]
    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2b = new ExprNode(Number, "2");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2; inner2->rightOperand = idx2b;        // a[2]
    ExprNode* idxJ = new ExprNode(Var, "j");
    ExprNode* varNode = new ExprNode(ArrayAccess);
    varNode->leftOperand = inner2; varNode->rightOperand = idxJ;   // a[2][j]

    Action* dependencyAction = nullptr;
    DependencyType result = graph.determineDependency(varNode, varTable, dependencyAction);

    QCOMPARE((int)result, (int)General);   // i и j - разные переменные
    QVERIFY(dependencyAction == action1);

    delete varNode;
    delete action1;
}

#include "TEST_DetermineDependency.moc"
