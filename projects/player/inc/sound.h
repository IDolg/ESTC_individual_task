#ifndef SOUND_H
#define SOUND_H 1

void init_sound(void);
void enable_volume_change(volatile int8_t *vol_addr);
void play_this(size_t *start_addr1, size_t *end_addr1);
void set_volume(int8_t vol);
void start_playing(void);
void stop_playing(void);

#endif
