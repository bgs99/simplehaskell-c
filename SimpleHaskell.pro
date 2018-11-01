TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    types.c \
    dictionary_t.c \
    eval.c \
    parsing/parser.c \
    parsing/reader.c \
    parsing/tokens.c \
    parsing_new/parser.c

HEADERS += \
    types.h \
    dictionary_t.h \
    eval.h \
    parsing/parser.h \
    parsing/reader.h \
    parsing/tokens.h \
    parsing_new/parser.h

DISTFILES += \
    README.md \
    syntax
