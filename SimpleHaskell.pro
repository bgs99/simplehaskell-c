TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lreadline

SOURCES += \
        main.c \
    types.c \
    dictionary_t.c \
    eval.c \
    parsing/parser.c \
    parsing/type_parser.c \
    parsing/expression_parser.c \
    parsing/pattern_parser.c \
    processing/process.c \
    freemem.c \
    pretty.c

HEADERS += \
    types.h \
    dictionary_t.h \
    eval.h \
    list.h \
    parsing/parser.h \
    parsing/type_parser.h \
    parsing/parser_common.h \
    parsing/expression_parser.h \
    parsing/pattern_parser.h \
    processing/process.h \
    freemem.h \
    pretty.h

DISTFILES += \
    README.md \
    syntax
