CC = gcc
CFLAGS = -Wall -g -lreadline -pthread
TARGET = averageCities
SOURCES = averageCities.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean