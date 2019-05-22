#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"
#include "inc/audio_wav.h"
#include <string.h>
#include "inc/sound.h"
#include "inc/led_indication.h"
#include "inc/buttons.h"
#include "inc/scheduler.h"

#define WAV_START_BLOCK ((size_t*) 0x08010000)
#define WAV_END_BLOCK ((size_t*) 0x080FFFF0)

volatile int8_t volume = 0;
volatile uint8_t play = 0;
size_t *dataStartAddr;

void toggle_pause(void);
void volume_up(void);
void volume_down(void);

void toggle_pause()
{
  if (play == 0)
    {
      play = 1;
      start_playing();
      blink_green();
      volume = set_volume(volume);
    }
  else 
  {
      play = 0;
      stop_playing();
      blink_red();
      set_volume(-120);
  }
}

void volume_up()
{
  if(volume<12) volume += 6;
  volume = set_volume(volume);
}

void volume_down()
{
  if (volume>-120) volume -= 6;
  volume = set_volume(volume);
}

int main(void) 
{
  SystemInit(); 
  init_scheduler();
  WavHdr_t wheader;  
  __enable_irq();
  init_sound();
  play = 0;
  blink_red();
  enable_led_indication();
  buttons_run_functions(toggle_pause, volume_up, volume_down);
  
  if (wav_parse_headers((const uint32_t*)WAV_START_BLOCK, &wheader, &dataStartAddr))
    {
      play_this(dataStartAddr, WAV_END_BLOCK);
      stop_playing();
    }
 
  while(1)
  {      
   scheduler_iteration();
  }
}
