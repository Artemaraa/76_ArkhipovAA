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
