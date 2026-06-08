#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <QMap>
#include <QList>
#include <QSet>
#include <QString>

#include "exprnode.h"
#include "action.h"
#include "dependencyedge.h"
#include "error.h"

class DependencyGraph
{
public:
    DependencyGraph();
    ~DependencyGraph();

    DependencyType determineDependency(ExprNode* varNode,
                                       const QMap<QString, Action*>& varTable,
                                       Action*& dependencyAction);

    void applyAction(Action* currentAction,
                     ExprNode* targetRoot,
                     const QList<ExprNode*>& sourceVariables,
                     const QList<ExprNode*>& modifiedVariables,
                     QMap<QString, Action*>& varTable,
                     QSet<Error>& errors);

    QList<Action*> getIncoming(Action* action) const;

    void buildGraph(const QList<Action*>& actionsList,
                    QMap<QString, Action*>& varTable,
                    QSet<Error>& errors);

    QString toDOT() const;
};

#endif // DEPENDENCYGRAPH_H
