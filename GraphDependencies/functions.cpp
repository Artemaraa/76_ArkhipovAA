#include "functions.h"

// Разбор всех строк
void parseActions(const QStringList& fileContent, QList<Action*>& actions, QSet<Error>& errors)
{
    // Обнулить счетчик строк
    int lineNum = 0;
    // Для каждой строки
    for (int i = 0; i < fileContent.size(); i++){
        // Увеличить счётчик строк
        lineNum++;
        // Убрать пробелы по краям
        const QString trimmedLine = fileContent[i].trimmed();
        // Если строка непустая - передать в разбор одной строки
        if (!trimmedLine.isEmpty()) {
            parseSingleLine(trimmedLine, lineNum, actions, errors);
        }
    }
    // Если количество действий превысило лимит => добавить ошибку
    if (actions.size() > MAX_ACTIONS) {
        errors.insert(Error(TooManyActions, 0, 0, ""));
    }

}
void parseSingleLine(const QString& trimmedLine, int lineNumber,
                     QList<Action*>& actions, QSet<Error>& errors)
{
    Q_UNUSED(trimmedLine);
    Q_UNUSED(lineNumber);
    Q_UNUSED(actions);
    Q_UNUSED(errors);
}
ExprNode* parseExpression(const QString& exprStr,
                          QList<ExprNode*>& sources,
                          QList<ExprNode*>& modified,
                          int lineNumber,
                          QSet<Error>& errors)
{
    Q_UNUSED(exprStr);
    Q_UNUSED(lineNumber);
    Q_UNUSED(sources);
    Q_UNUSED(errors);
    Q_UNUSED(modified);
    return nullptr;
}


DependencyType compareIndices(ExprNode* node1, ExprNode* node2)
{
    Q_UNUSED(node1);
    Q_UNUSED(node2);
    return NoDependency;
}

