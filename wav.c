/*
 * wavファイル読み出しユーティリティ
 * (c) ytakata69
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"

#ifdef STANDALONE
# define DEBUG  1
#endif

enum Bool {
  False = 0, True = 1
};
typedef enum Bool Bool;

/*
 * ファイル中の1バイトを読み出す
 * 全データを読み出したらEOFを返す
 */
int wavGetc(Wav *wav)
{
  if (wav->remain <= 0) { return EOF; }
  wav->remain--;
  return getc(wav->fp);
}

/*
 * ファイル中のnバイト整数 (little endian) を読み出す
 * (ただし n <= 16)
 */
static unsigned int wavGetNBytes(Wav *wav, unsigned int n)
{
  static unsigned char buf[16];
  unsigned int sum;
  unsigned int i;
  if (fread(buf, 1, n, wav->fp) < n) {
    return (unsigned int)-1;
  }
  sum = 0;
  for (i = 0; i < n; i++) {
    sum |= buf[i] << (i * 8);
  }
  return sum;
}

/*
 * ファイル中の32ビット整数を読み出す
 */
static unsigned int wavGetInt32(Wav *wav) { return wavGetNBytes(wav, 4); }

/*
 * ファイル中の16ビット整数を読み出す
 */
static unsigned int wavGetInt16(Wav *wav) { return wavGetNBytes(wav, 2); }

/*
 * ファイル中の4バイトが特定の文字列かどうか
 */
static Bool wavCheckMagic(Wav *wav, const char *magic)
{
  static char buf[4];
  if (fread(buf, 1, sizeof(buf), wav->fp) < sizeof(buf)) {
    return False;
  }
  return strncmp(buf, magic, sizeof(buf)) == 0;
}

/*
 * 指定したチャンクが現れるまでスキップする
 */
static Bool wavSkipChunkUntil(Wav *wav, const char *chunkName)
{
  static char buf[4];
  size_t chunkLen;
  unsigned int i;

  while (True) {
    if (fread(buf, 1, sizeof(buf), wav->fp) < sizeof(buf)) {
      return False;
    }
#   if DEBUG
    printf("magic: %c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);
#   endif
    if (strncmp(buf, chunkName, sizeof(buf)) == 0) {
      return True;
    }
    chunkLen = wavGetInt32(wav);
    /* skip */
    for (i = 0; i < chunkLen; i++) {
      if (getc(wav->fp) == EOF) {
        return False;
      }
    }
  }
}

/*
 * wavファイルのヘッダから情報を取り出す
 */
static void wavCheckHeader(Wav *wav)
{
  size_t chunkLen, dataLen;
  unsigned int frequency, bytePerSec;
  unsigned int format, nChannel, blockSize, bitPerSample;
  Bool valid = True;
  int i;

  valid &= wavCheckMagic(wav, "RIFF");
  wavGetInt32(wav); /* skip file size */
  valid &= wavCheckMagic(wav, "WAVE");
  valid &= wavCheckMagic(wav, "fmt ");
  if (!valid) {
    fprintf(stderr, "Not a supported WAV file\n");
    exit(-1);
  }

  chunkLen     = wavGetInt32(wav);
  format       = wavGetInt16(wav);
  nChannel     = wavGetInt16(wav);
  frequency    = wavGetInt32(wav);
  bytePerSec   = wavGetInt32(wav);
  blockSize    = wavGetInt16(wav);
  bitPerSample = wavGetInt16(wav);

# if DEBUG
  printf("chunkLen    = %zd\n", chunkLen);
  printf("format      = %d\n", format);
  printf("nChannel    = %d\n", nChannel);
  printf("frequency   = %d\n", frequency);
  printf("bytePerSec  = %d\n", bytePerSec);
  printf("blockSize   = %d\n", blockSize);
  printf("bitPerSample= %d\n", bitPerSample);
# endif

  valid &= (format == 1);	/* linear PCM */
  valid &= (nChannel == 1);	/* mono */
  valid &= (bitPerSample == 8);	/* 8 bits */
  valid &= (frequency == bytePerSec);
  valid &= (blockSize == 1);
  if (!valid) {
    fprintf(stderr, "Not a supported WAV file\n");
    exit(-1);
  }

  /* fmtチャンクの拡張部をスキップ */
  for (i = 0; i < (int)chunkLen - 16; i++) {
    getc(wav->fp);
  }

  valid &= wavSkipChunkUntil(wav, "data");
  if (!valid) {
    fprintf(stderr, "Not a supported WAV file\n");
    exit(-1);
  }
  dataLen = wavGetInt32(wav);
# if DEBUG
  printf("dataLen     = %zd\n", dataLen);
  printf("filePos     = %ld\n", ftell(wav->fp));
# endif

  wav->frequency = frequency;
  wav->dataLen   = dataLen;
  wav->remain    = dataLen;
}

/*
 * オープン済みファイルからwavオブジェクトを作る
 */
Wav wavForOpenedFile(FILE *fp)
{
  Wav wav;
  memset(&wav, 0, sizeof(wav));
  wav.fp = fp;
  wavCheckHeader(&wav);
  return wav;
}

/*
 * wavファイルを開く
 */
Wav wavOpen(const char *filename)
{
  FILE *fp = fopen(filename, "rb");
  if (fp == 0) {
    perror(filename);
    exit(1);
  }
  return wavForOpenedFile(fp);
}

#ifdef STANDALONE
int main(int argc, char *argv[])
{
  Wav wav;
  int n;
  int c;

  if (argc > 2) {
    fprintf(stderr, "Usage: %s [file]\n", argv[0]);
    exit(1);
  }
  if (argc > 1) {
    wav = wavOpen(argv[1]);
  } else {
    wav = wavForOpenedFile(stdin);
  }

  n = 0;

  printf("bytes:\n");
  while ((c = wavGetc(&wav)) != EOF) {
    if (n < 3 || wav.remain < 3) {
      printf("%d\n", c);
    } else if (n == 3) {
      printf("...\n");
    }
    n++;
  }

  return 0;
}
#endif
