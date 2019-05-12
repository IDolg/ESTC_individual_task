#ifndef LED_INDICATION_C
#define LED_INDICATION_C 1

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "inc/led_indication.h"

volatile uint8_t led_is_on = 0;
volatile uint8_t *play_ptr = 0;

static void init_led_indication(void);
static void blink(void);
static void init_led_timer(void);
void TIM4_IRQHandler(void);

void enable_play_indication(volatile uint8_t *play)
{
  play_ptr = play;
  init_led_indication();
  init_led_timer();
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

void init_led_timer(void)
{
  TIM_TimeBaseInitTypeDef timer;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
 
  TIM_TimeBaseStructInit(&timer);
  timer.TIM_Prescaler = 10000;  
  timer.TIM_Period = 2650 - 1 ;  
  TIM_TimeBaseInit(TIM4, &timer);
  
  NVIC_InitTypeDef nvic_struct;
  nvic_struct.NVIC_IRQChannel = TIM4_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
    
  TIM_Cmd(TIM4, ENABLE);
  NVIC_EnableIRQ(TIM4_IRQn);
  TIM_ITConfig(TIM4, TIM_DIER_UIE, ENABLE);
}

void TIM4_IRQHandler()
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
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
        
      TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

#endif
