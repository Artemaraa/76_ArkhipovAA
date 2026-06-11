#ifndef ACTION_H
#define ACTION_H

#include <QString>
#include <QList>

class ExprNode;

/**
 * @brief Одно действие трассы вида "<цель> = <выражение>"
 *
 * Хранит номер действия, исходный текст строки, деревья левой (цель) и правой
 * (выражение) частей, а также списки переменных правой части: прочитанных
 * (sourceVariables) и изменяемых операторами ++/-- (modifiedVariables).
 * Действие владеет деревьями targetRoot и expression и удаляет их в деструкторе.
 * Узлы в списках переменных - не владеющие указатели (принадлежат деревьям).
 */
class Action
{
public:
    int number;                            ///< номер действия (равен номеру строки)
    QString originalLine;                  ///< исходный текст строки трассы
    ExprNode* targetRoot;                  ///< дерево левой части (цель присваивания)
    ExprNode* expression;                  ///< дерево правой части (выражение)
    QList<ExprNode*> sourceVariables;      ///< прочитанные переменные правой части
    QList<ExprNode*> modifiedVariables;    ///< переменные, изменяемые через ++/--

    /**
     * @brief Создаёт действие с заданным номером.
     * @param[in] num  номер действия (номер строки трассы)
     */
    Action(int num);

    /**
     * @brief Деструктор: удаляет деревья левой и правой частей.
     */
    ~Action();
};

#endif // ACTION_H
