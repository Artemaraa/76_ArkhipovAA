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
    $$PWD/../GraphDependencies/dependencygraph.cpp \
    TEST_ApplyAction.cpp \
    TEST_BuildGraph.cpp \
    TEST_CompareIndices.cpp \
    TEST_DetermineDependency.cpp \
    TEST_ParseActions.cpp

# Заголовки основного проекта
HEADERS += \
    $$PWD/../GraphDependencies/functions.h \
    $$PWD/../GraphDependencies/exprnode.h \
    $$PWD/../GraphDependencies/error.h \
    $$PWD/../GraphDependencies/action.h \
    $$PWD/../GraphDependencies/dependencyedge.h \
    $$PWD/../GraphDependencies/dependencyedge.h \
    TEST_ApplyAction.h \
    TEST_BuildGraph.h \
    TEST_CompareIndices.h \
    TEST_DetermineDependency.h \
    TEST_ParseActions.h \
    TEST_ParseExpression.h

# Тестовые файлы
SOURCES += \
    main.cpp \
    TEST_ParseExpression.cpp
