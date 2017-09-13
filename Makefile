OBJECT = c8dasm.o
TARGET = c8dasm
SOURCE = c8dasm.c

CC = gcc
LD = gcc
RM = rm
CFLAGS = -Wall -std=c99
RELEASE_CFLAGS = -std=c99 -O2 -march=native -s
DEBUG_CFLAGS = $(CFLAGS) -D_DEBUG -g
all:
	$(CC) $(CFLAGS) -c $(SOURCE) -o $(OBJECT)
	$(LD) $(OBJECT) -o $(TARGET)
	@echo "Build done!"

release:
	$(CC) $(RELEASE_CFLAGS) -c $(SOURCE) -o $(OBJECT)
	$(LD) $(OBJECT) -o $(TARGET)
	@echo "Build done!"

debug:
	$(CC) $(DEBUG_CFLAGS) -c $(SOURCE) -o $(OBJECT)
	$(LD) $(OBJECT) -o $(TARGET)
	@echo "Build done!"

clean:
	$(RM) -f $(OBJECT)
	@echo "Clean done!"

sweep:
	$(RM) -f $(OBJECT) $(TARGET)
	@echo "Sweep done!"
