#ifndef EXPRNODE_H
#define EXPRNODE_H

#include <QString>

enum TokenType {
    Var, Number, Plus, Minus, Multiply, Divide, Increment, Decrement, ArrayAccess
};

class ExprNode
{
public:
    TokenType type;
    QString value;
    ExprNode* leftOperand;
    ExprNode* rightOperand;

    ExprNode(TokenType t, const QString& v = "");
    ~ExprNode();
};

#endif // EXPRNODE_H
