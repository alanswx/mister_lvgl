# makefile to fail if any command in pipe is failed.
SHELL = /bin/bash -o pipefail

# using gcc version 5.4.1 20161213 (Linaro GCC 5.4-2017.01-rc2)
BASE    = arm-linux-gnueabihf

CC      = $(BASE)-gcc
LD      = $(BASE)-ld
STRIP   = $(BASE)-strip

ifeq ($(V),1)
	Q :=
else
	Q := @
endif

INCLUDE	= -I./
INCLUDE	+= -I./lvgl



PRJ = FBDemo 
SRC = $(wildcard *.c)
SRC2 = $(wildcard *.cpp)
IMG = $(wildcard *.png)
LVGL_SRC	= $(wildcard ./lvgl/*/*.c)
LVGL_DEMO_SRC	= $(wildcard ./lv_examples/lv_apps/demo/*.c)
LVGL_DEMO2_SRC	= $(wildcard ./DONTUSElv_examples/lv_tests/lv_test_group/*.c)
LVGL_DRIVER_SRC	= $(wildcard ./lv_drivers/display/*.c)
LVGL_DRIVER2_SRC= $(wildcard ./lv_drivers/indev/*.c)

#IMLIB2_LIB  = -lfreetype  -lpng16 -lz 
IMLIB2_LIB  = 

VPATH	= ./:./support/minimig:./support/sharpmz:./support/archie:./support/st:./support/x86:./support/snes

OBJ	= $(SRC:.c=.c.o) $(SRC2:.cpp=.cpp.o) $(IMG:.png=.png.o) $(LVGL_SRC:.c=.c.o) $(LVGL_DRIVER2_SRC:.c=.c.o) $(LVGL_DRIVER_SRC:.c=.c.o) $(LVGL_DEMO_SRC:.c=.c.o) $(LVGL_DEMO2_SRC:.c=.c.o) 
DEP	= $(SRC:.c=.cpp.d) $(SRC2:.cpp=.cpp.d) $(LVGL_SRC:.c=.c.d) $(LVGL_DEMO_SRC:.c=.c.d) $(LVGL_DEMO2_SRC:.c=.c.d) $(LVGL_DRIVER_SRC:.c=.c.d) $(LVGL_DRIVER2_SRC:.c=.c.d)

DFLAGS	= $(INCLUDE) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -DVDATE=\"`date +"%y%m%d"`\" -DONION_VERSION=\"0.8.123.f6b9d.dirty\" -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112L
CFLAGS	= $(DFLAGS) -Wall -Wextra -Wno-strict-aliasing -c -O3
LFLAGS	= -lc -lstdc++ -lrt $(IMLIB2_LIB)

#parse.h: /usr/include/linux/input.h
#	@echo generating parse.h
#	@echo -en "struct parse_key {\n\tchar *name;\n\tunsigned int value;\n} " >parse.h
#	@echo -en "keynames[] = {\n" >>parse.h
#	@more /usr/include/linux/input-event-codes.h |perl -n \
#	-e 'if (m/^\#define\s+(KEY_[^\s]+)\s+(0x[\d\w]+|[\d]+)/) ' \
#	-e '{ printf "\t{\"%s\", %s},\n",$$1,$$2; }' \
#	-e 'if (m/^\#define\s+(BTN_[^\s]+)\s+(0x[\d\w]+|[\d]+)/) ' \
#	-e '{ printf "\t{\"%s\", %s},\n",$$1,$$2; }' \
#	>> parse.h
#	@echo -en "\t{ NULL, 0}\n};\n" >>parse.h

$(PRJ): $(OBJ)
	$(Q)$(info $@)
	$(Q)$(CC) -o $@ $+ $(LFLAGS)
	$(Q)cp $@ $@.elf
	$(Q)$(STRIP) $@

clean:
	$(Q)rm -f *.elf *.map *.lst *.user *~ $(PRJ)
	$(Q)rm -rf obj DTAR* x64
	$(Q)find . \( -name '*.o' -o -name '*.d' -o -name '*.bak' -o -name '*.rej' -o -name '*.org' \) -exec rm -f {} \;

cleanall:
	$(Q)rm -rf $(OBJ) $(DEP) *.elf *.map *.lst *.bak *.rej *.org *.user *~ $(PRJ)
	$(Q)rm -rf obj DTAR* x64
	$(Q)find . -name '*.o' -delete
	$(Q)find . -name '*.d' -delete

%.c.o: %.c
	$(Q)$(info $<)
	$(Q)$(CC) $(CFLAGS) -std=gnu99 -o $@ -c $< 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

%.cpp.o: %.cpp
	$(Q)$(info $<)
	$(Q)$(CC) $(CFLAGS) -std=gnu++14 -o $@ -c $< 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

%.png.o: %.png
	$(Q)$(info $<)
	$(Q)$(LD) -r -b binary -o $@ $< 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

-include $(DEP)
%.c.d: %.c
	$(Q)$(CC) $(DFLAGS) -MM $< -MT $@ -MT $*.c.o -MF $@ 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

%.cpp.d: %.cpp
	$(Q)$(CC) $(DFLAGS) -MM $< -MT $@ -MT $*.cpp.o -MF $@ 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

# Ensure correct time stamp
main.cpp.o: $(filter-out main.cpp.o, $(OBJ))
