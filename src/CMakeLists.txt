cmake_minimum_required(VERSION 3.10)
project(g800 C)
set(EXE g800)

set(SOURCES
    g800.c
    z80.c
    z80disasm.c
    z80prof.c
    SDL_hex.c
    conf.c
    io.c
    iocs.c
    sio.c
    sound.c
    util.c
    menu.c
    monitor.c
    basic.c
    init.c
    depend.c
    sdlxpm.c
    utils.c
    z80memory.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DZ80_USE_SDL -DZ80_TRACE -DZ80_PROF -Wall")

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
add_executable(${EXE} ${SOURCES} ${GENERATED_HEADERS})
target_include_directories(${EXE} PRIVATE ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
if(TARGET SDL2::SDL2main)
    target_link_libraries(${EXE} PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(${EXE} PRIVATE SDL2::SDL2)
