CC = gcc
CCFLAGS = -W -Wall

BINARY = thermistor-calculator
SOURCES = Main.c

all: $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) -o $(BINARY)

clean:
	rm -f $(BINARY)
