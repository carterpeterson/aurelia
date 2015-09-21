# i recognize this makefile is horrible, i'm gonna fix it up later

CC=gcc
CFLAGS=-Wall -lpthread -g -I /opt/X11/include -L /opt/X11/lib/ -lX11 -lm
SIMULATION_DEFINES=-D SIMULATED
DEBUG_DEFINES=-D DEBUG_PRINT

all: simulation

debug_print:
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) $(SIMULATION_DEFINES) ./simulator/jelly_manager.c ./simulator/input.c ./simulator/render.c ./simulator/init.c ./device/*.c -o aurelia

simulation:
	$(CC) $(CFLAGS) $(SIMULATION_DEFINES) ./simulator/jelly_manager.c ./simulator/input.c ./simulator/render.c ./simulator/init.c ./device/*.c -o aurelia

clean:
	rm aurelia
