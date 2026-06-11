#ifndef ERROR_H
#define ERROR_H
#include <QString>
#include <QHash>

/**
 * @brief Типы ошибок, обнаруживаемых при разборе трассы и построении графа.
 *
 * Значение NoError используется как «ошибки нет» (по умолчанию). Остальные
 * значения соответствуют конкретным диагностикам этапов чтения, разбора и
 * построения графа.
 */
enum ErrorType {
    NoError,                  // ошибки нет (значение по умолчанию)
    InputFileNotFound,        // входной файл не открылся
    OutputFileCreateFail,     // не удалось создать выходной файл
    EmptyInputFile,           // входной файл пуст
    NoEqualSign,              // в строке нет знака '='
    EmptyLeftPart,            // пустая левая часть (до '=')
    EmptyExpression,          // пустая правая часть (после '=')
    InvalidVariableName,      // имя переменной не по формату
    VariableNameTooLong,      // имя длиннее 255 символов
    UnsupportedOperator,      // неизвестный токен/оператор
    NumberOutOfRange,         // константа вне диапазона int
    InvalidNumberFormat,      // константа не целая (напр. 3.14)
    NotEnoughOperands,        // оператору не хватило операндов в стеке
    UnaryOperandNotVariable,  // ++/-- применён не к переменной (напр. к числу)
    ExtraOperands,            // после разбора в стеке осталось > 1 узла
    UnclosedBracket,          // незакрытая '['
    ExtraClosingBracket,      // лишняя ']'
    MissingArrayIndex,        // отсутствует индекс массива
    ArrayWithoutIndex,        // массив использован без индекса
    ScalarWithIndex,          // скаляр использован с индексом
    InvalidArrayDimension,    // у массива изменилась размерность
    NestingDepthExceeded,     // глубина вложенности индексов > 5
    TooManyActions            // в трассе > 10 000 действий
};


/**
 * @brief Описание одной ошибки: тип, место (строка/столбец) и контекст
 */
class Error
{
public:
    ErrorType type;          ///< тип ошибки
    int lineNumber;          ///< номер строки трассы (0 - если неприменимо)
    int columnNumber;        ///< номер токена/столбца (0 - если неприменимо)
    QString errorContext;    ///< дополнительный контекст (имя переменной, токен)

    /**
     * @brief Конструктор по умолчанию создаёт ошибку (NoError).
     */
    Error();

    /**
     * @brief Создаёт ошибку с заданными параметрами.
     * @param[in] t        тип ошибки
     * @param[in] line     номер строки
     * @param[in] col      номер столбца/токена
     * @param[in] context  контекст (имя переменной, токен) (по умолчанию пустой)
     */
    Error(ErrorType t, int line, int col, const QString& context = "");

    /**
     * @brief Возвращает тип ошибки
     * @return тип ошибки
     */
    ErrorType getTypeError() const;

    /**
     * @brief Формирует текстовое сообщение об ошибке для вывода пользователю
     * @return строка с описанием ошибки
     */
    QString generateErrorMessage() const;
};

/**
 * @brief Сравнение двух ошибок на равенство (по всем полям).
 * @param[in] left   первая ошибка
 * @param[in] right  вторая ошибка
 * @return true, если все поля совпадают
 */
bool operator==(const Error& left, const Error& right);

/**
 * @brief Хеш-функция для Error.
 *
 * @param[in] key   ошибка, для которой вычисляется хеш
 * @param[in] seed  начальное значение хеша (по умолчанию 0)
 * @return хеш-код ошибки
 */
uint qHash(const Error& key, uint seed = 0);

#endif // ERROR_H
