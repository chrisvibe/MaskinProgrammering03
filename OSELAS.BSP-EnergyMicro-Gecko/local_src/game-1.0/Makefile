#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
CFLAGS += -O2 -g -Wall #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
CPPFLAGS +=
LDFLAGS +=


all: game

clean:
	-rm -f game

install:


.PHONY: all install clean

# end of development makefile

# -------------------------------
# added this:
.PHONY : pretty
pretty :
	-indent *.c *.h

.PHONY : clean
clean :
	-rm -rf *.o *.elf *.bin *.hex *.out
