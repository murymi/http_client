ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

TARGET_EXEC := bin

BUILD_DIR := ./build
SRC_DIRS := ./src

SRCS := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.s' -or -name '*.cpp')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP

CC := gcc

CFLAGS := -Wall -Wextra -ljson-c `mysql_config --libs` `mysql_config --cflags`

#run: $(BUILD_DIR)/$(TARGET_EXEC)
#	./$(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo "Linking $@"
	$(V_AT)$(CXX) -g $(OBJS) -o $@ $(LDFLAGS) $(CFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	@echo "Compiling $<"
	$(V_AT)mkdir -p $(dir $@)
	$(V_AT)$(CC) -g $(CPPFLAGS) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo "Compiling $<"
	$(V_AT)mkdir -p $(dir $@)
	$(V_AT)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)