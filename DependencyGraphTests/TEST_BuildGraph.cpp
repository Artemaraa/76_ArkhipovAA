#include "TEST_BuildGraph.h"
#include "dependencygraph.h"
#include "functions.h"
#include "action.h"

// ============================================================
// Тест 1: Одно действие
// ============================================================
void TEST_BuildGraph::TestSingleAction()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a = b c +"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 1);
    QVERIFY(varTable.contains("a"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 2: Два действия с зависимостью
// ============================================================
void TEST_BuildGraph::TestTwoActionsWithDependency()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a = b c +", "b = a 1 *"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 2);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    QList<Action*> incoming = graph.getIncoming(action2);
    QVERIFY(incoming.contains(action1));

    // Тип ребра: прямая зависимость (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 3: Три действия (базовый пример)
// ============================================================
void TEST_BuildGraph::TestThreeActions()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a = b c +", "b = c d +", "e = b a +"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 3);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    Action* action3 = actions[2];

    QList<Action*> incoming3 = graph.getIncoming(action3);
    QVERIFY(incoming3.contains(action1));
    QVERIFY(incoming3.contains(action2));

    // Оба ребра прямые (сплошные)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("3 -> 1;"));
    QVERIFY(dot.contains("3 -> 2;"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 4: Полный пример из спецификации (5 строк)
// ============================================================
void TEST_BuildGraph::TestFullExampleFromSpec()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a = b c +", "b = c d +", "e = b a +", "a = 2 e *", "f = a d +"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 4);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    Action* action3 = actions[2];
    Action* action4 = actions[3];
    Action* action5 = actions[4];

    QList<Action*> incoming3 = graph.getIncoming(action3);
    QVERIFY(incoming3.contains(action1));
    QVERIFY(incoming3.contains(action2));

    QList<Action*> incoming4 = graph.getIncoming(action4);
    QVERIFY(incoming4.contains(action3));

    QList<Action*> incoming5 = graph.getIncoming(action5);
    QVERIFY(incoming5.contains(action4));

    // Все рёбра прямые (сплошные)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("3 -> 1;"));
    QVERIFY(dot.contains("3 -> 2;"));
    QVERIFY(dot.contains("4 -> 3;"));
    QVERIFY(dot.contains("5 -> 4;"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 5: Массивы - прямая и общая зависимость
// ============================================================
void TEST_BuildGraph::TestArrayDirectAndGeneral()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a 2 [] = 4", "b = a 2 [] 2 *", "c = a i [] 1 +"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 3);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    Action* action3 = actions[2];

    QList<Action*> incoming2 = graph.getIncoming(action2);
    QVERIFY(incoming2.contains(action1));

    QList<Action*> incoming3 = graph.getIncoming(action3);
    QVERIFY(incoming3.contains(action1));

    // a[2] после a[2] - прямая (сплошная); a[i] после a[2] - общая (пунктир)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));
    QVERIFY(dot.contains("3 -> 1 [style=dashed]"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 6: Инкремент
// ============================================================
void TEST_BuildGraph::TestIncrement()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"x = 5", "y = x ++", "z = x"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    Action* action3 = actions[2];

    QList<Action*> incoming2 = graph.getIncoming(action2);
    QVERIFY(incoming2.contains(action1));

    QList<Action*> incoming3 = graph.getIncoming(action3);
    QVERIFY(incoming3.contains(action2));

    // Зависимость через ++ - прямая (сплошная)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));
    QVERIFY(dot.contains("3 -> 2;"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 7: Переопределение переменной
// ============================================================
void TEST_BuildGraph::TestVariableRedefinition()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a = 1", "b = a 2 +", "a = 3", "c = a b +"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    Action* action3 = actions[2];
    Action* action4 = actions[3];

    QList<Action*> incoming2 = graph.getIncoming(action2);
    QVERIFY(incoming2.contains(action1));

    QList<Action*> incoming4 = graph.getIncoming(action4);
    QVERIFY(incoming4.contains(action3));
    QVERIFY(incoming4.contains(action2));

    // Все рёбра прямые (сплошные)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));
    QVERIFY(dot.contains("4 -> 3;"));
    QVERIFY(dot.contains("4 -> 2;"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 8: Отсутствие транзитивных связей
// ============================================================
void TEST_BuildGraph::TestNoTransitiveEdges()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a = 1", "b = a", "c = b"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    Action* action3 = actions[2];

    QList<Action*> incoming2 = graph.getIncoming(action2);
    QVERIFY(incoming2.contains(action1));

    QList<Action*> incoming3 = graph.getIncoming(action3);
    QVERIFY(incoming3.contains(action2));
    QVERIFY(!incoming3.contains(action1));

    // Прямые рёбра 2->1 и 3->2; транзитивного 3->1 нет
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));
    QVERIFY(dot.contains("3 -> 2;"));
    QVERIFY(!dot.contains("3 -> 1"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 9: Ошибка - неверная размерность массива
// ============================================================
void TEST_BuildGraph::TestErrorInvalidArrayDimension()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    // a[2] = 4 (1 измерение), затем a[i][j] = 5 (2 измерения) - несоответствие
    QStringList lines = {"a 2 [] = 4", "a i [] j [] = 5"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    // Должна быть ошибка InvalidArrayDimension
    bool foundError = false;
    for (const Error& e : errors) {
        if (e.type == InvalidArrayDimension) {
            foundError = true;
            break;
        }
    }
    QVERIFY(foundError);

    // varTable не изменилась после первой строки: только a -> Action(1)
    QCOMPARE(varTable.size(), 1);
    QVERIFY(varTable.contains("a"));
    QCOMPARE(varTable["a"], actions[0]);

    qDeleteAll(actions);
}

// ============================================================
// Тест 10: Пустой входной файл
// ============================================================
void TEST_BuildGraph::TestEmptyInputFile()
{
    QSet<Error> errors;
    QList<Action*> actions;
    QStringList lines = {};
    parseActions(lines, actions, errors);
    // parseActions теперь сам ловит пустой файл и выдаёт EmptyInputFile
    QCOMPARE(errors.size(), 1);
    QCOMPARE(actions.size(), 0);
    // Проверить, что это именно ошибка пустого файла
    QVERIFY(errors.contains(Error(EmptyInputFile, 0, 0, "")));
}

// ============================================================
// Тест 11: Смешанный тест (массивы + инкремент + переопределение)
// ============================================================
void TEST_BuildGraph::TestMixedArraysIncrementRedefinition()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"a 2 [] = 4", "b = a 2 [] 2 *", "a i [] = i 1 +",
                         "c = a 2 [] 1 +", "x = 5", "y = x ++", "z = x"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 6);

    Action* action1 = actions[0];
    Action* action2 = actions[1];
    Action* action3 = actions[2];
    Action* action4 = actions[3];
    Action* action5 = actions[4];
    Action* action6 = actions[5];
    Action* action7 = actions[6];

    QList<Action*> incoming2 = graph.getIncoming(action2);
    QVERIFY(incoming2.contains(action1));

    QList<Action*> incoming4 = graph.getIncoming(action4);
    QVERIFY(incoming4.contains(action3));

    QList<Action*> incoming6 = graph.getIncoming(action6);
    QVERIFY(incoming6.contains(action5));

    QList<Action*> incoming7 = graph.getIncoming(action7);
    QVERIFY(incoming7.contains(action6));

    // 2->1 прямая (a[2] после a[2]); 4->3 общая (a[2] после a[i]);
    // 6->5 и 7->6 прямые (инкремент и чтение после него)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));
    QVERIFY(dot.contains("4 -> 3 [style=dashed]"));
    QVERIFY(dot.contains("6 -> 5;"));
    QVERIFY(dot.contains("7 -> 6;"));

    qDeleteAll(actions);
}

// ============================================================
// Тест 12: Комплексный тест
// ============================================================
void TEST_BuildGraph::TestComprehensive()
{
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    QSet<Error> errors;

    QList<Action*> actions;
    QStringList lines = {"x = 5", "y = x ++", "z = x y +", "a 2 [] = 10",
                         "b = a 2 [] 3 *", "c = a i [] 1 +", "i = 2", "d = a i [] 1 +"};
    parseActions(lines, actions, errors);

    graph.buildGraph(actions, varTable, errors);

    QCOMPARE(errors.size(), 0);
    QCOMPARE(varTable.size(), 8);

    // Ожидаемые рёбра и типы:
    // 2->1 (D), 3->2 (D), 5->4 (D), 6->4 (G), 8->7 (D), 8->4 (G)
    QString dot = graph.toDOT();
    QVERIFY(dot.contains("2 -> 1;"));                 // y от x (через ++) - прямая
    QVERIFY(dot.contains("3 -> 2;"));                 // z от y/x - прямая
    QVERIFY(dot.contains("5 -> 4;"));                 // b от a[2] - прямая
    QVERIFY(dot.contains("6 -> 4 [style=dashed]"));   // c от a[i] после a[2] - общая
    QVERIFY(dot.contains("8 -> 7;"));                 // d от i - прямая
    QVERIFY(dot.contains("8 -> 4 [style=dashed]"));   // d от a[i] после a[2] - общая

    // Транзитивного 3->1 быть не должно (x перезаписан действием 2)
    QVERIFY(!dot.contains("3 -> 1"));

    // Итоговая таблица переменных
    QCOMPARE(varTable["x"], actions[1]);
    QCOMPARE(varTable["y"], actions[1]);
    QCOMPARE(varTable["z"], actions[2]);
    QCOMPARE(varTable["a"], actions[3]);
    QCOMPARE(varTable["b"], actions[4]);
    QCOMPARE(varTable["c"], actions[5]);
    QCOMPARE(varTable["i"], actions[6]);
    QCOMPARE(varTable["d"], actions[7]);

    qDeleteAll(actions);
}
