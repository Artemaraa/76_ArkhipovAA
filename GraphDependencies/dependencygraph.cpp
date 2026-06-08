#include "dependencygraph.h"

DependencyGraph::DependencyGraph() {}

DependencyGraph::~DependencyGraph() {}

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

// Заглушка: граф не строится
void DependencyGraph::buildGraph(const QList<Action*>& actionsList,
                                 QMap<QString, Action*>& varTable,
                                 QSet<Error>& errors)
{
    Q_UNUSED(actionsList);
    Q_UNUSED(varTable);
    Q_UNUSED(errors);
}
// Заглушка пустой граф
QString DependencyGraph::toDOT() const
{
    return QString();
}
