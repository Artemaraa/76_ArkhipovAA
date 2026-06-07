#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QString>
#include <QList>
#include <QSet>

#include "exprnode.h"
#include "error.h"
#include "action.h"
#include "dependencyedge.h"

const int MAX_ACTIONS = 10000;
const int MAX_VAR_LEN = 255;
const int MAX_INDEX_DEPTH = 5;

ExprNode* parseExpression(const QString& exprStr,
                          QList<ExprNode*>& sources,
                          QList<ExprNode*>& modified,
                          int lineNumber,
                          QSet<Error>& errors);

DependencyType compareIndices(ExprNode* node1, ExprNode* node2);

#endif // FUNCTIONS_H
