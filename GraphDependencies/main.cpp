/**
 * @mainpage Построение графа зависимостей действий
 *
 * @section intro Назначение
 * Программа строит ориентированный граф непосредственных зависимостей
 * действий по заданной трассе выполнения в постфиксной записи и выводит
 * результат в формате DOT (Graphviz).
 *
 * Действие зависит от другого, если использует значение переменной
 * (или элемента массива), изменённое последним предшествующим действием.
 * Зависимости по массивам бывают прямыми (точное совпадение индексов)
 * и общими (индекс задан переменной, показаны пунктиром).
 *
 * @section usage Запуск
 * @code
 * GraphDependencies.exe input.txt output.dot
 * @endcode
 *
 * @section example Пример работы
 *
 * Входной файл:
 * @code
 * a = 5
 * b = a 1 +
 * c = a b *
 * n i [] = a
 * v = n i [] b +
 * f = n 3 [] a -
 * k = a ++
 * privet = a k -
 * @endcode
 *
 * Выходной файл (DOT):
 * @code
 * digraph G {
 *     1 [label="a = 5"];
 *     2 [label="b = a 1 +"];
 *     3 [label="c = a b *"];
 *     4 [label="n i [] = a"];
 *     5 [label="v = n i [] b +"];
 *     6 [label="f = n 3 [] a -"];
 *     7 [label="k = a ++"];
 *     8 [label="privet = a k -"];
 *     2 -> 1;
 *     3 -> 1;
 *     3 -> 2;
 *     4 -> 1;
 *     5 -> 4;
 *     5 -> 2;
 *     6 -> 4 [style=dashed];
 *     6 -> 1;
 *     7 -> 1;
 *     8 -> 7;
 * }
 * @endcode
 *
 * Визуализация полученного графа:
 * @image html dot.png "Граф зависимостей для примера"
 */
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSet>

#include "dependencygraph.h"
#include "functions.h"

// Печатает все накопленные ошибки в консоль
void printErrors(const QSet<Error>& errors)
{
    // Для каждой ошибки из множества вывести её текстовое сообщение
    for (const Error& err : errors) {
        qDebug().noquote() << err.generateErrorMessage();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Проверить количество аргументов: путь к входному и выходному файлам
    if (argc < 3) {
        qDebug().noquote() << "Usage: GraphDependencies.exe <input.txt> <output.dot>";
        return 1;
    }

    const QString inputFile  = argv[1];
    const QString outputFile = argv[2];

    QStringList fileContent; // строки входного файла
    QSet<Error> errors; // множество ошибок

    // Прочитать входной файл; при ошибке чтения, вывести ошибки и завершить работу
    if (!readFile(inputFile, fileContent, errors)) {
        printErrors(errors);
        return 1;
    }

    // Разобрать строки в список действий
    QList<Action*> actions;
    parseActions(fileContent, actions, errors);
    // Проверить согласованность размерностей переменных
    checkDimensions(actions, errors);

    // Если при разборе появились ошибки - вывести их, освободить действия и выйти
    if (!errors.isEmpty()) {
        printErrors(errors);
        qDeleteAll(actions);
        return 1;
    }

    // Создать граф и таблицу переменных, запустить построение графа
    DependencyGraph graph;
    QMap<QString, Action*> varTable;
    graph.buildGraph(actions, varTable, errors);

    // Если появились ошибки этапа построения - вывести их, освободить действия и выйти
    if (!errors.isEmpty()) {
        printErrors(errors);
        qDeleteAll(actions);
        return 1;
    }

    // Открыть выходной файл; если не удалось - ошибка и выход
    QFile outFile(outputFile);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errors.insert(Error(OutputFileCreateFail, 0, 0, outputFile));
        printErrors(errors);
        qDeleteAll(actions);
        return 1;
    }

    // Сформировать DOT-текст и записать его в выходной файл
    QTextStream out(&outFile);
    out << graph.toDOT();
    outFile.close();

    // Сообщить об успехе, освободить память действий и завершить работу
    qDebug().noquote() << "Graph saved to" << outputFile;
    qDeleteAll(actions);
    return 0;
}
