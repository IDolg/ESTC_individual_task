#include "inc/audio_wav.h"
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

int wav_parse_headers(const uint32_t* fptr, WavHdr_t* phdr, size_t** p_data_offset)
{
  char data_id[4] = "data";
  uint32_t chunk_id;
  uint32_t size;
  size_t f_offset, f_size;
  int data_found = 0;

  if (!fptr || !phdr || !p_data_offset)
  {
    return 0;
  }

  memcpy(phdr, fptr, sizeof(WavHdr_t));
  f_size = (size_t) phdr->chunkSize;
  f_offset = (size_t) fptr;

  /* Seek for data section */
  memcpy(&chunk_id, &phdr->subchunk2Id, sizeof(uint32_t));
  size = phdr->subchunk2Size;
  f_offset += sizeof(WavHdr_t);

  while (f_offset < (size_t) fptr + f_size + 8)
  {
    if (memcmp(&chunk_id, data_id, sizeof(uint32_t)) == 0)
    {
      data_found = 1;
      phdr->subchunk2Id = chunk_id;
      phdr->subchunk2Size = size;
      *p_data_offset = (size_t*)f_offset; 
      break;
    }
    else
    {
      f_offset += size;
      chunk_id = _be32_to_le32( *((uint32_t*) f_offset) );
      memcpy(&chunk_id, (void*) f_offset, sizeof(uint32_t));
      f_offset += sizeof(uint32_t);
      size = *((uint32_t*) f_offset);
      f_offset += sizeof(uint32_t);
    }
  }

  return data_found;
}; 


uint32_t _be32_to_le32(uint32_t be_data)
{
  uint32_t le_data;

  le_data = ((be_data & 0xFF000000) >> 24) |
            ((be_data & 0x00FF0000) >> 8) |
            ((be_data & 0x0000FF00) << 8)  |
            (be_data & 0x000000FF) << 24;
  return le_data;
};
