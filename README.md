# play-music-pi
WiringPiを使って音を鳴らすサンプルプログラム。

## 準備
### 回路

  * GPIO #19 に圧電スピーカーを接続
    * GPIOとスピーカーの間に保護抵抗1kΩを挟む
    * (ハードウェアPWMに接続されたピンならどこでも可。#19を使うのは とある実験ボードに合わせて。)

### WiringPi-Python

[WiringPi-Python](https://github.com/WiringPi/WiringPi-Python) のREADMEの Manual Build の手順に従ってインストールする
(WiringPi と WiringPi-Python の両方がインストールされる)。

## 実行方法

### 時報音を鳴らすプログラム

#### Python
```sh
$ sudo ./timesignal.py
```
Python2でもPython3でも実行可能。
Python3で実行したければ，先頭行 (shebang) を `#!/usr/bin/python3` に変更するか，`sudo python3 timesignal.py` を実行。

#### C
```sh
$ make timesig
$ sudo ./timesig
```

### 音楽を演奏するプログラム

#### Python
```sh
$ sudo ./playmus.py
```

Python2でもPython3でも実行できる。

#### C
```sh
$ make
$ sudo ./playmusic
```

### WAVファイルを鳴らすプログラム

WiringPiはPWMのFIFOを扱えないようなので，すべて自前実装。

```sh
$ make playwav
$ sudo ./playwav wav/groove.wav
```

モノラル, 1チャンネル, 8ビット リニアPCM のWAVファイルのみ扱える。

wav/goove.wav は，
[Minim](https://github.com/ddf/Minim)の
examples/AudioPlayer/* などに同梱されている groove.mp3 をWAVに変換したもの。
