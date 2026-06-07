#ifndef ACTION_H
#define ACTION_H

#include <QString>
#include <QList>

class ExprNode;

class Action
{
public:
    int number;
    QString originalLine;
    ExprNode* targetRoot;
    ExprNode* expression;
    QList<ExprNode*> sourceVariables;
    QList<ExprNode*> modifiedVariables;

    Action(int num);
    ~Action();
};

#endif // ACTION_H
