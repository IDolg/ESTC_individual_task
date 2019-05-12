BUILD_ROOT=../..
OBJS = main.o sound.o audio_wav.o led_indication.o buttons.o scheduler.o 

BINARY_NAME = main

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

$(BINARY_NAME): $(BINARY_NAME).bin

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map

flash: $(BINARY_NAME)
	$(FLASH_BINARY)

all: $(BINARY_NAME)
	#$(FLASH_BINARY)	
