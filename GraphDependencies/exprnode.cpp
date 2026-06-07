#include "exprnode.h"

ExprNode::ExprNode(TokenType t, const QString& v)
    : type(t), value(v), leftOperand(nullptr), rightOperand(nullptr)
{
}

ExprNode::~ExprNode()
{
    delete leftOperand;
    delete rightOperand;
}
