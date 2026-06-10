#include "dependencygraph.h"

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

// Заглушка действие не применяется
void DependencyGraph::applyAction(Action* currentAction,
                                  ExprNode* targetRoot,
                                  const QList<ExprNode*>& sourceVariables,
                                  const QList<ExprNode*>& modifiedVariables,
                                  QMap<QString, Action*>& varTable,
                                  QSet<Error>& errors)
{
    Q_UNUSED(currentAction);
    Q_UNUSED(targetRoot);
    Q_UNUSED(sourceVariables);
    Q_UNUSED(modifiedVariables);
    Q_UNUSED(varTable);
    Q_UNUSED(errors);
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
