#include "TEST_CompareIndices.h"
#include "functions.h"
#include "exprnode.h"

// ============================================================
// Тест 1: Оба пустые узла
// ============================================================
void TEST_CompareIndices::TestBothNull()
{
    DependencyType result = compareIndices(nullptr, nullptr);
    QCOMPARE((int)result, (int)Direct);
}

// ============================================================
// Тест 2: Оба - константы, равны
// ============================================================
void TEST_CompareIndices::TestEqualNumbers()
{
    ExprNode* n1 = new ExprNode(Number, "2");
    ExprNode* n2 = new ExprNode(Number, "2");
    DependencyType result = compareIndices(n1, n2);
    QCOMPARE((int)result, (int)Direct);
    delete n1;
    delete n2;
}

// ============================================================
// Тест 3: Оба - константы, разные
// ============================================================
void TEST_CompareIndices::TestDifferentNumbers()
{
    ExprNode* n1 = new ExprNode(Number, "6");
    ExprNode* n2 = new ExprNode(Number, "7");
    DependencyType result = compareIndices(n1, n2);
    QCOMPARE((int)result, (int)NoDependency);
    delete n1;
    delete n2;
}

// ============================================================
// Тест 4: Оба - переменные, равны
// ============================================================
void TEST_CompareIndices::TestEqualVariables()
{
    ExprNode* n1 = new ExprNode(Var, "i");
    ExprNode* n2 = new ExprNode(Var, "i");
    DependencyType result = compareIndices(n1, n2);
    QCOMPARE((int)result, (int)Direct);
    delete n1;
    delete n2;
}

// ============================================================
// Тест 5: Оба - переменные, разные
// ============================================================
void TEST_CompareIndices::TestDifferentVariables()
{
    ExprNode* n1 = new ExprNode(Var, "i");
    ExprNode* n2 = new ExprNode(Var, "j");
    DependencyType result = compareIndices(n1, n2);
    QCOMPARE((int)result, (int)General);
    delete n1;
    delete n2;
}

// ============================================================
// Тест 6: Константа и переменная
// ============================================================
void TEST_CompareIndices::TestConstantAndVariable()
{
    ExprNode* n1 = new ExprNode(Number, "2");
    ExprNode* n2 = new ExprNode(Var, "i");
    DependencyType result = compareIndices(n1, n2);
    QCOMPARE((int)result, (int)General);
    delete n1;
    delete n2;
}

// ============================================================
// Тест 7: Одномерные массивы совпадают
// ============================================================
void TEST_CompareIndices::TestEqual1dArray()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = a1;
    arr1->rightOperand = idx1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "2");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)Direct);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 8: Одномерные - разные константы
// ============================================================
void TEST_CompareIndices::TestDifferentConstants1d()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = a1;
    arr1->rightOperand = idx1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Number, "3");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)NoDependency);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 9: Одномерные - константа и переменная
// ============================================================
void TEST_CompareIndices::TestConstantAndVariable1d()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1 = new ExprNode(Number, "2");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = a1;
    arr1->rightOperand = idx1;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2 = new ExprNode(Var, "i");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = a2;
    arr2->rightOperand = idx2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)General);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 10: Двумерные массивы совпадают
// ============================================================
void TEST_CompareIndices::TestEqual2dArray()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = inner1;
    arr1->rightOperand = idx1_2;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Number, "2");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "3");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = inner2;
    arr2->rightOperand = idx2_2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)Direct);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 11: Двумерные - первый индекс разный
// ============================================================
void TEST_CompareIndices::TestDifferentFirstIndex2d()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = inner1;
    arr1->rightOperand = idx1_2;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Number, "4");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "3");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = inner2;
    arr2->rightOperand = idx2_2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)NoDependency);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 12: Двумерные - второй индекс разный
// ============================================================
void TEST_CompareIndices::TestDifferentSecondIndex2d()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = inner1;
    arr1->rightOperand = idx1_2;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Number, "2");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "5");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = inner2;
    arr2->rightOperand = idx2_2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)NoDependency);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 13: Двумерные - первый индекс переменная
// ============================================================
void TEST_CompareIndices::TestVariableFirstIndex2d()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Var, "i");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Number, "3");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = inner1;
    arr1->rightOperand = idx1_2;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Number, "2");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "3");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = inner2;
    arr2->rightOperand = idx2_2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)General);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 14: Двумерные - второй индекс переменная
// ============================================================
void TEST_CompareIndices::TestVariableSecondIndex2d()
{
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* idx1_1 = new ExprNode(Number, "2");
    ExprNode* inner1 = new ExprNode(ArrayAccess);
    inner1->leftOperand = a1;
    inner1->rightOperand = idx1_1;
    ExprNode* idx1_2 = new ExprNode(Var, "i");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = inner1;
    arr1->rightOperand = idx1_2;

    ExprNode* a2 = new ExprNode(Var, "a");
    ExprNode* idx2_1 = new ExprNode(Number, "2");
    ExprNode* inner2 = new ExprNode(ArrayAccess);
    inner2->leftOperand = a2;
    inner2->rightOperand = idx2_1;
    ExprNode* idx2_2 = new ExprNode(Number, "3");
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = inner2;
    arr2->rightOperand = idx2_2;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)General);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 15: Один узел пуст, другой - Var
// ============================================================
void TEST_CompareIndices::TestNullAndVar()
{
    ExprNode* node = new ExprNode(Var, "a");
    DependencyType result = compareIndices(nullptr, node);
    QCOMPARE((int)result, (int)General);
    delete node;
}

// ============================================================
// Тест 16: Один узел пуст, другой - Number
// ============================================================
void TEST_CompareIndices::TestNullAndNumber()
{
    ExprNode* node = new ExprNode(Number, "5");
    DependencyType result = compareIndices(nullptr, node);
    QCOMPARE((int)result, (int)General);
    delete node;
}

// ============================================================
// Тест 17: Один узел пуст, другой - ArrayAccess
// ============================================================
void TEST_CompareIndices::TestNullAndArray()
{
    ExprNode* a = new ExprNode(Var, "a");
    ExprNode* idx = new ExprNode(Number, "2");
    ExprNode* arr = new ExprNode(ArrayAccess);
    arr->leftOperand = a;
    arr->rightOperand = idx;
    DependencyType result = compareIndices(nullptr, arr);
    QCOMPARE((int)result, (int)General);
    delete arr;
}

// ============================================================
// Тест 18: Глубокий многомерный массив (5 уровней) - все индексы совпадают
// ============================================================
void TEST_CompareIndices::TestDeep5dEqual()
{
    // Создание a[1][2][3][4][5] для первого дерева
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
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = l4;
    arr1->rightOperand = i5;

    // Создание такой же структуры для второго дерева
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
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = m4;
    arr2->rightOperand = j5;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)Direct);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 19: Глубокий массив (5 уровней) - один индекс переменная
// ============================================================
void TEST_CompareIndices::TestDeep5dVariableIndex()
{
    // Создание a[1][2][i][4][5] для первого дерева
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* i1 = new ExprNode(Number, "1");
    ExprNode* l1 = new ExprNode(ArrayAccess);
    l1->leftOperand = a1;
    l1->rightOperand = i1;
    ExprNode* i2 = new ExprNode(Number, "2");
    ExprNode* l2 = new ExprNode(ArrayAccess);
    l2->leftOperand = l1;
    l2->rightOperand = i2;
    ExprNode* i3 = new ExprNode(Var, "i");
    ExprNode* l3 = new ExprNode(ArrayAccess);
    l3->leftOperand = l2;
    l3->rightOperand = i3;
    ExprNode* i4 = new ExprNode(Number, "4");
    ExprNode* l4 = new ExprNode(ArrayAccess);
    l4->leftOperand = l3;
    l4->rightOperand = i4;
    ExprNode* i5 = new ExprNode(Number, "5");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = l4;
    arr1->rightOperand = i5;

    // Создание a[1][2][3][4][5] для второго дерева
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
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = m4;
    arr2->rightOperand = j5;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)General);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 20: Глубокий массив (5 уровней) - один индекс другая константа
// ============================================================
void TEST_CompareIndices::TestDeep5dDifferentConstant()
{
    // Создание a[1][2][6][4][5] для первого дерева
    ExprNode* a1 = new ExprNode(Var, "a");
    ExprNode* i1 = new ExprNode(Number, "1");
    ExprNode* l1 = new ExprNode(ArrayAccess);
    l1->leftOperand = a1;
    l1->rightOperand = i1;
    ExprNode* i2 = new ExprNode(Number, "2");
    ExprNode* l2 = new ExprNode(ArrayAccess);
    l2->leftOperand = l1;
    l2->rightOperand = i2;
    ExprNode* i3 = new ExprNode(Number, "6");
    ExprNode* l3 = new ExprNode(ArrayAccess);
    l3->leftOperand = l2;
    l3->rightOperand = i3;
    ExprNode* i4 = new ExprNode(Number, "4");
    ExprNode* l4 = new ExprNode(ArrayAccess);
    l4->leftOperand = l3;
    l4->rightOperand = i4;
    ExprNode* i5 = new ExprNode(Number, "5");
    ExprNode* arr1 = new ExprNode(ArrayAccess);
    arr1->leftOperand = l4;
    arr1->rightOperand = i5;

    // Создание a[1][2][3][4][5] для второго дерева
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
    ExprNode* arr2 = new ExprNode(ArrayAccess);
    arr2->leftOperand = m4;
    arr2->rightOperand = j5;

    DependencyType result = compareIndices(arr1, arr2);
    QCOMPARE((int)result, (int)NoDependency);
    delete arr1;
    delete arr2;
}

// ============================================================
// Тест 21: Узлы одного типа-оператора (не Var/Number/ArrayAccess)
// Покрывает завершающую ветку else -> General
// ============================================================
void TEST_CompareIndices::TestSameTypeOperators()
{
    // Два узла одного типа Plus: типы равны, но это не Var/Number/ArrayAccess
    ExprNode* node1 = new ExprNode(Plus);
    ExprNode* node2 = new ExprNode(Plus);

    DependencyType result = compareIndices(node1, node2);

    // Должна сработать завершающая ветка else -> General
    QCOMPARE((int)result, (int)General);

    delete node1;
    delete node2;
}
