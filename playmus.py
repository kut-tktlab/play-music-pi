#!/usr/bin/python
# coding: utf-8

# WiringPi-Pythonを使って音楽を演奏するよ

import musdata	# 楽譜データ musdata.py
import note	# 音高データ note.py
import wiringpi

SPEAKER = 19
wiringpi.wiringPiSetupGpio()
wiringpi.pinMode(SPEAKER, wiringpi.PWM_OUTPUT)
wiringpi.pwmSetMode(wiringpi.PWM_MODE_MS)	# mark:space mode
wiringpi.pwmSetClock(3)		# divisor (base clock == 19.2MHz)

clock = 19200 * 1000 // 3
n_utick = 1000 * 1000 * 60 // musdata.tempo // musdata.tick

for (nt, note_len, rest_len) in musdata.music_data:
  # print("%d %d %d" % (note.note_table[nt], note_len, rest_len))

  rng = note.note_table[nt]
  wiringpi.pwmSetRange(rng)
  wiringpi.pwmWrite(SPEAKER, rng // 2)

  for _ in range(note_len):
    wiringpi.delayMicroseconds(n_utick)

  # stop sounding
  wiringpi.pwmWrite(SPEAKER, 0)

  for _ in range(rest_len):
    wiringpi.delayMicroseconds(n_utick)

