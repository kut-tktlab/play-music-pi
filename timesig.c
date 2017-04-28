#include <stdio.h>
#include <wiringPi.h>

#define SPEAKER  19	/* スピーカのGPIO番号 */
#define CLOCK_DIV  3	/* 分周数 (base clock == 19.2MHz) */
#define CLOCK  (19200 * 1000 / CLOCK_DIV)
  /*
   * note: CLOCK_DIVが小さい方が正確な周波数を設定できるけど,
   * range (= CLOCK/CLOCK_DIV/freq) が32767を超えるとだめっぽい
   */

/*
 * WiringPi-Pythonを使って時報音を鳴らすよ
 */
int main()
{
  unsigned int freq;
  int i;

  /* 最初にセットアップが必要だよ. GPIO番号を使うよ */
  if (wiringPiSetupGpio() == -1) {
    fprintf(stderr, "cannot setup gpio.\n");
    return -1;
  }

  /* SPEAKERピンをPWM出力モードにするよ */
  pinMode(SPEAKER, PWM_OUTPUT);

  /* PWMの設定 (pinModeより後に実行しないとだめっぽい) */
  pwmSetMode(PWM_MODE_MS);	/* mark:space mode */
  pwmSetClock(CLOCK_DIV);	/* divisor (base clock == 19.2MHz) */

  freq = 440;

  for (i = 0; i < 4; i++) {
    /*
     * 波長   range = CLOCK / freq  (4回目は1オクターブ上)
     * 山の幅 data  = range / 2     (duty cycle == 50%)
     */
    unsigned int range = CLOCK / freq / (i < 3 ? 1 : 2);
    pwmSetRange(range);
    pwmWrite(SPEAKER, range / 2);

    /* 0.1秒待ってください (4回目は1秒) */
    delay(i < 3 ? 100 : 1000);

    /* 音を止めます (duty cycle == 0) */
    pwmWrite(SPEAKER, 0);

    /* 0.9秒待ってください */
    delay(900);
  }

  return 0;
}
