#!/usr/bin/python
# coding: utf-8

# 和音を鳴らす (wavファイルを出力)

import sys
import struct

# 定数
SAMPLE_RATE = 48000	# サンプリング周波数
TEMPO = 80		# 60秒間の拍数
TICK  = 32		# 4分音符の分割数

TICK_LEN = SAMPLE_RATE * 60 // TEMPO // TICK

RANGE = 255		# 出力値の最大

# 音高名の定義
noteName = "C,Cs,D,Ds,E,F,Fs,G,Gs,A,As,B".split(",")
nt = 9	# A
oct = 1
hz = 55
for i in range(60):
  name = "%s%d" % (noteName[nt], oct)
  globals()[name] = SAMPLE_RATE // hz	# 音高名の変数を定義
  hz = hz * (2 ** (1.0 / 12))
  nt = nt + 1
  if nt >= len(noteName):
    nt = 0
    oct = oct + 1

R = A1	# 休符

#
# *** 楽譜データ ***
#
# 主よ人の望みの喜びよ, J.S.Bach
# 編曲: Art Studio まほろば
# http://mahoroba.logical-arts.jp/score/download.php?id=145
# 第21小節から下記楽譜のVln.IIを抽出使用
# Anne DeBlois, CC-BY-NC-SA 3.0
# http://imslp.org/wiki/Special:ReverseLookup/190969

music_data = [
  [
    #C5,60,4, B4,60,4, C5,64,128,

    C4,0,11,G4,11,0,A4,10,0, B4,11,0,D5,11,0,C5,9,1,
    C5,11,0,E5,11,0,D5,9,1, D5,11,0,G5,11,0,Fs5,10,0,
    G5,11,0,D5,11,0,B4,10,0, G4,11,0,A4,11,0,B4,10,0,

    C5,11,0,D5,11,0,E5,10,0, D5,11,0,C5,11,0,B4,10,0,
    A4,11,0,B4,11,0,G4,10,0, Fs4,11,0,G4,11,0,A4,10,0,
    D4,11,0,Fs4,11,0,A4,10,0, C5,11,0,B4,11,0,A4,10,0,

    # 5
    B4,11,0,G4,11,0,A4,10,0, B4,11,0,D5,11,0,C5,9,1,
    C5,11,0,E5,11,0,D5,9,1, D5,11,0,G5,11,0,Fs5,10,0,
    G5,11,0,D5,11,0,B4,10,0, G4,11,0,A4,11,0,B4,10,0,

    A4,11,0,D5,11,0,C5,10,0, B4,11,0,A4,11,0,G4,10,0,
    D4,11,0,G4,11,0,Fs4,10,0, G4,11,0,B4,11,0,D5,10,0,
    G5,11,0,D5,11,0,B4,10,0, G4,11,0,B4,11,0,D5,10,0,

    # 9
    G5,94,2, D4,60,4, D4,32,0, C4,60,4,B3,32,0,

    A3,11,0,D4,11,0,E4,10,0, Fs4,11,0,A4,11,0,G4,10,0,
    A4,11,0,C5,11,0,B4,10,0, C5,11,0,A4,11,0,Fs4,10,0,
    D4,11,0,Fs4,11,0,A4,10,0, C5,11,0,B4,11,0,A4,10,0,

    # 14
    B4,11,0,G4,11,0,A4,10,0, B4,11,0,D5,11,0,C5,9,1,
    C5,11,0,E5,11,0,D5,9,1, D5,11,0,G5,11,0,Fs5,10,0,
    G5,11,0,D5,11,0,B4,10,0, G4,11,0,A4,11,0,B4,10,0,

    E4,11,0,D5,11,0,C5,10,0, B4,11,0,A4,11,0,G4,10,0,
    D4,11,0,G4,10,0,Fs4,11,0, G4,11,0,B4,11,0,A4,10,0,
    B4,11,0,D5,11,0,C5,9,1, C5,11,0,E5,11,0,D5,9,1,

    # 18
    D5,11,0,G5,11,0,Fs5,10,0, G5,11,0,D5,11,0,B4,10,0,
    G4,11,0,A4,11,0,B4,10,0, C5,11,0,D5,11,0,E5,10,0,
    D5,11,0,C5,11,0,B4,10,0, A4,11,0,B4,11,0,G4,10,0,

    Fs4,11,0,G4,11,0,A4,10,0, D4,11,0,Fs4,11,0,A4,10,0,
    C5,11,0,B4,11,0,A4,10,0, B4,11,0,G4,11,0,A4,10,0,
    B4,11,0,D5,11,0,C5,9,1, C5,11,0,E5,11,0,D5,10,0,

    # 22
    D5,11,0,G5,11,0,Fs5,10,0, G5,11,0,D5,11,0,B4,10,0,
    G4,11,0,A4,11,0,B4,10,0, E4,11,0,D5,11,0,C5,10,0,
    B4,11,0,A4,11,0,G4,10,0, D4,11,0,G4,14,0,Fs4,19,0,
    G4,74,22,
    0
  ],
  [
    #G4,60,4, G4,60,4, G4,64,128,

    G2,32,0, G3,32,0, E3,32,0,
    B2,32,0, E3,32,0, E2,32,0,
    A2,32,0, B2,32,0, C3,32,0,
    D3,32,0, Fs3,32,0, D3,32,0,

    # 5
    G3,32,0, E3,32,0, C3,32,0,
    B2,32,0, E3,32,0, D3,32,0,
    C3,32,0, Cs3,32,0, D3,32,0,
    G2,44,10,G3,10,0,D3,22,0,B2,10,0,

    # 9
    G3,32,0, Fs3,32,0, E3,32,0,
    Fs3,32,0, E3,32,0, D3,32,0,
    E3,32,0, Fs3,32,0, G3,32,0,

    D3,32,21,D3,10,1, D3,20,2,D3,10,0,
    D2,32,21,D3,10,1, D3,20,2,D3,10,0,

    # 14
    G3,32,0, Fs3,32,0, E3,32,0,
    B3,32,0, G3,32,0, E3,32,0,
    C3,32,0, D3,30,2, D3,32,0,
    G2,32,0, G3,32,0, E3,32,0,

    # 18
    B2,32,0, E3,32,0, E2,32,0,
    A2,32,0, B2,32,0, C3,32,0,
    D3,32,0, Fs3,32,0, D3,32,0,
    G3,32,0, E3,32,0, C3,32,0,

    # 22
    B2,32,0, E3,32,0, D3,32,0,
    C3,32,0, Cs3,32,0, D3,44,0,
    G2,74,0,

    0
  ],
  [
    #E4,60,4, D4,60,4, E4,64,128,

    R,0,96, R,0,96, R,0,96, R,0,96,
    R,0,96, R,0,96, R,0,96, R,0,96,

    # 9
    B3,60,4, C4,32,0,
    A3,32,0, G3,32,0, Fs3,32,0,
    G3,32,0, A3,32,32,

    # 14
    Fs3,32,64, Fs3,0,96,
    B3,64,0,C4,32,0, D4,64,0,B3,32,0,
    A3,11,0,B3,11,0,C4,10,0, B3,32,0,A3,32,0, G3,30,66,

    # 18
    R,0,96, R,0,96, R,0,96,

    #B3,64,0,C4,32,0, D4,64,0,B3,32,0,
    #A3,11,0,B3,11,0,C4,10,0, B3,32,0,A3,25,1,A3,18,0,

    # 21
    D4,22,0,Fs4,10,0, G4,22,0,Fs4,10,0, G4,22,0,A4,10,0,
    B4,22,0,A4,10,0, B4,22,0,G4,10,0, E4,22,0,G4,10,0,
    A4,22,0,Fs4,10,0, G4,22,0,E4,10,0, A3,26,0,C4,18,0,

    B3,74,22,
    
    0
  ]
]

# 奏者オブジェクト
class Track:
  """
  単音を鳴らすオブジェクト
  """
  def __init__(self):
    self.value    = 0	# -1 or 0 or 1
    self.nextUp   = 0	# 次回Highにする時刻
    self.nextDown = 0	# 次回Lowに　 〃
    self.cycle    = 0	# High/Lowの周期
    self.noteEnd  = 0	# 発音が終わる時刻
    self.restEnd  = 0	# 音符が終わる時刻
    self.pos      = 0	# 現在の演奏位置
    self.end      = False	# 演奏が終了

  def update(self, time):
    """
    現在時刻に基づいて状態を更新
    """
    if self.end:
      return
    if time >= self.nextUp:
      self.value = 1
      self.nextUp += self.cycle
      if time >= self.noteEnd:	# 発音終了
        self.value = 0
        self.nextUp = self.restEnd
        self.nextDown = self.noteEnd = self.restEnd + 1
      if time >= self.restEnd:	# 音符終了
        self.value = 1
        self.cycle = self.music_data[self.pos]
        if self.cycle == 0:
          self.end = True
          return
        else:
          self.pos += 1
          self.nextUp   = self.restEnd + self.cycle
          self.nextDown = self.restEnd + self.cycle // 2
          self.noteEnd  = self.restEnd + self.music_data[self.pos] * TICK_LEN
          self.pos += 1
          self.restEnd  = self.noteEnd + self.music_data[self.pos] * TICK_LEN
          self.pos += 1
          if time >= self.noteEnd:
            self.value = 0
        #print(self.cycle, self.noteEnd, self.restEnd)
    if time >= self.nextDown:
      self.value = -1
      self.nextDown += self.cycle

nTrack = len(music_data)
tracks = []
for i in range(nTrack):
  track = Track()
  track.music_data = music_data[i]
  tracks.append(track)

# 演奏中のトラックが残っている間繰り返す
outdata = ""
time = 0
while len(filter(lambda t: not t.end, tracks)) > 0:
  v = 0
  for i in range(nTrack):
    tracks[i].update(time)
    v += tracks[i].value
  ov = (v + nTrack) * RANGE // 2 // nTrack	# -nTrack..nTrack -> 0..RANGE
  outdata += struct.pack("B", ov)
  time += 1

#
# *** WAVファイルを出力 ***
#
ofile = sys.stdout

# ヘッダ
filesize = len(outdata) + 4 + 24 + 8	# WAVE + fmt-chunk + data + n
ofile.write("RIFF")
ofile.write(struct.pack("<I", filesize))
ofile.write("WAVE")

# fmtチャンク
ofile.write("fmt ")
ofile.write(struct.pack("<I", 16))	# chunk長
ofile.write(struct.pack("<HH", 1, 1))	# PCM, mono
ofile.write(struct.pack("<II", SAMPLE_RATE, SAMPLE_RATE)) # rate, byte/sec
ofile.write(struct.pack("<HH", 1, 8))	# (byte/sample)*channel, bit/sample

# dataチャンク
ofile.write("data")
ofile.write(struct.pack("<I", len(outdata)))
ofile.write(outdata)

ofile.close()
