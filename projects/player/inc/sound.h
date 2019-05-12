#ifndef SOUND_H
#define SOUND_H 1

void init_sound(void);
void init_dma(volatile uint16_t *data_for_dma_ptr);
void enable_volume_change(volatile int8_t *vol_addr);
void volume_up(void);
void volume_down(void);
void data_motion(size_t *dataStartAddr, volatile uint8_t **wav_current_addr, volatile uint16_t *data_for_dma, volatile uint8_t *play);
void enable_data_motion(void);

#endif
