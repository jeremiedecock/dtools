CC = g++

LIBS = `pkg-config --libs gmodule-export-2.0 gtk+-2.0` -ldynamixel

SUBDIR = src

.PHONY: gtkui $(SUBDIR) clean

all: gtkui $(SUBDIR)

gtkui: $(SUBDIR)
	$(CC) -o gtkui src/gtkui.o $(LIBS)

$(SUBDIR):
	$(MAKE) --directory=$@

clean:
	rm -f gtkui
	$(MAKE) clean --directory=src

