CC 				= gcc

SRC_DIR			= src
BUILD_DIR		= build
INCLUE_DIR	 	= include

EXTRA_CCFLAGS 	= -Wall -Werror -Wextra -fPIC
EXTRA_LIBS		= dl

SRCS			= $(wildcard $(SRC_DIR)/*.c)
OBJS			= $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPENDS			= $(patsubst %.o, %.d, $(OBJS))

CC_FLAGS		= $(EXTRA_CCFLAGS) $(addprefix -I, $(INCLUE_DIR)) 
LD_FLAGS		= -shared -Wl,--no-as-needed $(addprefix -l, $(EXTRA_LIBS))

TARGET_SCRIPT	= logger
TARGET_LIB		= logger.so 

.PHONY: all clean

all:$(TARGET_SCRIPT) $(TARGET_LIB)
	chmod +x $(TARGET_SCRIPT)

$(TARGET_LIB):$(OBJS)
	$(CC) $(LD_FLAGS) -o $@ $^

-include $(DEPENDS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -c -MMD $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TARGET_LIB)
	rm -rf *out*