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
 * @brief Создаёт фиктивный узел.
 * Используется, когда оператору не хватило операндов, чтобы не прерывать разбор
 * @return новый узел Number("0")
 */
ExprNode* makeStubNode();


/**
 * @brief Разбирает все строки трассы в список действий.
 * @param[in]     fileContent  строки файла
 * @param[out]    actions      список действий
 * @param[in,out] errors       множество ошибок
 */
void parseActions(const QStringList& fileContent, QList<Action*>& actions, QSet<Error>& errors);

/**
 * @brief Разбирает одну строку трассы в действие.
 * @param[in]     trimmedLine  строка без крайних пробелов
 * @param[in]     lineNumber   номер строки
 * @param[in,out] actions      список действий
 * @param[in,out] errors       множество ошибок
 */
void parseSingleLine(const QString& trimmedLine, int lineNumber,
                     QList<Action*>& actions, QSet<Error>& errors);

/**
 * @brief Строит дерево выражения из постфиксной строки.
 * @param[in]     exprStr     текст выражения
 * @param[out]    sources     прочитанные переменные
 * @param[out]    modified    изменяемые переменные
 * @param[in]     lineNumber  номер строки
 * @param[in,out] errors      множество ошибок
 * @return корень построенного дерева (или nullptr)
 */
ExprNode* parseExpression(const QString& exprStr,
                          QList<ExprNode*>& sources,
                          QList<ExprNode*>& modified,
                          int lineNumber,
                          QSet<Error>& errors);

/**
 * @brief Разбирает один токен и направляет его в нужный обработчик.
 * @param[in]     token       текст токена
 * @param[in,out] stack       стек узлов
 * @param[in]     lineNumber  номер строки
 * @param[in]     tokenIndex  номер токена
 * @param[in]     numRegex    шаблон целого числа
 * @param[in]     varRegex    шаблон имени переменной
 * @param[in,out] errors      множество ошибок
 */
void parseSingleToken(const QString& token, QStack<ExprNode*>& stack,
                      int lineNumber, int tokenIndex,
                      const QRegularExpression& numRegex,
                      const QRegularExpression& varRegex,
                      QSet<Error>& errors);

/**
 * @brief Возвращает имя переменной/массива по корню дерева.
 * @param[in] node  корень дерева
 * @return имя без индексов
 */
QString getArrayName(ExprNode* node);

/**
 * @brief Возвращает размерность цели
 * @param[in] node  корень дерева
 * @return 0 для скаляра, 1 и более для массива
 */
int getArrayDimension(ExprNode* node);

/**
 * @brief Обрабатывает бинарный оператор при разборе
 * @param[in]     token       текст оператора
 * @param[in,out] stack       стек узлов
 * @param[in]     lineNumber  номер строки
 * @param[in]     tokenIndex  номер токена
 * @param[in,out] errors      множество ошибок
 */
void handleBinaryOp(const QString& token, QStack<ExprNode*>& stack,
                    int lineNumber, int tokenIndex, QSet<Error>& errors);

/**
 * @brief Обрабатывает унарный оператор при разборе
 * @param[in]     token       текст оператора
 * @param[in,out] stack       стек узлов
 * @param[in]     lineNumber  номер строки
 * @param[in]     tokenIndex  номер токена
 * @param[in,out] errors      множество ошибок
 */
void handleUnaryOp(const QString& token, QStack<ExprNode*>& stack,
                   int lineNumber, int tokenIndex, QSet<Error>& errors);

/**
 * @brief Обрабатывает доступ к элементу массива при разборе
 * @param[in]     token       текст оператора
 * @param[in,out] stack       стек узлов
 * @param[in]     lineNumber  номер строки
 * @param[in]     tokenIndex  номер токена
 * @param[in,out] errors      множество ошибок
 */
void handleArrayAccess(const QString& token, QStack<ExprNode*>& stack,
                       int lineNumber, int tokenIndex, QSet<Error>& errors);

/**
 * @brief Собирает прочитанные переменные обходом готового дерева.
 * @param[in]  node     корень дерева или поддерева
 * @param[out] sources  список источников
 */
void collectSources(ExprNode* node, QList<ExprNode*>& sources);

/**
 * @brief Собирает переменные-индексы из базы массива.
 * @param[in]  node     узел базы массива
 * @param[out] sources  список источников
 */
void collectIndexVars(ExprNode* node, QList<ExprNode*>& sources);

/**
 * @brief Собирает переменные, изменяемые операторами ++/--.
 * @param[in]  node      корень дерева или поддерева
 * @param[out] modified  список изменяемых переменных
 */
void collectModified(ExprNode* node, QList<ExprNode*>& modified);

/**
 * @brief Сравнивает индексы двух деревьев и определяет тип зависимости.
 * @param[in] node1  первое дерево
 * @param[in] node2  второе дерево
 * @return Direct, General или NoDependency
 */
DependencyType compareIndices(ExprNode* node1, ExprNode* node2);


#endif // FUNCTIONS_H
