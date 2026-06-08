#include "functions.h"


ExprNode* parseExpression(const QString& exprStr,
                          QList<ExprNode*>& sources,
                          QList<ExprNode*>& modified,
                          int lineNumber,
                          QSet<Error>& errors)
{
    return nullptr;
}


DependencyType compareIndices(ExprNode* node1, ExprNode* node2)
{
    return NoDependency;
}

void parseActions(const QStringList& fileContent,
                  QList<Action*>& actions,
                  QSet<Error>& errors)
{
    Q_UNUSED(fileContent);
    Q_UNUSED(actions);
    Q_UNUSED(errors);
}
