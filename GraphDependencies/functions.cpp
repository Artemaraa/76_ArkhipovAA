#include "functions.h"
#include <QFile>
#include <QTextStream>

// Разбор всех строк
void parseActions(const QStringList& fileContent, QList<Action*>& actions, QSet<Error>& errors)
{
    // Проверка на пустоту файла: есть ли хоть одна непустая (после обрезки) строка
    bool hasContent = false;
    for (int i = 0; i < fileContent.size(); i++) {
        if (!fileContent[i].trimmed().isEmpty()) {
            hasContent = true;
            break;
        }
    }
    // Если содержимого нет - добавить ошибку и выйти
    if (!hasContent) {
        errors.insert(Error(EmptyInputFile, 0, 0, ""));
        return;
    }

    // Обнулить счетчик строк
    int lineNum = 0;
    // Для каждой строки
    for (int i = 0; i < fileContent.size(); i++){
        // Увеличить счётчик строк
        lineNum++;
        // Если строка непустая - передать в разбор одной строки
        if (!fileContent[i].trimmed().isEmpty()) {
            // Передать строку в разбор одной строки (обрезку делает parseSingleLine)
            parseSingleLine(fileContent[i], lineNum, actions, errors);
        }
    }
    // Если количество действий превысило лимит => добавить ошибку
    if (actions.size() > MAX_ACTIONS) {
        errors.insert(Error(TooManyActions, 0, 0, ""));
    }

}

// Разбор одной строки трассы в действие
void parseSingleLine(const QString& rawLine, int lineNumber, QList<Action*>& actions, QSet<Error>& errors)
{
    // Обрезать пробелы по краям
    const QString trimmedLine = rawLine.trimmed();
    // Найти знак равенства
    const int eqPos = trimmedLine.indexOf('=');
    // Если знака нет, добавить ошибку и выйти
    if (eqPos == -1) {
        errors.insert(Error(NoEqualSign, lineNumber, 0, trimmedLine));
        return;
    }

    // Разделить строку на части: левую (до равно) и правую, убрать пробелы по краям
    const QString leftPart  = trimmedLine.left(eqPos).trimmed();
    const QString rightPart = trimmedLine.mid(eqPos + 1).trimmed();

    bool hasValidationError = false;
    // Если левая часть пуста, добавить ошибку
    if (leftPart.isEmpty()) {
        errors.insert(Error(EmptyLeftPart, lineNumber, 0, ""));
        hasValidationError = true;
    }
    // Если правая часть пуста, добавить ошибку
    if (rightPart.isEmpty()) {
        errors.insert(Error(EmptyExpression, lineNumber, 0, ""));
        hasValidationError = true;
    }
    // При любой из этих ошибок действие не создаётся, выйти
    if (hasValidationError) {
        return;
    }

    // Создать действие с номером строки
    Action* action = new Action(lineNumber);
    // Сохранить исходный текст строки
    action->originalLine = trimmedLine;

    // Разобрать левую часть в дерево (targetRoot)(списки источников/изменяемых для неё не нужны)
    QList<ExprNode*> leftSources;
    QList<ExprNode*> leftModified;
    action->targetRoot = parseExpression(leftPart, leftSources, leftModified, lineNumber, errors);

    // Разобрать правую часть в дерево (expression), передав списки источников и изменяемых действия
    action->expression = parseExpression(rightPart, action->sourceVariables, action->modifiedVariables, lineNumber, errors);

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

// Обработчик бинарного оператора
void handleBinaryOp(const QString& token, QStack<ExprNode*>& stack, int lineNumber, int tokenIndex, QSet<Error>& errors)
{
    // Если в стеке меньше двух узлов, добавить ошибку
    if (stack.size() < 2) {
        errors.insert(Error(NotEnoughOperands, lineNumber, tokenIndex, token));
        // Положить заглушку и выйти
        stack.push(makeStubNode());
        return;
    }
    // Снять с вершины правый операнд
    ExprNode* right = stack.pop();
    // Снять с вершины левый операнд
    ExprNode* left  = stack.pop();
    // Определить операцию
    TokenType opType = Plus;
    if (token == "-") opType = Minus;
    else if (token == "*") opType = Multiply;
    else if (token == "/") opType = Divide;
    // Создать узел операции
    ExprNode* opNode = new ExprNode(opType);
    // Поместить левый и правый операнды
    opNode->leftOperand  = left;
    opNode->rightOperand = right;
    // Вернуть в стек
    stack.push(opNode);
}

// Обработчик унарного оператора
void handleUnaryOp(const QString& token, QStack<ExprNode*>& stack, int lineNumber, int tokenIndex, QSet<Error>& errors)
{
    // Если стек пуст, добавить ошибку
    if (stack.size() < 1) {
        errors.insert(Error(NotEnoughOperands, lineNumber, tokenIndex, token));
        // Положить заглушку и выйти
        stack.push(makeStubNode());
        return;
    }
    // Снять операнд с вершины стека
    ExprNode* operand = stack.pop();
    // Если операнд = не переменная, добавить ошибку
    if (operand->type != Var) {
        errors.insert(Error(UnaryOperandNotVariable, lineNumber, tokenIndex, token));
    }
    // Определить операцию
    TokenType opType = (token == "++") ? Increment : Decrement;
    // Создать узел
    ExprNode* opNode = new ExprNode(opType);
    // Поместить операнд левым потомком
    opNode->leftOperand = operand;
    // Положить в стек
    stack.push(opNode);
}

// Обработчик доступа к элементу массива
void handleArrayAccess(const QString& token, QStack<ExprNode*>& stack, int lineNumber, int tokenIndex, QSet<Error>& errors)
{
    // Если в стеке меньше двух узлов
    if (stack.size() < 2) {
        // Добавить ошибку
        errors.insert(Error(NotEnoughOperands, lineNumber, tokenIndex, token));
        // Положить две заглушки и выйти
        stack.push(makeStubNode());
        stack.push(makeStubNode());
        return;
    }

    // Снять с вершины индекс
    ExprNode* index = stack.pop();
    // Снять с вершины массив
    ExprNode* array = stack.pop();
    // Создать узел доступа к массиву
    ExprNode* arrNode = new ExprNode(ArrayAccess);
    // левый потомок = массив
    arrNode->leftOperand  = array;
    // правый = индекс
    arrNode->rightOperand = index;
    // Положить в стек
    stack.push(arrNode);

    // Если глубина вложенности > 5,добавить ошибку
    if (getArrayDimension(arrNode) > MAX_INDEX_DEPTH) {
        errors.insert(Error(NestingDepthExceeded, lineNumber, tokenIndex, getArrayName(arrNode)));
    }
}

// Сбор переменных-индексов из базы массива
void collectIndexVars(ExprNode* node, QList<ExprNode*>& sources)
{
    // Если узел пуст, ничего не делать
    if (!node) return;
    // Если узел = доступ к массиву
    if (node->type == ArrayAccess) {
        // Спуститься по базе тем же сбором индексов
        collectIndexVars(node->leftOperand, sources);
        // Индекс этого уровня собрать обычным сбором источников
        collectSources(node->rightOperand, sources);
    }
}

// Сбор прочитанных переменных
void collectSources(ExprNode* node, QList<ExprNode*>& sources)
{
    // Если узел пуст, выйти
    if (!node) return;

    switch (node->type) {
    case Var:
        // Если узел = переменная, добавить её в список источников
        sources.append(node);
        break;
    case Number:
        // Если узел = число, ничего не добавлять
        break;
    case ArrayAccess:
        // Если узел = доступ к массиву: добавить сам узел (целый элемент a[2], a[i][j])
        sources.append(node);
        // Собрать переменные-индексы из базы массива
        collectIndexVars(node->leftOperand, sources);
        // Собрать переменные из индекса текущего измерения
        collectSources(node->rightOperand, sources);
        break;
    default: // + - * / ++ --
        // Для прочих узлов (операторы) - рекурсивно обойти левого и правого потомка
        collectSources(node->leftOperand, sources);
        collectSources(node->rightOperand, sources);
    }
}

// Сбор изменяемых переменных
void collectModified(ExprNode* node, QList<ExprNode*>& modified)
{
    // Если узел пуст, выйти
    if (!node) return;
    // Если узел = инкремент или декремент и его операнд = переменная
    if ((node->type == Increment || node->type == Decrement) &&
        node->leftOperand && node->leftOperand->type == Var) {
        // Добавить её в список изменяемых
        modified.append(node->leftOperand);
    }
    // Рекурсивно обойти оба поддерева
    collectModified(node->leftOperand, modified);
    collectModified(node->rightOperand, modified);
}

// Сравнение индексов
DependencyType compareIndices(ExprNode* node1, ExprNode* node2)
{
    DependencyType result;

    if (!node1 && !node2) {
        // Если оба узла пусты, вернуть Direct
        result = Direct;
    } else if (!node1 || !node2) {
        // Если пуст ровно один, вернуть General
        result = General;
    } else if (node1->type != node2->type) {
        // Если типы узлов различаются, вернуть General
        result = General;
    } else if (node1->type == Var) {
        // Если узлы = переменные: имена равны - Direct, иначе General
        result = (node1->value == node2->value) ? Direct : General;
    } else if (node1->type == Number) {
        // Если узлы = числа: значения равны - Direct, иначе NoDependency
        result = (node1->value == node2->value) ? Direct : NoDependency;
    } else if (node1->type == ArrayAccess) {
        // Если узлы - доступ к массиву: сравнить базы и индексы рекурсивно
        const DependencyType leftResult  = compareIndices(node1->leftOperand,  node2->leftOperand);
        const DependencyType rightResult = compareIndices(node1->rightOperand, node2->rightOperand);
        // Если хотя бы один результат NoDependency - вернуть NoDependency
        if (leftResult == NoDependency || rightResult == NoDependency) {
            result = NoDependency;
            // Иначе если хотя бы один General - вернуть General
        } else if (leftResult == General || rightResult == General) {
            result = General;
            // Иначе - вернуть Direct
        } else {
            result = Direct;
        }
    } else {
        result = General;
    }

    return result;
}

// Чтение файла
bool readFile(const QString& filePath, QStringList& fileContent, QSet<Error>& errors)
{
    // Открыть файл
    QFile file(filePath);
    // Если файл не открылся - добавить ошибку и выйти
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errors.insert(Error(InputFileNotFound, 0, 0, filePath));
        return false;
    }
    // Прочитать все строки из файла и добавить их в список строк
    QTextStream in(&file);
    while (!in.atEnd()) {
        fileContent.append(in.readLine());
    }
    // Закрыть файл и выйти
    file.close();
    return true;
}

// Проверка согласованности размерностей переменных
void checkDimensions(const QList<Action*>& actions, QSet<Error>& errors)
{
    // Таблица:зафиксированная размерность (0 = скаляр, >=1 = массив)
    QMap<QString, int> dims;
    // Пройти по всем действиям по порядку
    for (Action* action : actions) {
        if (!action) continue;
        // Собрать все узлы-переменные действия: левая часть (цель) + правая часть
        QList<ExprNode*> vars;
        collectSources(action->targetRoot, vars);   // переменные левой части
        vars.append(action->sourceVariables);       // переменные правой части (уже собраны)
        // Проверить каждую переменную
        for (ExprNode* node : vars) {
            const QString name = getArrayName(node);
            if (name.isEmpty()) continue;
            const int dim = getArrayDimension(node);
            if (!dims.contains(name)) {
                // Первое появление имени фиксирует его размерность
                dims[name] = dim;
            } else if (dims[name] != dim) {
                // Размерность не совпала с зафиксированной - выбрать точный тип ошибки
                const int saved = dims[name];
                ErrorType t;
                if (saved == 0 && dim > 0) {
                    t = ScalarWithIndex;       // был скаляр, используется с индексом
                } else if (saved > 0 && dim == 0) {
                    t = ArrayWithoutIndex;     // был массив, используется без индекса
                } else {
                    t = InvalidArrayDimension; // другое число измерений
                }
                errors.insert(Error(t, action->number, 0, name));
            }
        }
    }
}
