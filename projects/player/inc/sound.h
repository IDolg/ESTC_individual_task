#ifndef SOUND_H
#define SOUND_H 1

void init_sound(void);
void start_playing(void);
void enable_volume_change(volatile int8_t *vol_addr);
void sound_data(size_t *dataStartAddr, volatile uint16_t **wav_current_addr, volatile uint16_t *data_for_dma, volatile uint8_t *play);
int8_t set_volume(int8_t vol);

#endif
