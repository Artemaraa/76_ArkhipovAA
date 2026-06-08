#include "TEST_ParseActions.h"
#include "functions.h"
#include "action.h"


// ============================================================
// Тест 1: Простое присваивание
// ============================================================
void TEST_ParseActions::TestSimpleAssignment()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b c +"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->targetRoot != nullptr);
    QVERIFY(action->expression != nullptr);
    QCOMPARE(action->sourceVariables.size(), 2);
    QCOMPARE(action->modifiedVariables.size(), 0);

    qDeleteAll(actions);
}

// ============================================================
// Тест 2: Присваивание с массивом
// ============================================================
void TEST_ParseActions::TestWithArray()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a 2 [] = 4"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->targetRoot != nullptr);
    QCOMPARE((int)action->targetRoot->type, (int)ArrayAccess);

    qDeleteAll(actions);
}

// ============================================================
// Тест 3: Инкремент
// ============================================================
void TEST_ParseActions::TestWithIncrement()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"x = x ++"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->expression != nullptr);
    QCOMPARE((int)action->expression->type, (int)Increment);
    QCOMPARE(action->modifiedVariables.size(), 1);

    qDeleteAll(actions);
}

// ============================================================
// Тест 4: Пустая левая часть
// ============================================================
void TEST_ParseActions::TestEmptyLeftPart()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"= b c +"};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == EmptyLeftPart) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 5: Пустое выражение
// ============================================================
void TEST_ParseActions::TestEmptyRightPart()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a ="};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == EmptyExpression) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 6: Нет знака равно
// ============================================================
void TEST_ParseActions::TestNoEqualSign()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a b c +"};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == NoEqualSign) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 7: Несколько строк
// ============================================================
void TEST_ParseActions::TestMultipleLines()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b c +", "b = a 1 *", "c = a b +"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 3);
    QCOMPARE(errors.size(), 0);

    for (int i = 0; i < actions.size(); ++i) {
        QCOMPARE(actions[i]->number, i + 1);
    }

    qDeleteAll(actions);
}

// ============================================================
// Тест 8: Пустые строки
// ============================================================
void TEST_ParseActions::TestEmptyLines()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"", "a = b c +", "", "b = a 1 *", ""};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 2);
    QCOMPARE(errors.size(), 0);

    qDeleteAll(actions);
}

// ============================================================
// Тест 9: Отрицательное число
// ============================================================
void TEST_ParseActions::TestNegativeNumber()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = -5"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->expression != nullptr);
    QCOMPARE((int)action->expression->type, (int)Number);
    QCOMPARE(action->expression->value, QString("-5"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 10: Бинарный оператор вычитания
// ============================================================
void TEST_ParseActions::TestBinaryMinus()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b c -"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->expression != nullptr);
    QCOMPARE((int)action->expression->type, (int)Minus);

    qDeleteAll(actions);
}

// ============================================================
// Тест 11: Бинарный оператор деления
// ============================================================
void TEST_ParseActions::TestBinaryDivide()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b c /"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->expression != nullptr);
    QCOMPARE((int)action->expression->type, (int)Divide);

    qDeleteAll(actions);
}

// ============================================================
// Тест 12: Унарный декремент
// ============================================================
void TEST_ParseActions::TestUnaryDecrement()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b --"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->expression != nullptr);
    QCOMPARE((int)action->expression->type, (int)Decrement);

    qDeleteAll(actions);
}

// ============================================================
// Тест 13: Многомерный массив в левой части
// ============================================================
void TEST_ParseActions::TestMultidimensionalLeft()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a i [] j [] = 5"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->targetRoot != nullptr);
    QCOMPARE((int)action->targetRoot->type, (int)ArrayAccess);

    qDeleteAll(actions);
}

// ============================================================
// Тест 14: Многомерный массив в правой части
// ============================================================
void TEST_ParseActions::TestMultidimensionalRight()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"b = a i [] j [] 2 *"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->expression != nullptr);

    qDeleteAll(actions);
}

// ============================================================
// Тест 15: Сложное выражение
// ============================================================
void TEST_ParseActions::TestComplexExpression()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b c + d e * +"};
    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 1);
    QCOMPARE(errors.size(), 0);

    Action* action = actions[0];
    QVERIFY(action->expression != nullptr);
    QCOMPARE(action->sourceVariables.size(), 4);

    qDeleteAll(actions);
}

// ============================================================
// Тест 16: Недопустимое имя переменной
// ============================================================
void TEST_ParseActions::TestInvalidVariableName()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"1a = 5"};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == InvalidVariableName) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 17: Число вне диапазона
// ============================================================
void TEST_ParseActions::TestNumberOutOfRange()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = 9999999999999"};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == NumberOutOfRange) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 18: Неподдерживаемый оператор
// ============================================================
void TEST_ParseActions::TestUnsupportedOperator()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b c ^"};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == UnsupportedOperator) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 19: Недостаточно операндов
// ============================================================
void TEST_ParseActions::TestNotEnoughOperands()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b +"};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == NotEnoughOperands) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 20: Лишние операнды
// ============================================================
void TEST_ParseActions::TestExtraOperands()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {"a = b c + d e"};
    parseActions(lines, actions, errors);

    bool found = false;
    for (const Error& e : errors) {
        if (e.type == ExtraOperands) {
            found = true;
            break;
        }
    }
    QVERIFY(found);

    qDeleteAll(actions);
}

// ============================================================
// Тест 21: Комплексный тест (все типы выражений)
// ============================================================
void TEST_ParseActions::TestComprehensive()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {
        "a = b c +",                    // простое сложение
        "x = 42",                       // число
        "y = -5",                       // отрицательное число
        "arr 2 [] = 10",                // массив в левой части
        "b = arr 2 [] 2 *",             // массив в правой части
        "counter = counter ++",         // инкремент
        "value = value --",             // декремент
        "result = a b + arr 2 [] *",    // сложное выражение
        "matrix i [] j [] = 0",         // многомерный массив
        "z = matrix i [] j [] 1 +"      // многомерный в правой части
    };

    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 10);
    QCOMPARE(errors.size(), 0);

    for (int i = 0; i < actions.size(); ++i) {
        QCOMPARE(actions[i]->number, i + 1);
    }

    QCOMPARE(actions[0]->sourceVariables.size(), 2);
    QCOMPARE((int)actions[1]->expression->type, (int)Number);
    QCOMPARE(actions[1]->expression->value, QString("42"));
    QCOMPARE((int)actions[2]->expression->type, (int)Number);
    QCOMPARE(actions[2]->expression->value, QString("-5"));
    QCOMPARE((int)actions[3]->targetRoot->type, (int)ArrayAccess);
    QCOMPARE((int)actions[4]->expression->type, (int)Multiply);
    QCOMPARE((int)actions[5]->expression->type, (int)Increment);
    QCOMPARE(actions[5]->modifiedVariables.size(), 1);
    QCOMPARE((int)actions[6]->expression->type, (int)Decrement);
    QCOMPARE(actions[6]->modifiedVariables.size(), 1);
    QCOMPARE((int)actions[7]->expression->type, (int)Multiply);
    QCOMPARE(actions[7]->sourceVariables.size(), 3);
    QCOMPARE((int)actions[8]->targetRoot->type, (int)ArrayAccess);
    QCOMPARE(actions[9]->sourceVariables.size(), 3);

    qDeleteAll(actions);
}

// ============================================================
// Тест 22: Комплексный тест с ошибками
// ============================================================
void TEST_ParseActions::TestComprehensiveWithErrors()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {
        "a = b c +",                    // корректно
        "= b c +",                      // ошибка: пустая левая часть
        "x = 5",                        // корректно
        "a =",                          // ошибка: пустое выражение
        "arr 2 [] = 10",                // корректно
        "a b c +",                      // ошибка: нет знака =
        "y = x ++"                      // корректно
    };

    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 4);
    QCOMPARE(errors.size(), 3);

    bool hasEmptyLeftPart = false;
    bool hasEmptyExpression = false;
    bool hasNoEqualSign = false;

    for (const Error& e : errors) {
        if (e.type == EmptyLeftPart) hasEmptyLeftPart = true;
        if (e.type == EmptyExpression) hasEmptyExpression = true;
        if (e.type == NoEqualSign) hasNoEqualSign = true;
    }

    QVERIFY(hasEmptyLeftPart);
    QVERIFY(hasEmptyExpression);
    QVERIFY(hasNoEqualSign);

    QCOMPARE(actions[0]->number, 1);
    QCOMPARE(actions[1]->number, 3);
    QCOMPARE(actions[2]->number, 5);
    QCOMPARE(actions[3]->number, 7);

    qDeleteAll(actions);
}

// ============================================================
// Тест 23: Граничные значения
// ============================================================
void TEST_ParseActions::TestBoundaryValues()
{
    QSet<Error> errors;
    QList<Action*> actions;

    QStringList lines = {
        "a = 2147483647",
        "b = -2147483648",
        "c = 0",
        "d = 1"
    };

    parseActions(lines, actions, errors);

    QCOMPARE(actions.size(), 4);

    if (errors.size() == 0) {
        QCOMPARE(actions[0]->expression->value, QString("2147483647"));
        QCOMPARE(actions[1]->expression->value, QString("-2147483648"));
        QCOMPARE(actions[2]->expression->value, QString("0"));
        QCOMPARE(actions[3]->expression->value, QString("1"));
    }

    qDeleteAll(actions);
}

