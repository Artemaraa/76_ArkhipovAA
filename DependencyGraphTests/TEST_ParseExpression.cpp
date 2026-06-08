#include "TEST_ParseExpression.h"
#include "functions.h"
#include "exprnode.h"


// ============================================================
// Тест 1: Операнд - переменная
// Вход: "b"
// Ожидается: узел Var("b"), sources = [b], modified = пуст
// ============================================================
void TEST_ParseExpression::TestOperandVariable()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("b", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Var);
    QCOMPARE(root->value, QString("b"));
    QCOMPARE(sources.size(), 1);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 2: Операнд - число
// Вход: "42"
// Ожидается: узел Number("42"), sources = пуст, modified = пуст
// ============================================================
void TEST_ParseExpression::TestOperandNumber()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("42", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Number);
    QCOMPARE(root->value, QString("42"));
    QCOMPARE(sources.size(), 0);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 3: Операнд - отрицательное число
// Вход: "-5"
// Ожидается: узел Number("-5"), sources = пуст, modified = пуст
// ============================================================
void TEST_ParseExpression::TestOperandNegativeNumber()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("-5", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Number);
    QCOMPARE(root->value, QString("-5"));
    QCOMPARE(sources.size(), 0);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 4: Бинарный оператор +
// Вход: "b c +"
// Ожидается: узел Plus с левым Var("b") и правым Var("c")
// ============================================================
void TEST_ParseExpression::TestBinaryPlus()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("b c +", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Plus);
    QVERIFY(root->leftOperand != nullptr);
    QVERIFY(root->rightOperand != nullptr);
    QCOMPARE((int)root->leftOperand->type, (int)Var);
    QCOMPARE(root->leftOperand->value, QString("b"));
    QCOMPARE((int)root->rightOperand->type, (int)Var);
    QCOMPARE(root->rightOperand->value, QString("c"));
    QCOMPARE(sources.size(), 2);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 5: Бинарный оператор *
// Вход: "x y *"
// Ожидается: узел Multiply с левым Var("x") и правым Var("y")
// ============================================================
void TEST_ParseExpression::TestBinaryMultiply()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("x y *", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Multiply);
    QVERIFY(root->leftOperand != nullptr);
    QVERIFY(root->rightOperand != nullptr);
    QCOMPARE((int)root->leftOperand->type, (int)Var);
    QCOMPARE(root->leftOperand->value, QString("x"));
    QCOMPARE((int)root->rightOperand->type, (int)Var);
    QCOMPARE(root->rightOperand->value, QString("y"));
    QCOMPARE(sources.size(), 2);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 6: Унарный оператор ++
// Вход: "x ++"
// Ожидается: узел Increment с потомком Var("x")
// ============================================================
void TEST_ParseExpression::TestUnaryIncrement()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("x ++", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Increment);
    QVERIFY(root->leftOperand != nullptr);
    QCOMPARE((int)root->leftOperand->type, (int)Var);
    QCOMPARE(root->leftOperand->value, QString("x"));
    QCOMPARE(sources.size(), 1);
    QCOMPARE(modified.size(), 1);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 7: Унарный оператор --
// Вход: "a --"
// Ожидается: узел Decrement с потомком Var("a")
// ============================================================
void TEST_ParseExpression::TestUnaryDecrement()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("a --", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Decrement);
    QVERIFY(root->leftOperand != nullptr);
    QCOMPARE((int)root->leftOperand->type, (int)Var);
    QCOMPARE(root->leftOperand->value, QString("a"));
    QCOMPARE(sources.size(), 1);
    QCOMPARE(modified.size(), 1);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 8: Доступ к элементу массива
// Вход: "a 2 []"
// Ожидается: узел ArrayAccess с левым Var("a") и правым Number("2")
// ============================================================
void TEST_ParseExpression::TestArrayAccess()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("a 2 []", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)ArrayAccess);
    QVERIFY(root->leftOperand != nullptr);
    QVERIFY(root->rightOperand != nullptr);
    QCOMPARE((int)root->leftOperand->type, (int)Var);
    QCOMPARE(root->leftOperand->value, QString("a"));
    QCOMPARE((int)root->rightOperand->type, (int)Number);
    QCOMPARE(root->rightOperand->value, QString("2"));
    QCOMPARE(sources.size(), 1);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 9: Многомерный массив
// Вход: "a i [] j []"
// Ожидается: цепочка ArrayAccess (двумерный массив)
// ============================================================
void TEST_ParseExpression::TestMultidimensionalArray()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("a i [] j []", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)ArrayAccess);
    QVERIFY(root->rightOperand != nullptr);
    QCOMPARE((int)root->rightOperand->type, (int)Var);
    QCOMPARE(root->rightOperand->value, QString("j"));

    ExprNode* inner = root->leftOperand;
    QVERIFY(inner != nullptr);
    QCOMPARE((int)inner->type, (int)ArrayAccess);
    QCOMPARE((int)inner->leftOperand->type, (int)Var);
    QCOMPARE(inner->leftOperand->value, QString("a"));
    QCOMPARE((int)inner->rightOperand->type, (int)Var);
    QCOMPARE(inner->rightOperand->value, QString("i"));
    QCOMPARE(sources.size(), 3);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 10: Недостаточно операндов для +
// Вход: "b +"
// Ожидается: ошибка NotEnoughOperands
// ============================================================
void TEST_ParseExpression::TestNotEnoughOperandsPlus()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("b +", sources, modified, 1, errors);

    QVERIFY(errors.size() > 0);

    if (root != nullptr) {
        delete root;
    }
}

// ============================================================
// Тест 11: Недостаточно операндов для ++
// Вход: "++"
// Ожидается: ошибка NotEnoughOperands
// ============================================================
void TEST_ParseExpression::TestNotEnoughOperandsIncrement()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("++", sources, modified, 1, errors);

    QVERIFY(errors.size() > 0);

    if (root != nullptr) {
        delete root;
    }
}

// ============================================================
// Тест 12: Лишние операнды
// Вход: "b c d +"
// Ожидается: ошибка ExtraOperands
// ============================================================
void TEST_ParseExpression::TestExtraOperands()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("b c d +", sources, modified, 1, errors);

    QVERIFY(errors.size() > 0);

    if (root != nullptr) {
        delete root;
    }
}

// ============================================================
// Тест 13: Использование массива в выражении
// Вход: "a 2 [] b +"
// Ожидается: корень +, левый a[2], правый b
// ============================================================
void TEST_ParseExpression::TestArrayInExpression()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("a 2 [] b +", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Plus);
    QVERIFY(root->leftOperand != nullptr);
    QCOMPARE((int)root->leftOperand->type, (int)ArrayAccess);
    QVERIFY(root->rightOperand != nullptr);
    QCOMPARE((int)root->rightOperand->type, (int)Var);
    QCOMPARE(root->rightOperand->value, QString("b"));
    QCOMPARE(sources.size(), 2);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 14: Инкремент элемента массива (должен вызывать ошибку)
// Вход: "a 2 [] ++"
// Ожидается: ошибка (++ можно применять только к переменным)
// ============================================================
void TEST_ParseExpression::TestIncrementArrayElement()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("a 2 [] ++", sources, modified, 1, errors);

    QVERIFY(errors.size() > 0);

    if (root != nullptr) {
        delete root;
    }
}

// ============================================================
// Тест 15: Комплексный тест
// Вход: "x y + a 2 [] *"
// Ожидается: корень *, левый +, правый a[2]
// ============================================================
void TEST_ParseExpression::TestComplexExpression()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("x y + a 2 [] *", sources, modified, 1, errors);

    QVERIFY(root != nullptr);
    QCOMPARE((int)root->type, (int)Multiply);
    QVERIFY(root->leftOperand != nullptr);
    QCOMPARE((int)root->leftOperand->type, (int)Plus);
    QVERIFY(root->rightOperand != nullptr);
    QCOMPARE((int)root->rightOperand->type, (int)ArrayAccess);
    QCOMPARE(sources.size(), 3);
    QCOMPARE(modified.size(), 0);
    QCOMPARE(errors.size(), 0);

    delete root;
}

// ============================================================
// Тест 16: Комплексный тест с ошибками
// Вход: "a b + c ++ d e"
// Ожидается: ошибка ExtraOperands
// ============================================================
void TEST_ParseExpression::TestComplexWithErrors()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("a b + c ++ d e", sources, modified, 1, errors);

    QVERIFY(errors.size() > 0);

    if (root != nullptr) {
        delete root;
    }
}

// ============================================================
// Тест 17: Комплексный тест (инкремент + декремент)
// Вход: "a 2 [] ++ x -- b c + * *"
// Ожидается: ошибка из-за a[2]++
// ============================================================
void TEST_ParseExpression::TestComplexIncrementDecrement()
{
    QSet<Error> errors;
    QList<ExprNode*> sources;
    QList<ExprNode*> modified;

    ExprNode* root = parseExpression("a 2 [] ++ x -- b c + * *", sources, modified, 1, errors);

    QVERIFY(errors.size() > 0);

    if (root != nullptr) {
        delete root;
    }
}
