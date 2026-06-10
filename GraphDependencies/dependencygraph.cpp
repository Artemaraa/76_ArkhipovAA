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

void DependencyGraph::addAction(Action* action)
{
    Q_UNUSED(action);
}

void DependencyGraph::addEdge(Action* from, Action* to, DependencyType type)
{
    Q_UNUSED(from);
    Q_UNUSED(to);
    Q_UNUSED(type);
}

bool DependencyGraph::validateArrayDimension(ExprNode* targetRoot,
                                             const QString& varName,
                                             const QMap<QString, Action*>& varTable,
                                             int lineNumber,
                                             Error& error)
{
    Q_UNUSED(targetRoot);
    Q_UNUSED(varName);
    Q_UNUSED(varTable);
    Q_UNUSED(lineNumber);
    Q_UNUSED(error);
    return true;
}

// Заглушка: связь не найдена, источника нет
DependencyType DependencyGraph::determineDependency(ExprNode* varNode,
                                                    const QMap<QString, Action*>& varTable,
                                                    Action*& dependencyAction)
{
    Q_UNUSED(varNode);
    Q_UNUSED(varTable);
    dependencyAction = nullptr;
    return NoDependency;
}

// Заглушка входящих связей нет
QList<Action*> DependencyGraph::getIncoming(Action* action) const
{
    Q_UNUSED(action);
    return QList<Action*>();
}

// Заглушка пустой граф
QString DependencyGraph::toDOT() const
{
    return QString();
}
