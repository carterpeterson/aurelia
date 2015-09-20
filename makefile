CC=gcc
CFLAGS=-Wall -lpthread -g -I /opt/X11/include -L /opt/X11/lib/ -lX11 -lm
SIMULATION_DEFINES=-D SIMULATED -D DEBUG_PRINT

all: simulation

simulation:
	$(CC) $(CFLAGS) $(SIMULATION_DEFINES) ./simulator/jelly_manager.c ./simulator/render.c ./simulator/init.c ./device/*.c -o aurelia

clean:
	rm aurelia
