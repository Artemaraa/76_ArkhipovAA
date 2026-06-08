#include "TEST_ApplyAction.h"
#include "dependencygraph.h"
#include "functions.h"
#include "action.h"
#include "exprnode.h"


// ============================================================
// Тест 1: Первое действие (нет зависимостей)
// ============================================================
void TEST_ApplyAction::TestFirstActionNoDependencies()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a = b c +
    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "a");
    ExprNode* b = new ExprNode(Var, "b");
    ExprNode* c = new ExprNode(Var, "c");
    ExprNode* plus = new ExprNode(Plus);
    plus->leftOperand = b;
    plus->rightOperand = c;
    action1->expression = plus;
    action1->sourceVariables.append(b);
    action1->sourceVariables.append(c);

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 1);
    QVERIFY(varTable.contains("a"));
    QCOMPARE(varTable["a"], action1);

    delete action1;
}

// ============================================================
// Тест 2: Зависимость от предыдущего действия
// ============================================================
void TEST_ApplyAction::TestDependencyFromPrevious()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a = 0
    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "a");
    action1->expression = new ExprNode(Number, "0");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // b = a 1 *
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "b");
    ExprNode* a = new ExprNode(Var, "a");
    ExprNode* one = new ExprNode(Number, "1");
    ExprNode* mult = new ExprNode(Multiply);
    mult->leftOperand = a;
    mult->rightOperand = one;
    action2->expression = mult;
    action2->sourceVariables.append(a);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 2);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // Прямая зависимость (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 3: Инкремент переменной
// ============================================================
void TEST_ApplyAction::TestIncrementVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // x = 5
    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "x");
    action1->expression = new ExprNode(Number, "5");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // y = x ++
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "y");
    ExprNode* x = new ExprNode(Var, "x");
    ExprNode* inc = new ExprNode(Increment);
    inc->leftOperand = x;
    action2->expression = inc;
    action2->sourceVariables.append(x);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QCOMPARE(errors.size(), 0);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // Прямая зависимость (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 4: Массив - прямая зависимость
// ============================================================
void TEST_ApplyAction::TestArrayDirectDependency()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[2] = 4
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Number, "4");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // b = a[2] 2 *
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "b");
    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "2");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;
    ExprNode* two = new ExprNode(Number, "2");
    ExprNode* mult = new ExprNode(Multiply);
    mult->leftOperand = arr2;
    mult->rightOperand = two;
    action2->expression = mult;
    action2->sourceVariables.append(arr2);
    action2->sourceVariables.append(two);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QCOMPARE(errors.size(), 0);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // Прямая зависимость (сплошная), не пунктир
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));
    QVERIFY(!dot.contains("2 -> 1 [style=dashed]"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 5: Массив - общая зависимость
// ============================================================
void TEST_ApplyAction::TestArrayGeneralDependency()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[i] = 4
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Var, "i");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Number, "4");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // b = a[2] 2 *
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "b");
    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "2");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;
    ExprNode* two = new ExprNode(Number, "2");
    ExprNode* mult = new ExprNode(Multiply);
    mult->leftOperand = arr2;
    mult->rightOperand = two;
    action2->expression = mult;
    action2->sourceVariables.append(arr2);
    action2->sourceVariables.append(two);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QCOMPARE(errors.size(), 0);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // Общая зависимость (пунктир)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1 [style=dashed]"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 6: Ошибка - неверная размерность массива
// ============================================================
void TEST_ApplyAction::TestErrorInvalidArrayDimension()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[2] = 4 (1 измерение)
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Number, "4");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // a[i][j] = 5 (2 измерения) - ОШИБКА
    Action* action2 = new Action(2);
    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Var, "i");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Var, "j");
    ExprNode* target2 = new ExprNode(ArrayAccess);
    target2->leftOperand = inner2;
    target2->rightOperand = idx2_2;
    action2->targetRoot = target2;
    action2->expression = new ExprNode(Number, "5");

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == InvalidArrayDimension) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    delete action1;
    delete action2;
}

// ============================================================
// Тест 7: Инкремент неинициализированной переменной
// ============================================================
void TEST_ApplyAction::TestIncrementUninitializedVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // counter = counter ++
    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "counter");
    ExprNode* counter = new ExprNode(Var, "counter");
    ExprNode* inc = new ExprNode(Increment);
    inc->leftOperand = counter;
    action1->expression = inc;
    action1->sourceVariables.append(counter);

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // Нет зависимости (первое появление counter)
    QList<Action*> incoming = graph.getIncoming(action1);
    QVERIFY(incoming.isEmpty());
    QVERIFY(varTable.contains("counter"));
    QCOMPARE(varTable["counter"], action1);

    delete action1;
}

// ============================================================
// Тест 8: Присваивание с использованием самой себя
// ============================================================
void TEST_ApplyAction::TestSelfAssignment()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // x = 5
    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "x");
    action1->expression = new ExprNode(Number, "5");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // x = x 1 +
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "x");
    ExprNode* x = new ExprNode(Var, "x");
    ExprNode* one = new ExprNode(Number, "1");
    ExprNode* plus = new ExprNode(Plus);
    plus->leftOperand = x;
    plus->rightOperand = one;
    action2->expression = plus;
    action2->sourceVariables.append(x);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // Прямая зависимость (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 9: Массив - разные индексы в левой и правой части
// ============================================================
void TEST_ApplyAction::TestArrayDifferentIndices()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[2] = 4
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Number, "4");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // a[3] = a[2] 1 +
    Action* action2 = new Action(2);
    ExprNode* aL = new ExprNode(Var, "a");
    ExprNode* idxL = new ExprNode(Number, "3");
    ExprNode* targetL = new ExprNode(ArrayAccess);
    targetL->leftOperand = aL;
    targetL->rightOperand = idxL;
    action2->targetRoot = targetL;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "2");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;
    ExprNode* one = new ExprNode(Number, "1");
    ExprNode* plus = new ExprNode(Plus);
    plus->leftOperand = arr2;
    plus->rightOperand = one;
    action2->expression = plus;
    action2->sourceVariables.append(arr2);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // a[2] после a[2] - прямая (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 10: Использование неинициализированной переменной
// ============================================================
void TEST_ApplyAction::TestUseUninitializedVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // b = a 1 +
    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "b");
    ExprNode* a = new ExprNode(Var, "a");
    ExprNode* one = new ExprNode(Number, "1");
    ExprNode* plus = new ExprNode(Plus);
    plus->leftOperand = a;
    plus->rightOperand = one;
    action1->expression = plus;
    action1->sourceVariables.append(a);

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // a не определена - зависимости нет
    QList<Action*> incoming = graph.getIncoming(action1);
    QVERIFY(incoming.isEmpty());
    QVERIFY(varTable.contains("b"));

    delete action1;
}

// ============================================================
// Тест 11: Инкремент существующей переменной
// ============================================================
void TEST_ApplyAction::TestIncrementExistingVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a = 5
    Action* action1 = new Action(1);
    action1->targetRoot = new ExprNode(Var, "a");
    action1->expression = new ExprNode(Number, "5");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // b = a ++
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "b");
    ExprNode* a = new ExprNode(Var, "a");
    ExprNode* inc = new ExprNode(Increment);
    inc->leftOperand = a;
    action2->expression = inc;
    action2->sourceVariables.append(a);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // Прямая зависимость (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 12: Повторное присваивание элемента массива (константа)
// ============================================================
void TEST_ApplyAction::TestReassignArrayElementConstant()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[2] = 4
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Number, "4");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // a[2] = 5
    Action* action2 = new Action(2);
    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "2");
    ExprNode* target2 = new ExprNode(ArrayAccess);
    target2->leftOperand = a2;
    target2->rightOperand = idx2;
    action2->targetRoot = target2;
    action2->expression = new ExprNode(Number, "5");

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QCOMPARE(varTable["a"], action2);

    delete action1;
    delete action2;
}

// ============================================================
// Тест 13: Повторное присваивание элемента массива (переменная)
// ============================================================
void TEST_ApplyAction::TestReassignArrayElementVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[i] = i
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Var, "i");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Var, "i");
    ExprNode* iSrc1 = new ExprNode(Var, "i");
    action1->sourceVariables.append(iSrc1);

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // a[i] = i 1 +
    Action* action2 = new Action(2);
    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Var, "i");
    ExprNode* target2 = new ExprNode(ArrayAccess);
    target2->leftOperand = a2;
    target2->rightOperand = idx2;
    action2->targetRoot = target2;
    ExprNode* iSrc2 = new ExprNode(Var, "i");
    ExprNode* one = new ExprNode(Number, "1");
    ExprNode* plus = new ExprNode(Plus);
    plus->leftOperand = iSrc2;
    plus->rightOperand = one;
    action2->expression = plus;
    action2->sourceVariables.append(iSrc2);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QCOMPARE(varTable["a"], action2);

    delete action1;
    delete action2;
}

// ============================================================
// Тест 14: Запись в фиксированный индекс после использования переменной
// ============================================================
void TEST_ApplyAction::TestWriteFixedIndexAfterVariable()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[i] = i
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Var, "i");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Var, "i");
    ExprNode* iSrc = new ExprNode(Var, "i");
    action1->sourceVariables.append(iSrc);

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // c = a[2]
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "c");
    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "2");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;
    action2->expression = arr2;
    action2->sourceVariables.append(arr2);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // a[2] после a[i] - общая (пунктир)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1 [style=dashed]"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 15: Смешанные зависимости (массив + скаляр)
// ============================================================
void TEST_ApplyAction::TestMixedArrayAndScalar()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[2] = 4
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Number, "4");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // b = 5
    Action* action2 = new Action(2);
    action2->targetRoot = new ExprNode(Var, "b");
    action2->expression = new ExprNode(Number, "5");

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    // c = a[2] b +
    Action* action3 = new Action(3);
    action3->targetRoot = new ExprNode(Var, "c");
    ExprNode* a3 = new ExprNode(Var, "a");
    ExprNode* idx3 = new ExprNode(Number, "2");
    ExprNode* arr3 = new ExprNode(ArrayAccess);
    arr3->leftOperand = a3;
    arr3->rightOperand = idx3;
    ExprNode* bSrc = new ExprNode(Var, "b");
    ExprNode* plus = new ExprNode(Plus);
    plus->leftOperand = arr3;
    plus->rightOperand = bSrc;
    action3->expression = plus;
    action3->sourceVariables.append(arr3);
    action3->sourceVariables.append(bSrc);

    graph.applyAction(action3, action3->targetRoot,
                      action3->sourceVariables, action3->modifiedVariables,
                      varTable, errors);

    QList<Action*> incoming = graph.getIncoming(action3);
    QVERIFY(incoming.contains(action1));
    QVERIFY(incoming.contains(action2));

    // Оба ребра прямые (сплошные): a[2] после a[2] и b после b
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("3 -> 1;"));
    QVERIFY(dot.contains("3 -> 2;"));

    delete action1;
    delete action2;
    delete action3;
}

// ============================================================
// Тест 16: Присваивание элемента массива самому себе
// ============================================================
void TEST_ApplyAction::TestSelfArrayAssignment()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // a[2] = 4
    Action* action1 = new Action(1);
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* target1 = new ExprNode(ArrayAccess);
    target1->leftOperand = a1;
    target1->rightOperand = idx1;
    action1->targetRoot = target1;
    action1->expression = new ExprNode(Number, "4");

    graph.applyAction(action1, action1->targetRoot,
                      action1->sourceVariables, action1->modifiedVariables,
                      varTable, errors);

    // a[2] = a[2] 1 +
    Action* action2 = new Action(2);
    ExprNode* aL = new ExprNode(Var, "a");
    ExprNode* idxL = new ExprNode(Number, "2");
    ExprNode* targetL = new ExprNode(ArrayAccess);
    targetL->leftOperand = aL;
    targetL->rightOperand = idxL;
    action2->targetRoot = targetL;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "2");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;
    ExprNode* one = new ExprNode(Number, "1");
    ExprNode* plus = new ExprNode(Plus);
    plus->leftOperand = arr2;
    plus->rightOperand = one;
    action2->expression = plus;
    action2->sourceVariables.append(arr2);

    graph.applyAction(action2, action2->targetRoot,
                      action2->sourceVariables, action2->modifiedVariables,
                      varTable, errors);

    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // a[2] после a[2] - прямая (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));

    delete action1;
    delete action2;
}

// ============================================================
// Тест 17: Комплексный тест
// ============================================================
void TEST_ApplyAction::TestComprehensive()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    // Данные готовит парсер, действия применяем по одному (это и тестируем)
    QList<Action*> actions;
    QStringList lines = {"x = 5", "y = x ++", "z = x y +", "a 2 [] = 10",
                         "b = a 2 [] 2 *", "c = a i [] 1 +", "i = 2", "d = a i [] 1 +"};
    parseActions(lines, actions, errors);

    for (Action* action : actions) {
        graph.applyAction(action, action->targetRoot,
                          action->sourceVariables, action->modifiedVariables,
                          varTable, errors);
    }

    QCOMPARE(errors.size(), 0);

    // Ожидаемые рёбра и типы:
    // 2->1 (D), 3->2 (D), 5->4 (D), 6->4 (G), 8->7 (D), 8->4 (G)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));
    QVERIFY(dot.contains("3 -> 2;"));
    QVERIFY(dot.contains("5 -> 4;"));
    QVERIFY(dot.contains("6 -> 4 [style=dashed]"));
    QVERIFY(dot.contains("8 -> 7;"));
    QVERIFY(dot.contains("8 -> 4 [style=dashed]"));

    // Транзитивного 3->1 быть не должно
    QVERIFY(!dot.contains("3 -> 1"));

    qDeleteAll(actions);
}

