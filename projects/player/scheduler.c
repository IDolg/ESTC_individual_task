#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"
#include <string.h>
#include "inc/scheduler.h"

static void voidfunc(void);
static void (*scheduled_func) (void) = voidfunc;
void TIM3_IRQHandler(void);

void voidfunc(void){};

void schedule(void (*func) (void))
{
  scheduled_func = func;
  TIM_Cmd(TIM3, ENABLE);
}

void init_scheduler(void)
{
  TIM_TimeBaseInitTypeDef timer;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
  TIM_TimeBaseStructInit(&timer);
  timer.TIM_Prescaler = 1000;  
  timer.TIM_Period = 2650 - 1;  
  TIM_TimeBaseInit(TIM3, &timer);
  
  NVIC_InitTypeDef nvic_struct;
  nvic_struct.NVIC_IRQChannel = TIM3_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
    
  NVIC_EnableIRQ(TIM3_IRQn);
  TIM_ITConfig(TIM3, TIM_DIER_UIE, ENABLE);
}

void TIM3_IRQHandler()
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
      TIM_Cmd(TIM3, DISABLE);
      scheduled_func();
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
