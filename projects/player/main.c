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

#define WAV_START_BLOCK ((uint8_t*) 0x08010000)
#define WAV_END_BLOCK ((uint8_t*) 0x080FFFF0)

volatile int8_t volume = 0;
volatile uint8_t play = 0;
volatile uint8_t *wav_current_addr = WAV_START_BLOCK;
size_t dataStartAddr;
volatile uint16_t data_for_dma;

void toggle_pause(void);
void volume_up(void);
void volume_down(void);

void toggle_pause()
{
  if (play == 0)
    {
      play = 1;
    }
  else 
  {
      play = 0;
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
  enable_play_indication(&play);
  buttons_run_functions(toggle_pause, volume_up, volume_down);
  data_motion( &dataStartAddr, &wav_current_addr, &data_for_dma, &play);
  
  if (wav_parse_headers((const uint32_t*)WAV_START_BLOCK, &wheader, &dataStartAddr))
    {
      enable_data_motion();
      wav_current_addr = (uint8_t*) dataStartAddr;
      init_dma(&data_for_dma);
    }
 
  while(1)
  {      
   scheduler_iteration();
  }
}
