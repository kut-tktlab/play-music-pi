#include <stdio.h>
#include <wiringPi.h>

#define SPEAKER  19	/* GPIO number for a speaker */
#define CLOCK_DIV  3	/* divisor (base clock == 19.2MHz) */
#define CLOCK  (19200 * 1000 / CLOCK_DIV)

extern unsigned char music_data[];	/* score data */
extern unsigned int  note_table[];	/* number to wave length */
extern unsigned int  tick_len;		/* clock count per 4*32-th note */

int main()
{
  unsigned char *p;

  /* Setting up */
  if (wiringPiSetupGpio() == -1) {
    fprintf(stderr, "cannot setup gpio.\n");
    return -1;
  }
  pinMode(SPEAKER, PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);	/* mark:space mode */
  pwmSetClock(CLOCK_DIV);	/* divisor (base clock == 19.2MHz) */

  for (p = music_data; *p != 0; p += 3) {
    unsigned int range = note_table[*p];
    unsigned int note_len = *(p + 1);
    unsigned int rest_len = *(p + 2);
    printf("  (%d, %d, %d),\n", *p, note_len, rest_len);

    /* Start sounding */
    pwmSetRange(range);
    pwmWrite(SPEAKER, range / 2);

    for (; note_len > 0; note_len--) {
      delayMicroseconds(tick_len);
    }

    /* Stop sounding */
    pwmWrite(SPEAKER, 0);

    for (; rest_len > 0; rest_len--) {
      delayMicroseconds(tick_len);
    }
  }
  return 0;
}
