#ifndef BUTTONS_C
#define BUTTONS_C 1

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
#include "inc/buttons.h"
#include "inc/scheduler.h"

void (*funct1)(void);
void (*funct2)(void);
void (*funct3)(void);

static void button_interrupts(void);
void EXTI0_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);

void buttons_run_functions(void (*f1)(void), void (*f2)(void), void (*f3)(void))
{
  funct1 = f1;
  funct2 = f2;
  funct3 = f3;
  button_interrupts();
}


void EXTI0_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
      cancel(funct1);
      cancel(funct2);
      cancel(funct3);
      schedule(funct1, 1000);
      EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void EXTI2_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
      cancel(funct1);
      cancel(funct2);
      cancel(funct3);
      schedule(funct2, 1000);
      EXTI_ClearITPendingBit(EXTI_Line2);
  }
}

void EXTI3_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
      cancel(funct1);
      cancel(funct2);
      cancel(funct3);
      schedule(funct3, 1000);
      EXTI_ClearITPendingBit(EXTI_Line3);
  }
}

void button_interrupts(void)
{
  GPIO_InitTypeDef A;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  A.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3;
  A.GPIO_Mode  = GPIO_Mode_IN;
  A.GPIO_OType = GPIO_OType_PP;
  A.GPIO_Speed = GPIO_Speed_100MHz;
  A.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &A);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  EXTI_InitTypeDef EXTI_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);  
  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);
  
  NVIC_EnableIRQ(EXTI0_IRQn);
  
  NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);  
  EXTI_InitStruct.EXTI_Line = EXTI_Line2;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);
  
  NVIC_EnableIRQ(EXTI2_IRQn);
  
  NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);  
  EXTI_InitStruct.EXTI_Line = EXTI_Line3;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);
  
  NVIC_EnableIRQ(EXTI2_IRQn);
  
  NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
};

#endif
