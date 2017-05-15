OBJCOPY = arm-none-eabi-objcopy
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

%.o: %.bin
	$(OBJCOPY) -I binary -O elf32-littlearm $< $@
%.bin: %.wav wavutil
	dd if=$< bs=`./wavutil $< | awk '/^filePos/{print $$3}'` skip=1 of=$@

.PHONY: clean
clean:
	$(RM) *.o a.out timesig playmusic note.s *.pyc playwav wavutil
	$(RM) wav/*.o wav/*.bin
