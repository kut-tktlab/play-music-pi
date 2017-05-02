#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "wav.h"

/* GPIO number for a speaker */
#define SPEAKER  19

/* clock for PWM */
#define PWM_BASECLOCK	19200000
#define PWM_CLOCK_DIV	2

int setupGpio();

void pinModePwm(int pin);
void pwmSetModeBalanced();
void pwmSetClock(unsigned int divider);
void pwmSetRange(unsigned int range);
void pwmWriteFifo(unsigned int byte);
void pwmWaitFifoEmpty();

void delayMicroseconds(unsigned int usec);


int main(int argc, char *argv[])
{
  Wav wav;
  unsigned int range;
  int c;

  /* Open a wav file */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s wavfile\n", argv[0]);
    exit(1);
  }
  wav = wavOpen(argv[1]);
  range = PWM_BASECLOCK / PWM_CLOCK_DIV / wav.frequency;

  /* Setting up */
  if (setupGpio() == -1) {
    fprintf(stderr, "cannot setup gpio.\n");
    return -1;
  }

  pinModePwm(SPEAKER);
  pwmSetClock(PWM_CLOCK_DIV);
  pwmSetModeBalanced();
  pwmSetRange(range);

  /* Play music */
  while ((c = wavGetc(&wav)) != EOF) {
    pwmWriteFifo(c * range / 255);
  }

  /* Wait until the fifo becomes empty */
  pwmWaitFifoEmpty();

  return 0;
}

/*
 * *** GPIO ***
 */

/*
 * Addresses in the SoC
 */
#define PERIPHERAL_BASE	 0x3f000000

#define GPIO_BASE	(0x00200000 + PERIPHERAL_BASE)
#define GPFSEL0		(0x00 /sizeof(uint32_t))
#define GPSET0		(0x1c /sizeof(uint32_t))
#define GPCLR0		(0x28 /sizeof(uint32_t))
#define GPFSEL_ALT5	2

#define PWMCLK_BASE	(0x00101000 + PERIPHERAL_BASE)
#define PWMCLK_CTL	(0xa0 /sizeof(uint32_t))
#define PWMCLK_DIV	(0xa4 /sizeof(uint32_t))
#define PWMCLK_PASSWD	0x5a000000

#define PWM_BASE	(0x0020c000 + PERIPHERAL_BASE)
#define PWM_CTL		(0x00 /sizeof(uint32_t))
#define PWM_STA		(0x04 /sizeof(uint32_t))
#define PWM_FIF1	(0x18 /sizeof(uint32_t))
#define PWM_RNG2	(0x20 /sizeof(uint32_t))
#define PWM_DAT2	(0x24 /sizeof(uint32_t))
#define PWM2_USEFIFO	(1<<13)
#define PWM2_ENABLE	(1<<8)
#define PWM_CLRFIFO	(1<<6)

#define TIMER_BASE	(0x00003000 + PERIPHERAL_BASE)
#define TMCLO		(0x04 /sizeof(uint32_t))

/* paging size */
#define BLOCK_SIZE	(4 * 1024)

typedef unsigned int uint32_t;

/* Base addresses of control registers */
static uint32_t *gpio, *clock, *pwm, *timer;

/* An aux function doing mmap */
static void *mmapControlRegs(int fd, off_t offset)
{
  void *p;
  p = mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
  if (p == MAP_FAILED) {
    perror("mmap");
  }
  return p;
}

/*
 * Set up this GPIO-manipulation module.
 * \return 0 for success; -1 otherwise.
 */
int setupGpio()
{
  int fd;

  /* Open /dev/mem (sudo required) */
  fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC);
  if (fd == -1) {
    perror("/dev/mem");
    return -1;
  }

  gpio  = mmapControlRegs(fd, GPIO_BASE);
  clock = mmapControlRegs(fd, PWMCLK_BASE);
  pwm   = mmapControlRegs(fd, PWM_BASE);
  timer = mmapControlRegs(fd, TIMER_BASE);

  if (gpio == MAP_FAILED || clock == MAP_FAILED ||
      pwm  == MAP_FAILED || timer == MAP_FAILED)
  {
    return -1;
  }
  return 0;
}

/*
 * *** PWM ***
 */

/* Set the pin mode to PWM_OUTPUT.  */
void pinModePwm(int pin)
{
  if (pin != 18 && pin != 19) {
    fprintf(stderr, "pinModePwm: only GPIO 18 and 19 are supported.\n");
    return;
  }
  /* Set the mode of the GPIO to alt5 */
  *(gpio + GPFSEL0 + pin/10) = GPFSEL_ALT5 << ((pin % 10) * 3);
}

/* Set the PWM to the balanced mode.  */
void pwmSetModeBalanced()
{
  *(pwm + PWM_CTL) = PWM_CLRFIFO;
  delayMicroseconds(10);
  *(pwm + PWM_CTL) = PWM2_USEFIFO | PWM2_ENABLE;
  delayMicroseconds(10);
}

/* Set PWM clock divider.  */
void pwmSetClock(unsigned int divider)
{
# if 0
  uint32_t pwmctl = *(pwm + PWM_CTL);	/* save */
  *(pwm + PWM_CTL) = 0;			/* stop pwm */
# else
  *(pwm + PWM_CTL) = PWM2_USEFIFO | PWM2_ENABLE;
# endif

  *(clock + PWMCLK_CTL) = PWMCLK_PASSWD|0x1;	/* enable=0, osc */
  delayMicroseconds(110);		/* cf. wiringPi.c */

  while ((*(clock + PWMCLK_CTL) & 0x80) != 0) {}  /* wait while busy */
  delayMicroseconds(1);

  *(clock + PWMCLK_DIV) = (PWMCLK_PASSWD | (divider << 12));
  *(clock + PWMCLK_CTL) = PWMCLK_PASSWD|0x11;	/* enable=1, osc */

# if 0
  *(pwm + PWM_CTL) = pwmctl | PWM_CLRFIFO;	/* restore */
# endif
}

/* Set PWM range. (Only supports GPIO #18 and #19) */
void pwmSetRange(unsigned int range)
{
  *(pwm + PWM_RNG2) = range;
  delayMicroseconds(10);
}

/* Write a byte into PWM FIFO. */
void pwmWriteFifo(unsigned int byte)
{
    uint32_t status;

    /* wait while full */
    while (((status = *(pwm + PWM_STA)) & 0x1) != 0) {
      uint32_t err = (status & 0x1fc);
      if (err != 0) { *(pwm + PWM_STA) = err; }
    }
    *(pwm + PWM_FIF1) = byte;
    delayMicroseconds(10);
}

/* Wait until the FIFO becomes empty. */
void pwmWaitFifoEmpty()
{
  /* wait while not empty */
  while ((*(pwm + PWM_STA) & 0x2) == 0) {}
}

/*
 * *** delay ***
 */

/* Wait for usec microseconds.  */
void delayMicroseconds(unsigned int usec)
{
  /* current + usec */
  uint32_t target = *(timer + TMCLO) + usec;

  /* wait until target */
  while (*(timer + TMCLO) < target) {}
}

