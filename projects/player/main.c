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

int main(void) 
{
  SystemInit(); 
  WavHdr_t wheader;  
  __enable_irq();
  init_sound();
  enable_volume_change(&volume);
  enable_play_indication(&play);
  init_scheduler();
  enable_buttons_control(&play, &volume);
  data_motion( &dataStartAddr, &wav_current_addr, &data_for_dma, &play);
  
  if (wav_parse_headers((const uint32_t*)WAV_START_BLOCK, &wheader, &dataStartAddr))
    {
      enable_data_motion();
      wav_current_addr = (uint8_t*) dataStartAddr;
      init_dma(&data_for_dma);
    }
 
  while(1)
  {      

  }
}
