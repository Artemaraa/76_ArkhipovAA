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

    /**
     * @brief Применяет одно действие: строит рёбра и обновляет таблицу переменных
     * @param[in]     currentAction     текущее действие
     * @param[in]     targetRoot        корень левой части
     * @param[in]     sourceVariables   прочитанные переменные правой части
     * @param[in]     modifiedVariables изменяемые переменные правой части
     * @param[in,out] varTable          таблица переменных
     * @param[in,out] errors            множество ошибок
     */
    void applyAction(Action* currentAction,
                     ExprNode* targetRoot,
                     const QList<ExprNode*>& sourceVariables,
                     const QList<ExprNode*>& modifiedVariables,
                     QMap<QString, Action*>& varTable,
                     QSet<Error>& errors);

    /**
     * @brief Добавляет вершину (действие) в граф
     * @param[in] action  действие
     */
    void addAction(Action* action);

    /**
     * @brief Добавляет ребро зависимости
     * @param[in] from  зависимое действие
     * @param[in] to    действие-источник
     * @param[in] type  тип зависимости
     */
    void addEdge(Action* from, Action* to, DependencyType type);

    bool validateArrayDimension(ExprNode* targetRoot,
                                const QString& varName,
                                const QMap<QString, Action*>& varTable,
                                int lineNumber,
                                Error& error);

    DependencyType determineDependency(ExprNode* varNode,
                                       const QMap<QString, Action*>& varTable,
                                       Action*& dependencyAction);

    QList<Action*> getIncoming(Action* action) const;

    QString toDOT() const;
};

#endif // DEPENDENCYGRAPH_H
