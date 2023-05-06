CONFIG_DIR      := config
SRC_DIR         := src
ASSETS_DIR      := assets
INCLUDE_DIR     := include
MAIN_NAME = main.exe
BUILD_DIR := build
DISK_DIR := $(BUILD_DIR)/disk
CROSS           := mipsel-unknown-linux-gnu-
AS              := $(CROSS)as -EL
LD              := $(CROSS)ld -EL
OBJCOPY         := $(CROSS)objcopy
CPP				:= $(CROSS)cpp

# CC := wine /tmp/silent-hill-decomp/tools/psyq/4.6/CC1PSX.EXE
CC = tools/cc1-26


ASM_DIR_BOOT	:= asm/tenchu/main.exe asm/tenchu/main.exe/data
C_DIR_BOOT		:= src/tenchu/main.exe
BIN_DIR_BOOT	:= assets/tenchu/main.exe

# S_FILES_BOOT	:= $(foreach dir,$(ASM_DIR_BOOT),$(wildcard $(dir)/*.s))
# C_FILES_BOOT	:= $(foreach dir,$(C_DIR_BOOT),$(wildcard $(dir)/*.c))
# BIN_FILES_BOOT	:= $(foreach dir,$(BIN_DIR_BOOT),$(wildcard $(dir)/*.bin))

# O_FILES_BOOT	:= $(foreach file,$(S_FILES_BOOT),$(BUILD_DIR)/$(file).o) \
# 					$(foreach file,$(C_FILES_BOOT),$(BUILD_DIR)/$(file).o) \
# 					$(foreach file,$(BIN_FILES_BOOT),$(BUILD_DIR)/$(file).o)

# ASM_DIRS_ALL	:= $(ASM_DIR_BOOT)
# C_DIRS_ALL		:= $(C_DIR_BOOT)
# BIN_DIRS_ALL	:= $(BIN_DIR_BOOT)

# Flags
OPT_FLAGS       := -O2
INCLUDE_CFLAGS	:= -Iinclude
# AS_FLAGS        := -march=r3000 -mtune=r3000 -Iinclude
D_FLAGS       	:= -D_LANGUAGE_C
# CC_FLAGS        := -G 0 -mips1 -mcpu=3000 -mgas -msoft-float $(OPT_FLAGS) -fgnu-linker
# CC_FLAGS        := -G 0 -mips1 -msoft-float $(OPT_FLAGS)
# CPP_FLAGS       := -undef -Wall -lang-c $(DFLAGS) $(INCLUDE_CFLAGS) -nostdinc
OBJCOPY_FLAGS   := -O binary

AS_FLAGS        += -Iinclude -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0
CC_FLAGS        += -mcpu=3000 -quiet -G0 -w -O2 -funsigned-char -fpeephole -ffunction-cse -fpcc-struct-return -fcommon -fverbose-asm -fgnu-linker -mgas -msoft-float
CPP_FLAGS       += -Iinclude -undef -Wall -lang-c -fno-builtin -gstabs
CPP_FLAGS       += -Dmips -D__GNUC__=2 -D__OPTIMIZE__ -D__mips__ -D__mips -Dpsx -D__psx__ -D__psx -D_PSYQ -D__EXTENSIONS__ -D_MIPSEL -D_LANGUAGE_C -DLANGUAGE_C -DHACKS

ASM_DIR := asm
SRC_DIR := src
MAIN_ASM_DIRS   := $(ASM_DIR)/tenchu/$(MAIN) # $(ASM_DIR)/tenchu/$(MAIN)/data
MAIN_SRC_DIRS   := $(SRC_DIR) 

MAIN_S_FILES    := $(foreach dir,$(MAIN_ASM_DIRS),$(wildcard $(dir)/*.s)) \
					$(foreach dir,$(MAIN_ASM_DIRS),$(wildcard $(dir)/**/*.s))
MAIN_C_FILES    := $(foreach dir,$(MAIN_SRC_DIRS),$(wildcard $(dir)/*.c)) \
					$(foreach dir,$(MAIN_SRC_DIRS),$(wildcard $(dir)/**/*.c))
MAIN_O_FILES    := $(foreach file,$(MAIN_S_FILES),$(BUILD_DIR)/$(file).o) \
					$(foreach file,$(MAIN_C_FILES),$(BUILD_DIR)/$(file).o)


# dirs:
# 	$(foreach dir,$(ASM_DIRS_ALL) $(C_DIRS_ALL) $(BIN_DIRS_ALL),$(shell mkdir -p $(BUILD_DIR)/$(dir)))

# mkdir -p build/asm/tenchu/main.exe/ build/asm/tenchu/main.exe/data build/src/tenchu/main.exe


# Rules
extract: extract_slps_019.01 extract_main.exe

extract_slps_019.01:
	split.py $(CONFIG_DIR)/splat.slps_019.01.yaml

extract_main.exe:
	split.py $(CONFIG_DIR)/splat.main.exe.yaml

clean:
	rm -r asm/* linker/* meta/* src/* build/*

main.exe: main.exe.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

# main.exe.elf: $(O_FILES_BOOT)
# 	$(LD) -Map $(TARGET_BOOT).map -T linker/$(MAIN_NAME).ld -T meta/undefined_symbols_auto.$(MAIN_NAME).txt -T meta/undefined_functions_auto.$(MAIN_NAME).txt -T meta/undefined_symbols.$(MAIN_NAME).txt --no-check-sections -o $@

main.exe.elf: $(MAIN_O_FILES)
	echo $(MAIN_S_FILES) $(MAIN_O_FILES)
	mkdir -p $(BUILD_DIR)/tenchu
	$(LD) -o $@ \
	-Map $(BUILD_DIR)/tenchu/main.exe.map \
	-T linker/main.exe.ld \
	-T <( cut -d '/' -f1 $(CONFIG_DIR)/symbols.main.exe.txt ) \
	-T meta/undefined_symbols_auto.main.exe.txt \
	--no-check-sections \
	-nostdlib \
	-s

# generate objects
# $(BUILD_DIR)/%.i: %.c
# 	mkdir -p $$(dirname $@)
# 	$(CPP) -P -MMD -MP -MT $@ -MF $@.d $(CPP_FLAGS) -o $@ $<

# $(BUILD_DIR)/%.c.s: $(BUILD_DIR)/%.i
# 	$(CC) $(CC_FLAGS) -o $@ $<

# $(BUILD_DIR)/%.c.o: $(BUILD_DIR)/%.c.s
# 	$(AS) $(AS_FLAGS) -o $@ $<
# $(BUILD_DIR)/%.c.o: %.c $(ASPATCH)
# 	$(CPP) $(CPP_FLAGS) $< | $(CC) $(CC_FLAGS) | $(ASPATCH) | $(AS) $(AS_FLAGS) -o $@


$(BUILD_DIR)/%.s.o: %.s
	mkdir -p $$(dirname $@)
	$(AS) $(AS_FLAGS) -o $@ $<

$(BUILD_DIR)/%.c.o: %.c 
	mkdir -p $$(dirname $@)
	$(CPP) $(CPP_FLAGS) $< | $(CC) $(CC_FLAGS) | $(AS) $(AS_FLAGS) -o $@

# $(BUILD_DIR)/%.s.o: %.s
# 	mkdir -p $$(dirname $@)
# 	$(AS) $(AS_FLAGS) -o $@ $<

# $(BUILD_DIR)/%.bin.o: %.bin
# 	$(LD) -r -b binary -o $@ $<


# SHELL = /usr/bin/env bash -e -o pipefail
SHELL = /run/current-system/sw/bin/bash -e -o pipefail