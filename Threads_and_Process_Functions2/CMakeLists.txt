cmake_minimum_required(VERSION 2.8)

set (CMAKE_VERBOSE_MAKEFILE 1)

project (example C)

set (CMAKE_BUILD_TYPE Debug)

set (CMAKE_C_FLAGS "-Wall -Werror")

set (TARGET_EXE_NAME example)
set (SRCS
    src/main.c
)

# Apache Portable Runtime
execute_process (
    COMMAND apr-1-config --includedir
    OUTPUT_VARIABLE _APR_INCLUDE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string (STRIP ${_APR_INCLUDE} APR_INCLUDE)
message ("APR_INCLUDE:" ${APR_INCLUDE})
execute_process (
    COMMAND apr-1-config --link-ld --libs
    OUTPUT_VARIABLE _APR_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string (STRIP ${_APR_LIBS} APR_LIBS)
message ("APR_LIBS:" ${APR_LIBS})
execute_process (
    COMMAND apu-1-config --includedir
    OUTPUT_VARIABLE _APU_INCLUDE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string (STRIP ${_APU_INCLUDE} APU_INCLUDE)
message ("APU_INCLUDE:" ${APU_INCLUDE})
execute_process (
    COMMAND apu-1-config --link-ld --libs
    OUTPUT_VARIABLE _APU_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string (STRIP ${_APU_LIBS} APU_LIBS)
message ("APU_LIBS:" ${APU_LIBS})

include_directories (
    ${APR_INCLUDE}
    ${APU_INCLUDE}
)
link_directories (
)
set (LIBS
    ${APR_LIBS}
    ${APU_LIBS}
)

add_executable (${TARGET_EXE_NAME} ${SRCS})
target_link_libraries (${TARGET_EXE_NAME} ${LIBS})
