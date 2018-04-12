CC = gcc
CCFLAGS = -W -Wall

BINARY = thermistor-calculator
LIBRARIES = -lm
SOURCES = Main.c

all: $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) $(LIBRARIES) -o $(BINARY)

clean:
	rm -f $(BINARY)
