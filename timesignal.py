#!/usr/bin/python
# coding: utf-8

# WiringPi-Pythonを使って時報音を鳴らすよ

import wiringpi

# スピーカのGPIO番号
SPEAKER = 19

# 最初にセットアップが必要だよ
wiringpi.wiringPiSetupGpio()		# GPIO番号を使うよ

# SPEAKERピンをPWM出力モードにするよ
wiringpi.pinMode(SPEAKER, wiringpi.PWM_OUTPUT)

# PWMの設定 (pinModeより後に実行しないとだめっぽい)
wiringpi.pwmSetMode(wiringpi.PWM_MODE_MS)	# mark:space mode
wiringpi.pwmSetClock(3)		# divisor (base clock == 19.2MHz)

# divisorが小さい方が正確な周波数を設定できるけど,
# range (= clock/divisor/freq) が32767を超えるとだめっぽい

clock = 19200 * 1000 / 3
freq = 440

for i in range(4):
  # 波長   range = clock / freq  (4回目は1オクターブ上)
  # 山の幅 data  = range / 2     (duty cycle == 50%)
  range = clock / freq / (1 if i < 3 else 2)
  wiringpi.pwmSetRange(range)
  wiringpi.pwmWrite(SPEAKER, range / 2)

  # 0.1秒待ってください (4回目は1秒)
  wiringpi.delay(100 if i < 3 else 1000)

  # 音を止めます (duty cycle == 0)
  wiringpi.pwmWrite(SPEAKER, 0)

  # 0.9秒待ってください
  wiringpi.delay(900)

