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

find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

add_executable(${EXE} ${SOURCES})
if(TARGET SDL2::SDL2main)
    target_link_libraries(${EXE} PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(${EXE} PRIVATE SDL2::SDL2)


if(WIN32)
    add_executable(${EXE} WIN32 ${SOURCES} resource.rc)
endif()

set(CONFIG_FILE ${CMAKE_SOURCE_DIR}/g800config)
set(DESTINATION_FILE ${CMAKE_BINARY_DIR}/g800config)
add_custom_command(
    TARGET ${EXE} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_FILE} ${DESTINATION_FILE}
    COMMENT "Copying g800config to build directory if it doesn't already exist."
)

add_custom_command(
    OUTPUT font.h
    COMMAND nkf -e -d font.txt | gawk -f makefont.awk > font.h
    DEPENDS font.txt makefont.awk
)

add_custom_command(
    OUTPUT symbol.h
    COMMAND nkf -e -d symbol1.txt | gawk -f makesymbol.awk > symbol.h
    COMMAND nkf -e -d symbol2.txt | gawk -f makesymbol.awk >> symbol.h
    COMMAND nkf -e -d symbol3.txt | gawk -f makesymbol.awk >> symbol.h
    DEPENDS symbol1.txt symbol2.txt symbol3.txt makesymbol.awk
)

add_custom_command(
    OUTPUT keytop.h
    COMMAND nkf -e -d keytop1.txt | gawk -f makesymbol.awk > keytop.h
    COMMAND nkf -e -d keytopE220_1.txt | gawk -f makesymbol.awk >> keytop.h
    COMMAND nkf -e -d keytop2.txt | gawk -f makesymbol.awk >> keytop.h
    COMMAND nkf -e -d keytopE220_2.txt | gawk -f makesymbol.awk >> keytop.h
    COMMAND nkf -e -d keytop3.txt | gawk -f makesymbol.awk >> keytop.h
    COMMAND nkf -e -d keytopE220_3.txt | gawk -f makesymbol.awk >> keytop.h
    DEPENDS keytop1.txt keytop2.txt keytop3.txt keytopE220_1.txt keytopE220_2.txt keytopE220_3.txt makesymbol.awk
)
