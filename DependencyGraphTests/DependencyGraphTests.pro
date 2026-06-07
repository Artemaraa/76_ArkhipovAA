QT += testlib
QT -= gui

CONFIG += c++17 console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# Путь к исходникам нового основного проекта
INCLUDEPATH += $$PWD/../GraphDependencies

# Исходники основного проекта
SOURCES += \
    $$PWD/../GraphDependencies/functions.cpp \
    $$PWD/../GraphDependencies/exprnode.cpp \
    $$PWD/../GraphDependencies/error.cpp \
    $$PWD/../GraphDependencies/action.cpp \
    $$PWD/../GraphDependencies/dependencyedge.cpp \
    TEST_CompareIndices.cpp

# Заголовки основного проекта
HEADERS += \
    $$PWD/../GraphDependencies/functions.h \
    $$PWD/../GraphDependencies/exprnode.h \
    $$PWD/../GraphDependencies/error.h \
    $$PWD/../GraphDependencies/action.h \
    $$PWD/../GraphDependencies/dependencyedge.h

# Тестовые файлы
SOURCES += \
    main.cpp \
    TEST_ParseExpression.cpp
