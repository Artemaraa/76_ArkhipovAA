#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <QStack>
#include <QRegularExpression>
#include <QString>
#include <QList>
#include <QSet>
#include <QStringList>

#include "exprnode.h"
#include "error.h"
#include "action.h"
#include "dependencyedge.h"

const int MAX_ACTIONS = 10000;
const int MAX_VAR_LEN = 255;
const int MAX_INDEX_DEPTH = 5;


/**
 * @brief Разбирает все строки трассы в список действий.
 * @param[in]     fileContent  строки файла
 * @param[out]    actions      список действий
 * @param[in,out] errors       множество ошибок
 */
void parseActions(const QStringList& fileContent, QList<Action*>& actions, QSet<Error>& errors);

void parseSingleLine(const QString& trimmedLine, int lineNumber,
                     QList<Action*>& actions, QSet<Error>& errors);

ExprNode* parseExpression(const QString& exprStr,
                          QList<ExprNode*>& sources,
                          QList<ExprNode*>& modified,
                          int lineNumber,
                          QSet<Error>& errors);

DependencyType compareIndices(ExprNode* node1, ExprNode* node2);



#endif // FUNCTIONS_H
