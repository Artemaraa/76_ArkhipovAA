#ifndef ERROR_H
#define ERROR_H
#include <QString>
#include <QHash>

enum ErrorType {
    NoError, InputFileNotFound, OutputFileCreateFail, EmptyInputFile,
    NoEqualSign, EmptyLeftPart, EmptyExpression, InvalidVariableName,
    VariableNameTooLong, UnsupportedOperator, NumberOutOfRange,
    InvalidNumberFormat, NotEnoughOperands, UnaryOperandNotVariable,
    ExtraOperands, UnclosedBracket, ExtraClosingBracket, MissingArrayIndex,
    ArrayWithoutIndex, ScalarWithIndex, InvalidArrayDimension,
    NestingDepthExceeded, TooManyActions
};

class Error
{
public:
    ErrorType type;
    int lineNumber;
    int columnNumber;
    QString errorContext;

    Error();
    Error(ErrorType t, int line, int col, const QString& context = "");
    ErrorType getTypeError() const;
    QString generateErrorMessage() const;
};

bool operator==(const Error& left, const Error& right);
uint qHash(const Error& key, uint seed = 0);

#endif // ERROR_H
