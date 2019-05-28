#ifndef LED_INDICATION_C
#define LED_INDICATION_C 1

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "inc/led_indication.h"
#include "inc/scheduler.h"

static uint8_t led_is_on = 0;

void enable_led_indication(void)
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

void blink_green()
{ 
  GPIO_ResetBits(GPIOD, GPIO_Pin_13); // turn the red led OFF  
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
  schedule(blink_green, 1000);  
}

void blink_red()
{
  GPIO_SetBits(GPIOD, GPIO_Pin_13); // turn the red led ON
  cancel(blink_green); // stop blinking
  GPIO_ResetBits(GPIOD, GPIO_Pin_12); // make sure green led is off
}

#endif
