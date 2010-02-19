CC = g++

LIBS = -ldynamixel

TOOLS = change_baudrate dump_ax12 monitor_ax12 ping_ax12 reset_ax12 switch_off_ax12 switch_on_ax12

SUBDIR = src

.PHONY: $(TOOLS) $(SUBDIR) clean

all: $(TOOLS) $(SUBDIR)

change_baudrate: $(SUBDIR)
	$(CC) -o change_baudrate src/change_baudrate.o $(LIBS)

dump_ax12: $(SUBDIR)
	$(CC) -o dump_ax12 src/dump_ax12.o $(LIBS)

monitor_ax12: $(SUBDIR)
	$(CC) -o monitor_ax12 src/monitor_ax12.o $(LIBS)

ping_ax12: $(SUBDIR)
	$(CC) -o ping_ax12 src/ping_ax12.o $(LIBS)

reset_ax12: $(SUBDIR)
	$(CC) -o reset_ax12 src/reset_ax12.o $(LIBS)

switch_off_ax12: $(SUBDIR)
	$(CC) -o switch_off_ax12 src/switch_off_ax12.o $(LIBS)

switch_on_ax12: $(SUBDIR)
	$(CC) -o switch_on_ax12 src/switch_on_ax12.o $(LIBS)

$(SUBDIR):
	$(MAKE) --directory=$@

clean:
	rm -f $(TOOLS)
	$(MAKE) clean --directory=src

