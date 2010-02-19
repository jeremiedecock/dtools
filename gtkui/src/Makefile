CC = g++
CFLAGS = `pkg-config --cflags gmodule-export-2.0 gtk+-2.0` -Wall -pedantic -g -c

OBJECTS = callbacks.o gtkui.o

.PHONY: all clean

all: $(OBJECTS) gui.xml

%.o : %.cc
	$(CC) $(CFLAGS) $<

gui.xml : gui.glade
	gtk-builder-convert gui.glade gui.xml

clean:
	rm -rf $(OBJECTS) gui.xml

