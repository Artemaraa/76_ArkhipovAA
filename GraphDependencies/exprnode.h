#ifndef EXPRNODE_H
#define EXPRNODE_H

#include <QString>

/**
 * @brief Тип узла дерева выражения (вид токена)
 *
 * Определяет, чем является узел: операндом (переменная/число) или операцией
 */
enum TokenType {
    Var,        // переменная
    Number,     // число
    Plus,       // +
    Minus,      // -
    Multiply,   // *
    Divide,     // /
    Increment,  // ++
    Decrement,  // --
    ArrayAccess // []
};

/**
 * @brief Узел дерева выражения.
 *
 * Из узлов строится дерево разбора постфиксного выражения. Операнды хранятся
 * в leftOperand/rightOperand: для бинарной операции заняты оба, для унарной -
 * только левый, для доступа к массиву левый - база массива, правый - индекс.
 */
class ExprNode
{
public:
    TokenType type;          ///< тип узла
    QString value;           ///< значение: имя переменной или текст числа
    ExprNode* leftOperand;   ///< левый потомок (база массива / левый операнд)
    ExprNode* rightOperand;  ///< правый потомок (индекс / правый операнд)

    /**
     * @brief Создаёт узел заданного типа.
     * @param[in] t  тип узла (вид токена)
     * @param[in] v  значение узла (имя переменной или текст числа)
     */
    ExprNode(TokenType t, const QString& v = "");

    /**
     * @brief Деструктор
     */
    ~ExprNode();
};

#endif // EXPRNODE_H
