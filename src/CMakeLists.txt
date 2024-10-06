cmake_minimum_required(VERSION 3.10)
project(g800 LANGUAGES C CXX)

set(SOURCES
    SDL_hex.cc
    # basic.cc
    conf.cc
    depend.cc
    g800.cc
    init.cc
    io.cc
    # iocs.cc
    load_rom.cc
    menu.cc
    monitor.cc
    sdlxpm.cc
    sio.cc
    sound.cc
    util.cc
    utils.cc
    z80.cc
    z80disasm.cc
    z80memory.cc
    z80prof.cc
)

# FIXME: re-enable -Wall
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DZ80_USE_SDL -DZ80_TRACE -DZ80_PROF -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DZ80_USE_SDL -DZ80_TRACE -DZ80_PROF -w")

set(SYMBOLS_DIR ${CMAKE_SOURCE_DIR}/symbols)
set(FONT_HEADER ${CMAKE_BINARY_DIR}/font.h)
add_custom_command(
    OUTPUT ${FONT_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/font.txt | gawk -f ${SYMBOLS_DIR}/makefont.awk > ${FONT_HEADER}
    DEPENDS ${SYMBOLS_DIR}/font.txt ${SYMBOLS_DIR}/makefont.awk
)

set(SYMBOL_HEADER ${CMAKE_BINARY_DIR}/symbol.h)
add_custom_command(
    OUTPUT ${SYMBOL_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/symbol1.txt | gawk -f ${SYMBOLS_DIR}/makesymbol.awk  > ${SYMBOL_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/symbol2.txt | gawk -f ${SYMBOLS_DIR}/makesymbol.awk >> ${SYMBOL_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/symbol3.txt | gawk -f ${SYMBOLS_DIR}/makesymbol.awk >> ${SYMBOL_HEADER}
    DEPENDS ${SYMBOLS_DIR}/symbol1.txt ${SYMBOLS_DIR}/symbol2.txt ${SYMBOLS_DIR}/symbol3.txt
    ${SYMBOLS_DIR}/makesymbol.awk
)

set(KEYTOP_HEADER ${CMAKE_BINARY_DIR}/keytop.h)
add_custom_command(
    OUTPUT ${KEYTOP_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/keytop1.txt      | gawk -f ${SYMBOLS_DIR}/makesymbol.awk  > ${KEYTOP_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/keytopE220_1.txt | gawk -f ${SYMBOLS_DIR}/makesymbol.awk >> ${KEYTOP_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/keytop2.txt      | gawk -f ${SYMBOLS_DIR}/makesymbol.awk >> ${KEYTOP_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/keytopE220_2.txt | gawk -f ${SYMBOLS_DIR}/makesymbol.awk >> ${KEYTOP_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/keytop3.txt      | gawk -f ${SYMBOLS_DIR}/makesymbol.awk >> ${KEYTOP_HEADER}
    COMMAND nkf -e -d ${SYMBOLS_DIR}/keytopE220_3.txt | gawk -f ${SYMBOLS_DIR}/makesymbol.awk >> ${KEYTOP_HEADER}
    DEPENDS ${SYMBOLS_DIR}/keytop1.txt ${SYMBOLS_DIR}/keytop2.txt ${SYMBOLS_DIR}/keytop3.txt
    ${SYMBOLS_DIR}/keytopE220_1.txt ${SYMBOLS_DIR}/keytopE220_2.txt
    ${SYMBOLS_DIR}/keytopE220_3.txt
    ${SYMBOLS_DIR}/makesymbol.awk
)

# FIXME: not tested on Windows.
# if(WIN32)
#     add_executable(${EXE} WIN32 ${SOURCES} resource.rc)
# endif()

set(GENERATED_HEADERS ${FONT_HEADER} ${SYMBOL_HEADER} ${KEYTOP_HEADER})

add_library(
  core
  ${SOURCES}
  ${GENERATED_HEADERS}
)
target_include_directories(core PRIVATE  ${CMAKE_BINARY_DIR})

set(EXE g800)
add_executable(${EXE} main.cc)
target_link_libraries(${EXE} PRIVATE core)
target_include_directories(${EXE} PRIVATE  ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
if(TARGET SDL2::SDL2main)
    target_link_libraries(core PRIVATE SDL2::SDL2main)
    target_link_libraries(${EXE} PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(core PRIVATE SDL2::SDL2)
target_link_libraries(${EXE} PRIVATE SDL2::SDL2)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_COLOR_DIAGNOSTICS ON)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# gtest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Tests
enable_testing()
add_executable(
  g800_test
  z80memory_test.cc
)
target_link_libraries(
  g800_test
  core
  GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(g800_test)
