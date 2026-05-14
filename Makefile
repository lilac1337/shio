CC = gcc
CFLAGS = -std=gnu23 -O2 -Wall -MMD -MP

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = shio

-include $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CLFAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

#main: src/main.c
#	$(CC) $(CFLAGS) src/main.c -o xc
