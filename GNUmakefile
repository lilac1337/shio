CC = gcc
CFLAGS = -std=gnu23 -O0 -Wall -Wextra -Wpedantic -Werror -Wwrite-strings -Wconversion -Wsign-conversion -Wshadow -Wundef -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wstrict-prototypes -Wmissing-prototypes -Wimplicit-fallthrough -Wcast-align -Wcast-qual -Wmissing-declarations -Wredundant-decls -Wbad-function-cast -Wlogical-op -Wduplicated-cond -Wduplicated-branches -Wformat-truncation -Wstringop-overflow -fanalyzer -g3 -fsanitize=address,undefined -fno-omit-frame-pointer -MMD -MP

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
