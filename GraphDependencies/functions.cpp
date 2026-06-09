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

// Разбор одной строки трассы в действие
void parseSingleLine(const QString& trimmedLine, int lineNum, QList<Action*>& actions, QSet<Error>& errors)
{
    // Найти знак равенства
    const int eqPos = trimmedLine.indexOf('=');
    // Если знака нет, добавить ошибку и выйти
    if (eqPos == -1) {
        errors.insert(Error(NoEqualSign, lineNum, 0, trimmedLine));
        return;
    }

    // Разделить строку на части: левую (до равно) и правую, убрать пробелы по краям
    const QString leftPart  = trimmedLine.left(eqPos).trimmed();
    const QString rightPart = trimmedLine.mid(eqPos + 1).trimmed();

    bool hasValidationError = false;
    // Если левая часть пуста, добавить ошибку
    if (leftPart.isEmpty()) {
        errors.insert(Error(EmptyLeftPart, lineNum, 0, ""));
        hasValidationError = true;
    }
    // Если правая часть пуста, добавить ошибку
    if (rightPart.isEmpty()) {
        errors.insert(Error(EmptyExpression, lineNum, 0, ""));
        hasValidationError = true;
    }
    // При любой из этих ошибок действие не создаётся, выйти
    if (hasValidationError) {
        return;
    }

    // Создать действие с номером строки
    Action* action = new Action(lineNum);
    // Сохранить исходный текст строки
    action->originalLine = trimmedLine;

    // Разобрать левую часть в дерево (targetRoot)(списки источников/изменяемых для неё не нужны)
    QList<ExprNode*> leftSources;
    QList<ExprNode*> leftModified;
    action->targetRoot = parseExpression(leftPart, leftSources, leftModified, lineNum, errors);

    // Разобрать правую часть в дерево (expression), передав списки источников и изменяемых действия
    action->expression = parseExpression(rightPart, action->sourceVariables, action->modifiedVariables, lineNum, errors);

    // Если оба дерева построены, добавить действие в список, иначе удалить действие
    if (action->targetRoot != nullptr && action->expression != nullptr) {
        actions.append(action);
    } else {
        delete action;
    }
}

// Построение дерева выражения из постфиксной формы
ExprNode* parseExpression(const QString& exprStr,
                          QList<ExprNode*>& sources,
                          QList<ExprNode*>& modified,
                          int lineNumber,
                          QSet<Error>& errors)
{
    // Разбить текст по пробелам
    const QStringList tokens = exprStr.split(' ', Qt::SkipEmptyParts);
    // Если токенов нет, вернуть пустой указатель
    if (tokens.isEmpty()){
        return nullptr;
    }
    // Шаблон целого числа и имени переменной
    const QRegularExpression numR("^-?\\d+$");
    const QRegularExpression varR("^[a-zA-Z][a-zA-Z0-9]*$");
    // Завести пустой стек узлов
    QStack<ExprNode*> stack;
    // Каждый токен разобрать
    for (int i = 0; i < tokens.size(); ++i) {
        parseSingleToken(tokens[i], stack, lineNumber, i + 1, numR, varR, errors);
    }
    // Проверить стек
    ExprNode* result = nullptr;
    if (stack.size() == 1) {
        // Если остался ровно один узел - это корень дерева
        result = stack.pop();
    } else {
        // Если узлов больше одного - добавить ошибку
        errors.insert(Error(ExtraOperands, lineNumber, 0, ""));
        // Взять нижний узел как корень
        result = stack.first();
        // Остальные деревья освободить
        for (ExprNode* node : stack) {
            if (node != result) {
                delete node;
            }
        }
    }
    // Собрать прочитанные и изменяемые переменные
    collectSources(result, sources);
    collectModified(result, modified);
    // Вернуть корень дерева
    return result;
}

void parseSingleToken(const QString& token, QStack<ExprNode*>& stack,
                      int lineNumber, int tokenIndex,
                      const QRegularExpression& numRegex,
                      const QRegularExpression& varRegex,
                      QSet<Error>& errors)
{
    Q_UNUSED(token);
    Q_UNUSED(stack);
    Q_UNUSED(lineNumber);
    Q_UNUSED(tokenIndex);
    Q_UNUSED(numRegex);
    Q_UNUSED(varRegex);
    Q_UNUSED(errors);
}

void collectSources(ExprNode* node, QList<ExprNode*>& sources)
{
    Q_UNUSED(node);
    Q_UNUSED(sources);
}

void collectModified(ExprNode* node, QList<ExprNode*>& modified)
{
    Q_UNUSED(node);
    Q_UNUSED(modified);
}

DependencyType compareIndices(ExprNode* node1, ExprNode* node2)
{
    Q_UNUSED(node1);
    Q_UNUSED(node2);
    return NoDependency;
}

