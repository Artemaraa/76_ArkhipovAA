#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include <QMultiMap>

#include "exprnode.h"
#include "action.h"
#include "dependencyedge.h"
#include "error.h"

/**
 * @brief Граф зависимостей действий.
 */
class DependencyGraph
{
private:
    QList<Action*> actions;                  ///< вершины графа
    QList<DependencyEdge*> edges;            ///< рёбра
    QMultiMap<Action*, Action*> outgoing;    ///< на кого влияет действие
    QMultiMap<Action*, Action*> incoming;    ///< от кого зависит действие
public:
    DependencyGraph();
    ~DependencyGraph();

    /**
     * @brief Освобождает рёбра и очищает все списки графа.
     */
    void clear();

    /**
     * @brief Строит граф по списку действий.
     * @param[in]     actionsList  список действий
     * @param[in,out] varTable     таблица переменных (последнее изменившее действие)
     * @param[in,out] errors       множество ошибок
     */
    void buildGraph(const QList<Action*>& actionsList,QMap<QString, Action*>& varTable, QSet<Error>& errors);


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

    QString toDOT() const;
};

#endif // DEPENDENCYGRAPH_H
