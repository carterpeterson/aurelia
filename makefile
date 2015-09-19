CC=gcc
CFLAGS=-Wall -lpthread -g
SIMULATION_DEFINES=-D SIMULATED -D DEBUG_PRINT

all: simulation

simulation:
	$(CC) $(CFLAGS) $(SIMULATION_DEFINES) ./simulator/jelly_manager.c ./simulator/init.c ./device/*.c -o aurelia

clean:
	rm aurelia
