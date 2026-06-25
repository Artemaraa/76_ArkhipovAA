#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H
/**
 * @file dependencygraph.h
 * @brief Класс графа зависимостей действий
 *
 * Строит граф зависимостей по списку действий, хранит вершины и рёбра,
 * определяет типы зависимостей и формирует вывод в формате DOT.
 */
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
    * @brief Применяет одно действие: проверяет размерности, строит рёбра и обновляет таблицу переменных
    * @param[in]     currentAction текущее действие (данные берутся из его полей)
    * @param[in,out] varTable      таблица переменных (последнее изменившее действие)
    * @param[in,out] errors        множество ошибок
    * @param[in,out] dims          таблица размерностей переменных (0 = скаляр, ≥1 = массив)
    */
    void applyAction(Action* currentAction,
                     QMap<QString, Action*>& varTable,
                     QSet<Error>& errors,
                     QMap<QString, int>& dims);

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

    /**
     * @brief Проверяет и фиксирует размерность одной переменной по контексту размерностей
     * @param[in]     node        узел переменной (цель или источник)
     * @param[in]     varName     имя переменной
     * @param[in,out] dims        таблица размерностей (0 = скаляр, ≥1 = массив)
     * @param[in]     lineNumber  номер строки
     * @param[out]    error       заполняется при несоответствии размерности
     * @return true - корректно (или имя зафиксировано впервые), false - несоответствие
     */
    bool validateArrayDimension(ExprNode* node,
                                const QString& varName,
                                QMap<QString, int>& dims,
                                int lineNumber,
                                Error& error);
    /**
     * @brief Определяет зависимость одной прочитанной переменной от предыдущего действия
     * @param[in]  varNode          узел прочитанной переменной
     * @param[in]  varTable         таблица переменных
     * @param[out] dependencyAction найденное действие-источник (или nullptr)
     * @return тип зависимости
     */
    DependencyType determineDependency(ExprNode* varNode,
                                       const QMap<QString, Action*>& varTable,
                                       Action*& dependencyAction);

    /**
     * @brief Возвращает действия, на которые влияет заданное (зависят от него)
     * @param[in] action  действие
     * @return список зависимых действий
     */
    QList<Action*> getOutgoing(Action* action) const;

    /**
     * @brief Возвращает действия, от которых зависит заданное (его источники)
     * @param[in] action  действие
     * @return список действий-источников
     */
    QList<Action*> getIncoming(Action* action) const;

    /**
     * @brief Формирует текст графа в формате DOT
     * @return строка DOT
     */
    QString toDOT() const;

    /**
     * @brief Проверяет согласованность размерностей всех переменных действия
     * @param[in]     currentAction  текущее действие
     * @param[in,out] dims           таблица зафиксированных размерностей переменных
     * @param[in,out] errors         множество ошибок
     * @return true - обнаружена ошибка размерности, иначе false
     */
    bool checkActionDimensions(Action* currentAction,
                                      QMap<QString, int>& dims,
                                      QSet<Error>& errors);

    /**
     * @brief Строит рёбра зависимости для прочитанных переменных действия
     * @param[in] currentAction    текущее действие
     * @param[in] sourceVariables  прочитанные переменные правой части
     * @param[in] varTable         таблица переменных (последнее изменившее действие)
     */
    void buildEdgesForAction(Action* currentAction,
                             const QList<ExprNode*>& sourceVariables,
                             const QMap<QString, Action*>& varTable);

    /**
     * @brief Обновляет таблицу последних изменений переменных текущим действием
     * @param[in]     currentAction      текущее действие
     * @param[in]     targetName         имя целевой переменной
     * @param[in]     modifiedVariables  переменные, изменяемые операторами ++/--
     * @param[in,out] varTable           таблица переменных
     */
    void updateVarTable(Action* currentAction,
                               const QString& targetName,
                               const QList<ExprNode*>& modifiedVariables,
                               QMap<QString, Action*>& varTable);

};

#endif // DEPENDENCYGRAPH_H
