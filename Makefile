CFLAGS = -W -Wall
LDFLAGS = -lwiringPi

music_data = god-knows

playmusic: playmusic.o $(music_data).o
	$(CC) $(LDFLAGS) $+ -o $@

$(music_data).o: note.s
note.s: note.pl
	./$< >$@

timesig:

.PHONY: clean
clean:
	$(RM) *.o a.out timesig playmusic note.s *.pyc
