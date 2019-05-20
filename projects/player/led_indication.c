#ifndef LED_INDICATION_C
#define LED_INDICATION_C 1

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "inc/led_indication.h"
#include "inc/scheduler.h"

volatile uint8_t led_is_on = 0;
volatile uint8_t *play_ptr = 0;

static void init_led_indication(void);
static void blink(void);
static void led_iteration(void);

void enable_play_indication(volatile uint8_t *play)
{
  play_ptr = play;
  init_led_indication();
  led_iteration();
}

void init_led_indication(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitTypeDef A;
  A.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
  A.GPIO_Mode  = GPIO_Mode_OUT;
  A.GPIO_OType = GPIO_OType_PP;
  A.GPIO_Speed = GPIO_Speed_100MHz;
  A.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &A);  
};

void blink()
{ 
  if(led_is_on)
    {
      GPIO_ResetBits(GPIOD, GPIO_Pin_12);
      led_is_on = 0;
    }
  else
    {
      GPIO_SetBits(GPIOD, GPIO_Pin_12);
      led_is_on = 1;
    }
}

void led_iteration()
{
      if (*play_ptr) //if play is 1
        {
          blink(); // blink the "play led"
          GPIO_ResetBits(GPIOD, GPIO_Pin_13); // make sure the "pause led" is off
        }
      if (!*play_ptr) // if play is 0
        {
          GPIO_SetBits(GPIOD, GPIO_Pin_13); // indicate pause
          GPIO_ResetBits(GPIOD, GPIO_Pin_12); // make sure play led is off
        }
        
    schedule(led_iteration, 1000);
}

#endif
