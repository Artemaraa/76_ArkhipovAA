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

// узл-заглушка
ExprNode* makeStubNode()
{
    // Создать и вернуть узел типа Number со значением 0
    return new ExprNode(Number, "0");
}

// Разбор одного токена
void parseSingleToken(const QString& token,
                      QStack<ExprNode*>& stack,
                      int lineNumber,
                      int tokenIndex,
                      const QRegularExpression& numRegex,
                      const QRegularExpression& varRegex,
                      QSet<Error>& errors)
{
    // Если токен - бинарный оператор, передать в обработчик бинарного оператора
    if (token == "+" || token == "-" || token == "*" || token == "/") {
        handleBinaryOp(token, stack, lineNumber, tokenIndex, errors);
    }
    // Иначе если токен - унарный оператор, передать в обработчик унарного оператора
    else if (token == "++" || token == "--") {
        handleUnaryOp(token, stack, lineNumber, tokenIndex, errors);
    }
    // Иначе если токен - доступ к элементу массива, передать в обработчик доступа к массиву
    else if (token == "[]") {
        handleArrayAccess(token, stack, lineNumber, tokenIndex, errors);
    }
    // Иначе если токен подходит под шаблон целого числа
    else if (numRegex.match(token).hasMatch()) {
        // Проверить, помещается ли число в диапазон
        bool ok = false;
        token.toInt(&ok);
        if (!ok) {
            // Если не помещается, добавить ошибку и положить фиктивный узел
            errors.insert(Error(NumberOutOfRange, lineNumber, tokenIndex, token));
            stack.push(makeStubNode());
        } else {
            // Иначе создать узел-число и положить в стек
            stack.push(new ExprNode(Number, token));
        }
    }
    // Иначе если токен подходит под шаблон имени переменной
    else if (varRegex.match(token).hasMatch()) {
        // Если имя длиннее 255 символов, добавить ошибку и положить фиктивный узел
        if (token.length() > MAX_VAR_LEN) {
            errors.insert(Error(VariableNameTooLong, lineNumber, tokenIndex, token));
            stack.push(makeStubNode());
        } else {
            // Иначе создать узел-переменную и положить в стек
            stack.push(new ExprNode(Var, token));
        }
    }
    // Иначе токен некорректен, определить точный вид ошибки
    else {
        if (!token.isEmpty()) {
            const QChar firstChar = token[0];
            // Определить, похож ли токен на число
            const bool looksNumeric = firstChar.isDigit() ||
                                      (token.size() > 1 && firstChar == '-' && token[1].isDigit());

            if (looksNumeric) {
                // Если токен состоит только из цифр, точки и минуса, то это неферный формат числа
                const QString numericChars = "0123456789.-";
                bool onlyNumericChars = true;
                for (const QChar c : token) {
                    if (!numericChars.contains(c)) {
                        onlyNumericChars = false;
                        break;
                    }
                }
                if (onlyNumericChars) {
                    // Похоже на число, но не целое
                    errors.insert(Error(InvalidNumberFormat, lineNumber, tokenIndex, token));
                } else {
                    // Имя с недопустимыми символами
                    errors.insert(Error(InvalidVariableName, lineNumber, tokenIndex, token));
                }
            } else {
                // Иначе это неизвестный оператор/символ
                errors.insert(Error(UnsupportedOperator, lineNumber, tokenIndex, token));
            }
        } else {
            errors.insert(Error(UnsupportedOperator, lineNumber, tokenIndex, token));
        }
        // Положить фиктивный узел
        stack.push(makeStubNode());
    }
}

// Имя переменной/массива
QString getArrayName(ExprNode* node)
{
    QString name;
    // Если узел пуст, вернуть пустую строку
    if (node) {
        if (node->type == Var) {
            // Если узел = переменная, вернуть её имя
            name = node->value;
        } else if (node->type == ArrayAccess) {
            // Если узел + доступ к массиву, рекурсивно взять имя у левого операнда
            name = getArrayName(node->leftOperand);
        }
    }
    // Вернуть имя
    return name;
}

// Размерность массива
int getArrayDimension(ExprNode* node)
{
    // Если узел пуст или не массив,то 0
    int dimension = 0;
    if (node && node->type == ArrayAccess) {
        // Каждый уровень [] добавляет 1
        dimension = 1 + getArrayDimension(node->leftOperand);
    }
    // Вернуть размерность
    return dimension;
}

void handleBinaryOp(const QString& token, QStack<ExprNode*>& stack,
                    int lineNumber, int tokenIndex, QSet<Error>& errors)
{
    Q_UNUSED(token);
    Q_UNUSED(stack);
    Q_UNUSED(lineNumber);
    Q_UNUSED(tokenIndex);
    Q_UNUSED(errors);
}

void handleUnaryOp(const QString& token, QStack<ExprNode*>& stack,
                   int lineNumber, int tokenIndex, QSet<Error>& errors)
{
    Q_UNUSED(token);
    Q_UNUSED(stack);
    Q_UNUSED(lineNumber);
    Q_UNUSED(tokenIndex);
    Q_UNUSED(errors);
}

void handleArrayAccess(const QString& token, QStack<ExprNode*>& stack,
                       int lineNumber, int tokenIndex, QSet<Error>& errors)
{
    Q_UNUSED(token);
    Q_UNUSED(stack);
    Q_UNUSED(lineNumber);
    Q_UNUSED(tokenIndex);
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

