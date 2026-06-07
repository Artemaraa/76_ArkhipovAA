#include "error.h"

Error::Error()
    : type(NoError), lineNumber(0), columnNumber(0)
{
}

Error::Error(ErrorType t, int line, int col, const QString& context)
    : type(t), lineNumber(line), columnNumber(col), errorContext(context)
{
}

ErrorType Error::getTypeError() const
{
    return type;
}

QString Error::generateErrorMessage() const
{
    return QString();
}

bool operator==(const Error& left, const Error& right)
{
    return left.type == right.type &&
           left.lineNumber == right.lineNumber &&
           left.columnNumber == right.columnNumber &&
           left.errorContext == right.errorContext;
}

uint qHash(const Error& key, uint seed)
{
    return qHash(key.type, seed) ^ qHash(key.lineNumber, seed) ^
           qHash(key.columnNumber, seed) ^ qHash(key.errorContext, seed);
}
