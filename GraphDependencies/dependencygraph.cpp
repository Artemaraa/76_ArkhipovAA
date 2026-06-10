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
            applyAction(action,
                        action->targetRoot,
                        action->sourceVariables,
                        action->modifiedVariables,
                        varTable,
                        errors);
        }
    }
}

// Применение одного действия
void DependencyGraph::applyAction(Action* currentAction,
                                  ExprNode* targetRoot,
                                  const QList<ExprNode*>& sourceVariables,
                                  const QList<ExprNode*>& modifiedVariables,
                                  QMap<QString, Action*>& varTable,
                                  QSet<Error>& errors)
{
    // Если действие пусто, выйти
    if (currentAction == nullptr) {
        return;
    }
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
    for (DependencyEdge* edge : edges) {
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

DependencyType DependencyGraph::determineDependency(ExprNode* varNode,
                                                    const QMap<QString, Action*>& varTable,
                                                    Action*& dependencyAction)
{
    Q_UNUSED(varNode);
    Q_UNUSED(varTable);
    dependencyAction = nullptr;
    return NoDependency;
}

QList<Action*> DependencyGraph::getIncoming(Action* action) const
{
    Q_UNUSED(action);
    return QList<Action*>();
}

QString DependencyGraph::toDOT() const
{
    return QString();
}
