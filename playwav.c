#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pwmfifo.h"
#include "wav.h"

/* GPIO number for a speaker */
#define SPEAKER  19

/* clock for PWM */
#if NOT_USE_PLL
# define PWM_BASECLOCK	19200000		/* oscillator */
# define PWM_CLOCK_DIV	2			/* minimum */
#else
# define PWM_BASECLOCK	(500 * 1000 * 1000)	/* PLLD */
# define PWM_CLOCK_DIV	20			/* there is 25MHz cap? */
#endif


int main(int argc, char **argv)
{
  Wav wav;
  unsigned int range;
  int c;
  char *argv0 = argv[0];
  unsigned int pwmDiv = PWM_CLOCK_DIV;

  /* Set up the clock divider of PWM */
  if (argc >= 2 && argv[1][0] == '-') {
    if (strcmp(argv[1], "-div") == 0 && argc >= 3) {
      pwmDiv = atoi(argv[2]);
      argc -= 2;
      argv += 2;
    } else {
      fprintf(stderr, "Usage: %s [-div d] wavfile\n", argv0);
      exit(1);
    }
  }

  /* Open a wav file */
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [-div d] wavfile\n", argv0);
    exit(1);
  }
  wav = wavOpen(argv[1]);
  range = PWM_BASECLOCK / pwmDiv / wav.frequency;

  /* Setting up */
  if (setupGpio() == -1) {
    fprintf(stderr, "cannot setup gpio.\n");
    return -1;
  }

  pinModePwm(SPEAKER);
  pwmSetModeBalanced();
  pwmSetClock(pwmDiv);
  pwmSetRange(range);

  /* Play music */
  while ((c = wavGetc(&wav)) != EOF) {
    pwmWriteFifo(c * range / 255);
  }

  /* Wait until the fifo becomes empty */
  pwmWaitFifoEmpty();

  return 0;
}
