# Dung dau '>' thay cho Tab de tranh loi khi copy/paste
.RECIPEPREFIX = >

TARGET  = app
BUILD   = build
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU     = -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
CFLAGS  = $(CPU) -Os -g -Wall -Wextra -ffreestanding \
          -fdata-sections -ffunction-sections -MMD -MP -Iinclude
LDFLAGS = $(CPU) -nostdlib -T linker/linker.ld -Wl,--gc-sections \
          -Wl,-Map=$(BUILD)/$(TARGET).map

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,$(BUILD)/%.o,$(SRCS))

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin
> $(SIZE) $(BUILD)/$(TARGET).elf

$(BUILD):
> python -c "import os; os.makedirs('$(BUILD)', exist_ok=True)"

$(BUILD)/%.o: src/%.c | $(BUILD)
> $(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS)
> $(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
> $(OBJCOPY) -O binary $< $@

flash: all
> openocd -f interface/stlink.cfg -f target/stm32h7x.cfg -c "program $(BUILD)/$(TARGET).elf verify reset exit"

debug:
> openocd -f interface/stlink.cfg -f target/stm32h7x.cfg

clean:
> python -c "import shutil; shutil.rmtree('$(BUILD)', ignore_errors=True)"

-include $(OBJS:.o=.d)

.PHONY: all flash debug clean