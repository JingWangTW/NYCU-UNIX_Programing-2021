CC 				= gcc

SRC_DIR			= src
BUILD_DIR		= build
INCLUE_DIR	 	= include

EXTRA_CCFLAGS 	= -Wall -Werror -Wextra

MAIN			= main.c
SRCS			= $(wildcard $(SRC_DIR)/*.c) $(MAIN)
OBJS			= $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPENDS			= $(patsubst %.o, %.d, $(OBJS))

CC_FLAGS		= $(EXTRA_CCFLAGS) -I$(INCLUE_DIR)

TARGET			= hw1

.PHONY: all clean

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

-include $(DEPENDS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -c -MMD $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TARGET)
