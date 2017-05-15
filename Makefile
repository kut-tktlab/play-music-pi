CFLAGS = -W -Wall
LIBS = -lwiringPi
LDFLAGS = $(LIBS)

music_data = god-knows

%.s: %.txt
	./txt2asm.pl $< > $@

playmusic: playmusic.o $(music_data).o
	$(CC) $(LDFLAGS) $(LIBS) $+ -o $@

$(music_data).o: note.s
note.s: mknote.py
	./$< >$@

timesig:

playwav: playwav.o pwmfifo.o wav.o
	$(CC) $+ -o $@
wavutil: wav.c
	$(CC) $(CFLAGS) -DSTANDALONE $+ -o $@

.PHONY: clean
clean:
	$(RM) *.o a.out timesig playmusic note.s *.pyc playwav wavutil
