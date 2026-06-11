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

    // Проверить, что файл не пуст: если все строки пустые - ошибка и выход
    bool hasNonEmptyLine = false;
    for (const QString& line : fileContent) {
        if (!line.trimmed().isEmpty()) { hasNonEmptyLine = true; break; }
    }
    if (!hasNonEmptyLine) {
        errors.insert(Error(EmptyInputFile, 0, 0, ""));
        printErrors(errors);
        return 1;
    }

    // Разобрать строки в список действий
    QList<Action*> actions;
    parseActions(fileContent, actions, errors);

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
