#include "dependencygraph.h"
#include "functions.h"

DependencyGraph::DependencyGraph() {}

DependencyGraph::~DependencyGraph()
{
    clear();
}

// Очистка графа
void DependencyGraph::clear()
{
    // Удалить все рёбра
    qDeleteAll(edges);
    // Опустошить списки и таблицы навигации
    actions.clear();
    edges.clear();
    outgoing.clear();
    incoming.clear();
}

// Построение графа по всем действиям
void DependencyGraph::buildGraph(const QList<Action*>& actionsList, QMap<QString, Action*>& varTable, QSet<Error>& errors)
{
    // Очистить таблицу переменных и граф
    varTable.clear();
    clear();

    // Пройти по списку действий
    for (int i = 0; i < actionsList.size(); ++i) {
        Action* action = actionsList[i];
        // Для каждого действия вызвать применение действия
        if (action != nullptr) {
            applyAction(action, varTable, errors);
        }
    }
}

// Применение одного действия
void DependencyGraph::applyAction(Action* currentAction, QMap<QString, Action*>& varTable, QSet<Error>& errors)
{
    // Если действие пусто, выйти
    if (currentAction == nullptr) {
        return;
    }
    // Взять данные из самого действия
    ExprNode* targetRoot = currentAction->targetRoot;
    const QList<ExprNode*>& sourceVariables = currentAction->sourceVariables;
    const QList<ExprNode*>& modifiedVariables = currentAction->modifiedVariables;

    // Узнать имя целевой переменной; если имя пустое - выйти (некорректная цель)
    const QString targetName = getArrayName(targetRoot);
    if (targetName.isEmpty()) {
        return;
    }
    // Проверить корректность размерности цели; при несоответствии добавить ошибку и выйти
    Error dimError;
    if (!validateArrayDimension(targetRoot, targetName, varTable, currentAction->number, dimError)) {
        errors.insert(dimError);
        return;
    }
    // Для каждой прочитанной переменной определить зависимость по таблице состояний
    for (ExprNode* varNode : sourceVariables) {
        Action* dependencyAction = nullptr;
        const DependencyType depType = determineDependency(varNode, varTable, dependencyAction);
        // Если зависимость есть,добавить ребро от текущего действия к найденному
        if (depType != NoDependency && dependencyAction != nullptr) {
            addEdge(currentAction, dependencyAction, depType);
        }
    }
    // Запомнить, что целевую переменную последним изменило текущее действие
    varTable[targetName] = currentAction;
    // Для каждой изменяемой переменной (++/--) обновить запись в таблице на текущее действие
    for (ExprNode* varNode : modifiedVariables) {
        if (varNode) {
            const QString varName = getArrayName(varNode);
            if (!varName.isEmpty()) {
                varTable[varName] = currentAction;
            }
        }
    }
    // Добавить текущее действие в список вершин графа
    addAction(currentAction);
}

// Добавление вершины
void DependencyGraph::addAction(Action* action)
{
    // если указатель не пуст, добавить действие в список вершин
    if (action) {
        actions.append(action);
    }
}

// Добавление ребра зависимости
void DependencyGraph::addEdge(Action* from, Action* to, DependencyType type)
{
    // Если любое из действий отсутствует - ничего не делать
    if (!from || !to) return;

    // Проверить список рёбер на дубликат (зависимость уже имеется)
    bool exists = false;
    for (const DependencyEdge* edge : edges) {
        if (edge->from == from && edge->to == to) {
            exists = true;
            break;// дубликат найден, прекращаем поиск
        }
    }
    // Если дубликата нет
    if (!exists) {
        // Создать ребро
        DependencyEdge* newEdge = new DependencyEdge(from, to, type);
        edges.append(newEdge);
        // Обнавление таблицы навигации
        // Исходящие (источник -> зависимое) и входящие (зависимое -> источник)
        outgoing.insert(to, from);
        incoming.insert(from, to);
    }
}

// Проверка размерности целевой переменной
bool DependencyGraph::validateArrayDimension(ExprNode* targetRoot,
                                             const QString& varName,
                                             const QMap<QString, Action*>& varTable,
                                             int lineNumber,
                                             Error& error)
{
    // Вычислить размерность текущей цели
    const int currentDim = getArrayDimension(targetRoot);
    // Если имя ещё не встречалось в таблице, это первое использование
    if (!varTable.contains(varName)) {
        return true;
    }
    // Взять последнее действие, изменявшее эту переменную
    Action* lastAction = varTable[varName];
    if (lastAction == nullptr || lastAction->targetRoot == nullptr) {
        return true;
    }
    // Вычислить размерность у переменной, которую мы взяли
    const int savedDim = getArrayDimension(lastAction->targetRoot);
    // Если размерности совпадают - вернуть успех
    if (currentDim == savedDim) {
        return true;
    }
    // Если размерности разошлись,заполнить ошибку точным типом
    if (savedDim == 0 && currentDim > 0) {
        error = Error(ScalarWithIndex, lineNumber, 0, varName); // был скаляр, стал массив
    } else if (savedDim > 0 && currentDim == 0) {
        error = Error(ArrayWithoutIndex, lineNumber, 0, varName); // был массив, стал скаляр
    } else {
        error = Error(InvalidArrayDimension, lineNumber, 0, varName); // число измерений разное
    }
    return false;
}

// Определение зависимости для одной прочитанной переменной
DependencyType DependencyGraph::determineDependency(ExprNode* varNode,
                                                    const QMap<QString, Action*>& varTable,
                                                    Action*& dependencyAction)
{
    DependencyType result = NoDependency;// по умолчанию связи нет
    dependencyAction = nullptr;
    // Если узел задан, найти источник
    if (varNode) {
        const QString varName = getArrayName(varNode);
        // Имя должно быть непустым и присутствовать в таблице
        if (!varName.isEmpty() && varTable.contains(varName)) {
            Action* last = varTable[varName];
            // переменная изменилась / объявилась ранее
            if (last != nullptr && last->targetRoot != nullptr) {
                dependencyAction = last;
                // полчить имя цели действия
                const QString targetName = getArrayName(last->targetRoot);
                if (targetName == varName) {
                    // сравнить индексы у полученной цели и заданного узла
                    result = compareIndices(varNode, last->targetRoot);
                } else {
                    // Пользователь изменил переменную через ++/-- прямая связь
                    result = Direct;
                }
            }
        }
    }
    // Вернуть результат
    return result;
}

// Навигация на кого влияет действие
QList<Action*> DependencyGraph::getOutgoing(Action* action) const
{
    return outgoing.values(action);
}

// Навигация от кого зависит действие
QList<Action*> DependencyGraph::getIncoming(Action* action) const
{
    return incoming.values(action);
}

// Формирование DOT-текста
QString DependencyGraph::toDOT() const
{
    // Записать заголовок графа
    QString result = "digraph G {\n";

    // Для каждой вершины добавить строку: номер действия и подпись с исходной строкой
    for (Action* action : actions) {
        result += "    " + QString::number(action->number) +
                  " [label=\"" + action->originalLine + "\"];\n";
    }

    // Для каждого ребра добавить строку-стрелку "from -> to"
    for (DependencyEdge* edge : edges) {
        result += "    " + QString::number(edge->from->number) +
                  " -> " + QString::number(edge->to->number);
        // Если связь общая (General) - пометить её пунктиром
        if (edge->type == General) {
            result += " [style=dashed]";
        }
        result += ";\n";
    }

    // Закрыть блок графа и вернуть собранный текст
    result += "}\n";
    return result;
}
