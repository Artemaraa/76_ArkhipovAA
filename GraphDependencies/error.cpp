#include "error.h"

// Конструктор по умолчанию: ошибка отсутствует, позиции обнулены
Error::Error()
    : type(NoError), lineNumber(0), columnNumber(0)
{
}

// Конструктор с параметрами: запоминаем тип, позицию и контекст ошибки
Error::Error(ErrorType t, int line, int col, const QString& context)
    : type(t), lineNumber(line), columnNumber(col), errorContext(context)
{
}

// Вернуть тип ошибки
ErrorType Error::getTypeError() const
{
    return type;
}

//  Формирование текстового сообщения об ошибке
QString Error::generateErrorMessage() const
{
    // Заготовки префиксов:"Строка N, столбец M"
    const QString line = "Строка " + QString::number(lineNumber);
    const QString lineCol = line + ", столбец " + QString::number(columnNumber);

    // Итоговое сообщение
    QString message;

    // Выбрать текст сообщения в зависимости от типа ошибки
    switch (type) {
    case InputFileNotFound:
        message = "Неверно указан файл с входными данными. Возможно, файл не существует";
        break;
    case OutputFileCreateFail:
        message = "Неверно указан файл для выходных данных. "
                  "Возможно, указанного расположения не существует или нет прав на запись.";
        break;
    case EmptyInputFile:
        message = "Ошибка: входной файл пуст";
        break;
    case NoEqualSign:
        message = line + ": отсутствует знак '='";
        break;
    case EmptyLeftPart:
        message = line + ": пустая левая часть";
        break;
    case EmptyExpression:
        message = line + ": пустое выражение";
        break;
    case InvalidVariableName:
        message = lineCol + ": имя переменной не соответствует формату";
        break;
    case VariableNameTooLong:
        message = line + ": имя переменной длиннее 255 символов";
        break;
    case UnsupportedOperator:
        message = lineCol + ": неизвестный токен";
        break;
    case NumberOutOfRange:
        message = lineCol + ": константа вне допустимого диапазона";
        break;
    case InvalidNumberFormat:
        message = lineCol + ": константа должна быть целым числом";
        break;
    case NotEnoughOperands:
        message = line + ": недостаточно операндов для оператора в столбце " + QString::number(columnNumber);
        break;
    case UnaryOperandNotVariable:
        message = line + ": операторы ++/-- применимы только к переменным";
        break;
    case ExtraOperands:
        message = line + ": лишние операнды в выражении";
        break;
    case UnclosedBracket:
        message = line + ": незакрытая скобка '['";
        break;
    case ExtraClosingBracket:
        message = line + ": лишняя закрывающая скобка ']'";
        break;
    case MissingArrayIndex:
        message = line + ": отсутствует индекс массива";
        break;
    case ArrayWithoutIndex:
        message = line + ": переменная '" + errorContext + "' (массив) используется без индекса";
        break;
    case ScalarWithIndex:
        message = line + ": переменная '" + errorContext + "' (скаляр) используется с индексом";
        break;
    case InvalidArrayDimension:
        message = line + ": неверная размерность массива '" + errorContext + "'";
        break;
    case NestingDepthExceeded:
        message = line + ": глубина вложенности индексов превышает 5";
        break;
    case TooManyActions:
        message = "Трасса содержит более 10 000 действий, что превышает допустимый лимит.";
        break;
    default:
        message = line + ": неизвестная ошибка";
        break;
    }
    // Вернуть сформированное сообщение
    return message;
}


// Сравнение двух ошибок: равны, если совпадают все поля
bool operator==(const Error& left, const Error& right)
{
    return left.type == right.type &&
           left.lineNumber == right.lineNumber &&
           left.columnNumber == right.columnNumber &&
           left.errorContext == right.errorContext;
}

// Хеш ошибки: объединяем хеши всех полей через XOR
uint qHash(const Error& key, uint seed)
{
    return qHash(key.type, seed) ^
           qHash(key.lineNumber, seed) ^
           qHash(key.columnNumber, seed) ^
           qHash(key.errorContext, seed);
}
