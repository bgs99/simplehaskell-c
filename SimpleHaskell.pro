TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    types.c \
    dictionary_t.c \
    eval.c \
    parsing/reader.c \
    parsing/parser.c

HEADERS += \
    types.h \
    dictionary_t.h \
    eval.h \
    parsing/reader.h \
    list.h \
    parsing/parser.h

DISTFILES += \
    README.md \
    syntax
