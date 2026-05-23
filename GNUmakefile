CC = gcc
CFLAGS = -std=gnu23 -O2 -Wall -MMD -MP

SRC_DIR = src

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET = shio

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(SRC_DIR)/*.o $(SRC_DIR)/*.d $(TARGET)

#main: src/main.c
#	$(CC) $(CFLAGS) src/main.c -o xc
