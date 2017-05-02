/*
 * wavファイル読み出しユーティリティ
 * (c) ytakata69
 */

#include <stdio.h>

struct Wav {
  FILE *fp;
  unsigned int frequency;	/* sampling rate */
  unsigned int dataLen;		/* num of bytes */
  unsigned int remain;          /* num of remaining bytes */
};
typedef struct Wav Wav;

int wavGetc(Wav *wav);
Wav wavOpen(const char *filename);
Wav wavForOpenedFile(FILE *fp);
