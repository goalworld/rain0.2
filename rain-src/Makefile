BUILD=../build/
INC_DIR=-I./include
CFLAGS=-Wall -O -g -Wl,-E -pg
SHARED=-fPIC -shared
CC = gcc
OBJ= ./src/rain_context.c\
			./src/rain_event_epoll.c\
			./src/rain_event.c\
			./src/rain_imp.c\
			./src/rain_lifequeue.c\
			./src/rain_loger.c\
			./src/rain_module.c\
			./src/rain_msgqueue.c\
			./src/rain_start.c\
			./src/rain_tcp.c\
			./src/rain_timer.c
all:$(BUILD)rain
$(BUILD)rain:$(OBJ)
	$(CC) $(CFLAGS) $(INC_DIR)  -o $@ $^  -lpthread -lwod -ldl
clean:
	rm $(BUILD)rain