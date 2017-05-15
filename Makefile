TOOL_CHAIN = arm-none-eabi-
AS = $(TOOL_CHAIN)as
CC = $(TOOL_CHAIN)gcc
LD = $(TOOL_CHAIN)ld
OBJCOPY = $(TOOL_CHAIN)objcopy

TARGET_ARCH = -march=armv8-a
CFLAGS = -W -Wall
LIBS = -lwiringPi
LDFLAGS = -m armelf --no-undefined

music_data = god-knows
wav_data = wav/groove.wav

.PHONY: all
all: playwav.img

%.s: %.txt
	./txt2asm.pl $< > $@

playmusic: playmusic.o $(music_data).o
	$(CC) $(LDFLAGS) $(LIBS) $+ -o $@

$(music_data).o: note.s
note.s: mknote.py
	./$< >$@

timesig:

%.img: %.elf
	$(OBJCOPY) $< -O binary $@
playwav.elf: boot.o bareplaywav.o pwmfifo.o $(wav_data:.wav=.o)
	$(LD) $(LDFLAGS) $+ -o $@

wav_sym = _binary_$(subst .,_,$(subst /,_,$(wav_data:.wav=.bin)))

bareplaywav.o: bareplaywav.c wavutil
	$(CC) $(CFLAGS) $(TARGET_ARCH) -c $< -o $@ \
	-DWAV_START=$(wav_sym)_start \
	-DWAV_END=$(wav_sym)_end \
	-DFREQUENCY=$(shell ./wavutil $(wav_data)|awk '/^frequency/{print $$3}')

playwav: playwav.o pwmfifo.o wav.o
	$(CC) $+ -o $@
wavutil: wav.c
	cc $(CFLAGS) -DSTANDALONE $+ -o $@

%.o: %.bin
	$(OBJCOPY) -I binary $< -O elf32-littlearm -B arm \
	--rename-section .data=.rodata,alloc,load,readonly,data,contents $@
%.bin: %.wav wavutil
	dd if=$< bs=$(shell ./wavutil $< | awk '/^filePos/{print $$3}') skip=1 of=$@

.PHONY: clean
clean:
	$(RM) *.o a.out timesig playmusic note.s *.pyc playwav wavutil
	$(RM) wav/*.o wav/*.bin *.elf *.img
