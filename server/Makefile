
ifeq ($(CROSS_COMPILE),aarch64-none-linux-gnu-)
		XCOMP := aarch64-none-linux-gnu-
else
		XCOMP :=
endif

CC := $(XCOMP)gcc
CFLAGS := -Wall -Werror
SRC := aesdsocket.c 
TARGET = aesdsocket
OBJS := $(SRC:.c=.o)


all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	-rm -f *.o $(TARGET) *.elf *.map