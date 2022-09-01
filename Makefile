#----------------------------------------#
#-- Makefile for MSP430G2 Launchpad    --#
#-- Created by Nhi Van Pham @ 2018     --#
#----------------------------------------#

SRCS      := $(wildcard src/*.c)
OBJECTS   := $(subst src,build,$(SRCS:.c=.o))
DEPS      := $(subst src,build,$(SRCS:.c=.d))
BUILD_DIR := build
SRC_DIR   := src
TARGET    := cli_main

# Definition of compiler toolchain
DEVICE  := msp430fr5969
CC      := msp430-gcc -c
LD      := msp430-gcc
GDB     := msp430-gdb
FLASH   := mspdebug

#  Options for C Compiler
# Details:
#      -std=c99 -Wpedantic: Issue all the warnings demanded by strict ISO C; reject all programs that use forbidden extensions, and some other programs that do not follow ISO C.
#                           For ISO C, follows the version of the ISO C standard specified by any -std option used.
#      -Wshadow: Warn whenever a local variable or type declaration shadows another variable,
#                parameter, type, or whenever a built-in function is shadowed.
#      -Wextra: This enables some extra warning flags that are not enabled by -Wall.
#      -Werror: Make all warnings into errors.
#      -ggdb:
#      -gdwarf-2: Source-level debugging (this is the specified DWARF standard) that use a compatible debugger to load, run, and debug images.
#                  For more information --> http://www.keil.com/support/man/docs/armclang_ref/armclang_ref_chr1409753002958.htm
#      --gc-sections: Remove unused sections
CFLAGS =
CFLAGS += -mmcu=$(DEVICE)
CFLAGS += -I ./inc
CFLAGS += -O0 -g -ggdb -gdwarf-2
CFLAGS += -Wall -Wextra -Wshadow -std=c99

#  Options for Linker
#  Details:
#     -nostartfiles: Do not use the standard system startup files when linking. The standard system libraries are used normally
LFLAGS =
LFLAGS += -mmcu=$(DEVICE)
LFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map

compile: $(TARGET)

all: $(BUILD_DIR)/$(TARGET) upload

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) user_definitions.x $(OBJECTS) -o $(@).elf
	msp430-objdump -Sd -W $(@).elf > $(@).lss
	msp430-size $(@).elf
	msp430-objcopy -O ihex $(@).elf $(@).hex

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if [ ! -e build ]; then mkdir build; fi
	$(CC) $(CFLAGS) -MD -MF $(subst src,build,$(subst .c,.d,$<)) $< -c -o $@

upload:
	$(FLASH) tilib "prog $(BUILD_DIR)/$(TARGET).elf"

debug:
	clear
	@echo -e "--------------------------------------------------------------------------------"
	@echo -e "-- Make sure you are running mspdebug in another window                       --"
	@echo -e "--------------------------------------------------------------------------------"
	@echo -e "$$ # you can start it like this:"
	@echo -e "$$ mspdebug rf2500 gdb\n"
	$(GDB) $(BUILD_DIR)/$(TARGET).elf

clean:
	rm -rf build/*

.PHONY: clean upload debug compile
