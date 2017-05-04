#!/usr/bin/python
# Create note.s, a table of the wave lengths of notes

# PWM clock frequency
PHI_BASE = 19200 * 1000
PHI_DIV  = 3
PHI = PHI_BASE // PHI_DIV

# list of note names
note = "C,Cs,D,Ds,E,F,Fs,G,Gs,A,As,B".split(",")

# preamble
print("\t.section .data")
print("\t.global  note_table")
print("\t.p2align 2")
print("note_table:")

# phase 1: output a table of wave lengths
# phase 2: output a table of note symbols
for phase in [1, 2]:
  nt  = 9  # A
  oct = 3
  hz  = 220
  for i in range(32):
    wavelen = PHI / hz
    name = "%s%d" % (note[nt], oct)
    if phase == 1:
      print("\t.word\t%d\t@ %3s, %f" % (wavelen, name, hz))
    else:
      print("\t.equ\t%3s, %d" % (name, i))

    hz = hz * (2 ** (1.0 / 12))
    nt = nt + 1
    if nt >= len(note):
      nt  = 0
      oct = oct + 1

# afterword
print("\t.global\tn_utick")
print("\t.equ\tTIMER_HZ, (1000 * 1000)")
print("n_utick:\t.word\tTIMER_HZ * 60 / tempo / tick")
