CC 				= gcc

SRC_DIR			= src
BUILD_DIR		= build
INCLUE_DIR	 	= include

EXTRA_CCFLAGS 	= -Wall -Werror -Wextra

MAIN			= main.c
SRCS			= $(wildcard $(SRC_DIR)/*.c) $(MAIN)
OBJS			= $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

CC_FLAGS		= $(EXTRA_CCFLAGS) -I$(INCLUE_DIR)

TARGET			= hw1

.PHONY: all clean

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TARGET)
