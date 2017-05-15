#include "pwmfifo.h"

/* GPIO number for a speaker */
#define SPEAKER  19

/* clock for PWM */
#define PWM_BASECLOCK	19200000
#define PWM_CLOCK_DIV	2

/* sampling rate */
#ifndef  FREQUENCY
# define FREQUENCY 44100
#endif

/* start and end addr of wav data */
#ifndef  WAV_START
# define WAV_START  _binary_wav_groove_bin_start
#endif
#ifndef  WAV_END
# define WAV_END    _binary_wav_groove_bin_end
#endif

extern unsigned char WAV_START[];
extern unsigned char WAV_END[];

int main()
{
  unsigned int range;
  int c;
  unsigned int pwmDiv = PWM_CLOCK_DIV;
  unsigned char *wavData = WAV_START;
  unsigned char *wavDataEnd = WAV_END;

  range = PWM_BASECLOCK / pwmDiv / FREQUENCY;

  /* Setting up */
  if (setupGpio() == -1) {
    return -1;
  }

  pinModePwm(SPEAKER);
  pwmSetModeMS();	/* これがないと初期化に失敗することがある */
  pwmSetClock(pwmDiv);	/* 〃 */
  pwmSetModeBalanced();
  pwmSetClock(pwmDiv);
  pwmSetRange(range);

  /* Play music */
  while (wavData < wavDataEnd) {
    c = *wavData++;
    pwmWriteFifo(c * range / 255);
  }

  /* Wait until the fifo becomes empty */
  pwmWaitFifoEmpty();

  return 0;
}
