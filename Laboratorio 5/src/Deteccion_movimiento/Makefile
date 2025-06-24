BINARY = firmware

CFILES = clock.c console.c
CXXFILES = spi_mems.cpp

OBJS = $(CFILES:.c=.o) $(CXXFILES:.cpp=.o)

LDSCRIPT = ../stm32f429i-discovery.ld

PREFIX ?= arm-none-eabi-
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
OBJCOPY = $(PREFIX)objcopy

OPENCM3_INC = ../../libopencm3/include

CFLAGS = -std=c99 -Os -Wall -Wextra -ffunction-sections -fdata-sections -g3 -I. -I$(OPENCM3_INC) -DSTM32F4 -DSTM32F429xx
CXXFLAGS = -std=c++17 -Os -Wall -Wextra -ffunction-sections -fdata-sections -g3 -I. -I$(OPENCM3_INC) -DSTM32F4 -DSTM32F429xx

LDFLAGS = -T$(LDSCRIPT) -nostartfiles -Wl,--gc-sections -u _printf_float -mfloat-abi=hard

LDLIBS = -lc -lgcc -lnosys -lstdc++ -lsupc++

all: $(BINARY).elf $(BINARY).bin

$(BINARY).elf: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.elf *.bin

.PHONY: all clean
