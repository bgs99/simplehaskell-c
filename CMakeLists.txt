cmake_minimum_required(VERSION 2.8)

project(SimpleHaskell)

include_directories(.)

set(SOURCES
    type_structs.c
    eval.c
    pretty.c
    dictionary_t.c
    freemem.c
    processing/process.c
    parsing/parser.c
    parsing/expression_parser.c
    parsing/type_parser.c
    parsing/pattern_parser.c
    parsing/expression_parser.c
    main.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} readline)


