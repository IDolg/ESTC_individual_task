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
void TIM3_IRQHandler(void);

volatile uint8_t scheduler_flag = 0;
struct task
{
    void (*funct) (void);
    uint32_t time;
}task_n;

struct task task1;
struct task task_arr[5];

void voidfunc(void){};

void schedule(void (*func) (void), uint32_t time)
{
  for(int i = 0;i<5; i++)
    {
      if(task_arr[i].funct == voidfunc)
       {
        task_arr[i].funct = func;
        task_arr[i].time = time;
        i = 6; // exit this cycle
       }
    }
}

void cancel(void (*func) (void))
{
    for(int i = 0;i<5; i++)
    {
      if(task_arr[i].funct == func)
       {
        task_arr[i].funct = voidfunc;
        task_arr[i].time = 1000;
       }
    }
}

void init_scheduler(void)
{
  TIM_TimeBaseInitTypeDef timer;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
  TIM_TimeBaseStructInit(&timer);
  timer.TIM_Prescaler = 1;  
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
  TIM_Cmd(TIM3, ENABLE);
  
  for(int i = 0; i<5; i++)
    {
      task_arr[i].funct = voidfunc;
      task_arr[i].time = 0;
    }
}

void TIM3_IRQHandler()
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        for(int i = 0; i<5; i++)
          {
            if(task_arr[i].time > 0)
              {
                task_arr[i].time -= 1;
              }
          }   
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

void scheduler_iteration(void)
{
  for(int i = 0; i < 5; i++)
    {
      if(task_arr[i].time < 1)
        {
          task_arr[i].funct();
          task_arr[i].funct = voidfunc;
        }
    }
}
