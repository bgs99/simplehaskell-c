TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    types.c \
    dictionary_t.c \
    eval.c \
    hstd.c \
    parser.c \
    reader.c

HEADERS += \
    types.h \
    dictionary_t.h \
    eval.h \
    hstd.h \
    parser.h \
    reader.h

DISTFILES += \
    README.md
