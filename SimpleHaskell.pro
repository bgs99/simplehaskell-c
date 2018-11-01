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
    parsing/tokens.c \
    parsing_new/parser.c \
    parsing/parser_old.c

HEADERS += \
    types.h \
    dictionary_t.h \
    eval.h \
    parsing/reader.h \
    parsing/tokens.h \
    parsing_new/parser.h \
    list.h \
    parsing/parser_old.h

DISTFILES += \
    README.md \
    syntax
