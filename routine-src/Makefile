BUILD=../build/
INC_DIR=-I../rain-src/include
CFLAGS=-Wall -O -g -Wl,-E -pg
SHARED=-fPIC -shared
CC = gcc
all:$(BUILD)libtest.so
$(BUILD)libtest.so:./test_routine/test_main.c
	$(CC) $(CFLAGS) $(INC_DIR) $(SHARED)  -o $@ $^ 
clean:
	rm $(BUILD)libtest.so