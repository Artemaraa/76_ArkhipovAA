#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <QMap>
#include <QString>

#include "exprnode.h"
#include "action.h"
#include "dependencyedge.h"
class DependencyGraph
{
public:
    DependencyGraph();
    ~DependencyGraph();

    DependencyType determineDependency(ExprNode* varNode,
                                       const QMap<QString, Action*>& varTable,
                                       Action*& dependencyAction);
};

#endif // DEPENDENCYGRAPH_H
