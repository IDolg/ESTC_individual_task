#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"
#include <stm32f4xx_dma.h>
#include "inc/audio_wav.h"
#include <string.h>
#include "inc/sound.h"

#define Audio_DMA_I2S3_Stream DMA1_Stream5
#define Audio_DMA_I2S3_Channel DMA_Channel_0

#define WAV_START_BLOCK ((uint16_t*) 0x08010000)
#define WAV_END_BLOCK ((uint16_t*) 0x080FFFF0)

static size_t *start_addr;
static volatile size_t *current_addr;
static size_t *end_addr;

static void start_dma(void);
static void init_gpio(void); 
static void init_i2s(void);
static void init_i2c(void);
static void init_cs32l22(void);
static void delay(uint32_t delayTime);
static void write_i2c_data(uint8_t bytesToSend[], uint8_t numOfBytesToSend);
void DMA1_Stream5_IRQHandler(void);

void play_this(size_t *start_addr1, size_t *end_addr1)
{
  start_addr = start_addr1;
  current_addr = start_addr1;
  end_addr = end_addr1;
  
  start_dma();
}


void init_sound()
{
  FLASH_SetLatency(FLASH_Latency_4);
  init_gpio();
  init_i2c();
  init_i2s();
  init_cs32l22();
};

void init_gpio()
{
  // Включаем тактирование
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB1Periph_SPI3, ENABLE);
  // свой источник тактирования I2S 
  RCC_PLLI2SCmd(ENABLE);
 
  GPIO_InitTypeDef gpio;
  // Reset сигнал для CS43L22 
  gpio.GPIO_Pin = GPIO_Pin_4;;
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &gpio);
 
  // Выводы I2C1
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_OType = GPIO_OType_OD;
  gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gpio);
 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
 
  // выводы I2S  
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12;
  GPIO_Init(GPIOC, &gpio);
 
  gpio.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &gpio);
 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
 
  // Сбрасываем Reset в ноль
  GPIO_ResetBits(GPIOD, GPIO_Pin_4);
};

void init_i2s()
{
  I2S_InitTypeDef i2s;
  SPI_I2S_DeInit(SPI3);
  i2s.I2S_AudioFreq = I2S_AudioFreq_16k;
  i2s.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  i2s.I2S_DataFormat = I2S_DataFormat_16b;
  i2s.I2S_Mode = I2S_Mode_MasterTx;
  i2s.I2S_Standard = I2S_Standard_Phillips;
  i2s.I2S_CPOL = I2S_CPOL_Low;
 
  I2S_Init(SPI3, &i2s);
  I2S_Cmd(SPI3, ENABLE);
    
//SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
};

void init_i2c()
{
  I2C_InitTypeDef i2c;
  I2C_DeInit(I2C1);
  i2c.I2C_ClockSpeed = 100000;
  i2c.I2C_Mode = I2C_Mode_I2C;
  i2c.I2C_OwnAddress1 = 0x33;
  i2c.I2C_Ack = I2C_Ack_Enable;
  i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2c.I2C_DutyCycle = I2C_DutyCycle_2;
 
  I2C_Cmd(I2C1, ENABLE);
  I2C_Init(I2C1, &i2c);
}

void delay(uint32_t delayTime)
{
  uint32_t i = 0;
  for (i = 0; i < delayTime; i++);
};

void write_i2c_data(uint8_t bytesToSend[], uint8_t numOfBytesToSend)
{
  uint8_t currentBytesValue = 0;
 
  // Ждем пока шина освободится
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  // Генерируем старт
  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
  // Посылаем адрес подчиненному устройству - микросхеме CS43L22
  I2C_Send7bitAddress(I2C1, 0x94, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 
  // И наконец отправляем наши данные
  while (currentBytesValue < numOfBytesToSend)
    {
	  I2C_SendData(I2C1, bytesToSend[currentBytesValue]);
	  currentBytesValue++;
	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
    }
 
  while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
  I2C_GenerateSTOP(I2C1, ENABLE);
};

void init_cs32l22()
{
  uint8_t sendBuffer[2];
  GPIO_SetBits(GPIOD, GPIO_Pin_4);
 
  delay(0xFFFF);
  delay(0xFFFF);
  delay(0xFFFF);
  delay(0xFFFF);
  delay(0xFFFF);
    
  sendBuffer[0] = 0x0D;
  sendBuffer[1] = 0x01;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x00;
  sendBuffer[1] = 0x99;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x47;
  sendBuffer[1] = 0x80;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x32;
  sendBuffer[1] = 0xFF;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x32;
  sendBuffer[1] = 0x7F;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x00;
  sendBuffer[1] = 0x00;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x04;
  sendBuffer[1] = 0xAF;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x0D;
  sendBuffer[1] = 0x70;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x05;
  sendBuffer[1] = 0x81;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x06;
  sendBuffer[1] = 0x07;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x0A;
  sendBuffer[1] = 0x00;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x27;
  sendBuffer[1] = 0x00;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x1A;
  sendBuffer[1] = 0x0A;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x1B;
  sendBuffer[1] = 0x0A;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x1F;
  sendBuffer[1] = 0x0F;
  write_i2c_data(sendBuffer, 2);
 
  sendBuffer[0] = 0x02;
  sendBuffer[1] = 0x9E;
  write_i2c_data(sendBuffer, 2);
    
  sendBuffer[0] = 0x20;  // volume to max = 18
  sendBuffer[1] = 0x00;
  write_i2c_data(sendBuffer, 2);
    
  sendBuffer[0] = 0x21;  //
  sendBuffer[1] = 0x00;
  write_i2c_data(sendBuffer, 2);
};

void set_volume(int8_t vol)
{
  uint8_t sendBuffer[2];
  sendBuffer[0] = 0x20;  // volume to max = 18
  sendBuffer[1] = vol;
  write_i2c_data(sendBuffer, 2);
    
  sendBuffer[0] = 0x21;  
  sendBuffer[1] = vol;
  write_i2c_data(sendBuffer, 2);
};

void start_dma() 
{
  DMA_InitTypeDef DMA_InitStructure;
  // Enable DMA
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  // Common initialization
  DMA_InitStructure.DMA_BufferSize = 500;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord; 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //Circular
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;

  DMA_Cmd(Audio_DMA_I2S3_Stream, DISABLE); 
  DMA_DeInit(Audio_DMA_I2S3_Stream);
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Channel = Audio_DMA_I2S3_Channel; 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &SPI3->DR; // SPI data register for sending
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) current_addr;
  DMA_Init(Audio_DMA_I2S3_Stream, &DMA_InitStructure);
    
  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
  DMA_ITConfig(DMA1_Stream5, DMA_IT_TC , ENABLE);
  DMA_Cmd(Audio_DMA_I2S3_Stream, ENABLE);
  
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void DMA1_Stream5_IRQHandler(void)
{
  DMA_ClearFlag(DMA1_Stream5, DMA_IT_TC);  
  if(current_addr + 250 < end_addr) 
    {
      current_addr += 250;       
    }
  if(current_addr + 250 >= end_addr)
    {
        current_addr = start_addr;
    }
  start_dma();
}

void start_playing(void)
{
  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
  DMA_ITConfig(DMA1_Stream5, DMA_IT_TC , ENABLE);
  DMA_Cmd(Audio_DMA_I2S3_Stream, ENABLE);
}

void stop_playing(void)
{
  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);
  DMA_ITConfig(DMA1_Stream5, DMA_IT_TC , DISABLE);
  DMA_Cmd(Audio_DMA_I2S3_Stream, DISABLE);
}
