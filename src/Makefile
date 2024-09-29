EXE = g800
CC = gcc
OBJS = g800.o z80.o z80disasm.o z80prof.o SDL_hex.o conf.o io.o iocs.o sio.o sound.o util.o menu.o monitor.o basic.o init.o depend.o sdlxpm.o

# SDL2.0
CFLAGS = -DZ80_USE_SDL -DZ80_TRACE -DZ80_PROF -finline-limit=20000 -Wall $(shell sdl2-config --cflags)
LDFLAGS = -s $(shell sdl2-config --libs)

# SDL1.2
#CFLAGS = -DZ80_USE_SDL -DZ80_TRACE -finline-limit=20000 -Wall $(shell sdl-config --cflags)
#LDFLAGS = -s $(shell sdl-config --libs)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LDFLAGS)
win32: $(OBJS) resource.o
	$(CC) -o $(EXE) $(OBJS) resource.o $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) -O2 $<
z80disasm.o: z80disasm.c
	$(CC) -c $(CFLAGS) -Os $<
SDL_hex.o: SDL_hex.c SDL_hex.h
	$(CC) -c $(CFLAGS) -Os $<
conf.o: conf.c
	$(CC) -c $(CFLAGS) -Os $<
menu.o: menu.c
	$(CC) -c $(CFLAGS) -Os $<
monitor.o: monitor.c
	$(CC) -c $(CFLAGS) -Os $<
init.o: init.c
	$(CC) -c $(CFLAGS) -Os $<
sdlxpm.o: sdlxpm.c
	$(CC) -c $(CFLAGS) -Os $<
resource.o: resource.rc g800icon.ico
	windres $< -o $@
$(OBJS): g800.h z80.h basic.h
z80.o: z80.h z80memory.h
iocs.o: font.h
depend.o: depend.h symbol.h keytop.h g800icon.xpm
font.h: makefont.awk font.txt
	nkf -e -d font.txt | gawk -f makefont.awk > font.h
symbol.h: makesymbol.awk symbol1.txt symbol2.txt symbol3.txt
	nkf -e -d symbol1.txt | gawk -f makesymbol.awk > symbol.h
	nkf -e -d symbol2.txt | gawk -f makesymbol.awk >> symbol.h
	nkf -e -d symbol3.txt | gawk -f makesymbol.awk >> symbol.h
keytop.h: makesymbol.awk keytop1.txt keytop2.txt keytop3.txt keytopE220_1.txt keytopE220_2.txt keytopE220_3.txt
	nkf -e -d keytop1.txt | gawk -f makesymbol.awk > keytop.h
	nkf -e -d keytopE220_1.txt | gawk -f makesymbol.awk >> keytop.h
	nkf -e -d keytop2.txt | gawk -f makesymbol.awk >> keytop.h
	nkf -e -d keytopE220_2.txt | gawk -f makesymbol.awk >> keytop.h
	nkf -e -d keytop3.txt | gawk -f makesymbol.awk >> keytop.h
	nkf -e -d keytopE220_3.txt | gawk -f makesymbol.awk >> keytop.h

install:
	cp $(EXE) /usr/local/bin

clean:
	rm -f stdout.txt
	rm -f stderr.txt
	rm -f *.o
	rm -f $(EXE)
	rm -f $(EXE).exe
