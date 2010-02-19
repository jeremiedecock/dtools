CC = g++
CFLAGS = -Wall -pedantic -g -c

OBJECTS = change_baudrate.o dump_ax12.o monitor_ax12.o ping_ax12.o reset_ax12.o switch_off_ax12.o switch_on_ax12.o

.PHONY: all clean

all: $(OBJECTS)

%.o : %.cpp
	$(CC) $(CFLAGS) $<

clean:
	rm -rf $(OBJECTS)

