TARGET_EXEC ?= openmenu.elf

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

include ./Makefile.config

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

default: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) $(BUILD_DIR)/romdisk.o
	@echo "Linking $(TARGET_EXEC)"
	@$(CXX) $(OBJS) $(BUILD_DIR)/romdisk.o -o $@ $(DREAM_LIBS) -Wl,-Ttext=0x8CE00000
	@echo "Converting $(TARGET_EXEC) to Binary"
	@sh-elf-objcopy -R .stack -O binary $@ $(basename $@)
	
.PHONY: scramble
scramble: $(BUILD_DIR)/$(TARGET_EXEC)
	scramble $(BUILD_DIR)/$(basename $(TARGET_EXEC)) $(BUILD_DIR)/1ST_READ.BIN
	
# assembly
$(BUILD_DIR)/%.s.o: %.s
	@$(MKDIR_P) $(dir $@)
	@echo $(AS) $<
	@$(AS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	@$(MKDIR_P) $(dir $@)
	@echo $(CC) $< 
	@$(CC) -c $< -o $@ -Wl,-Ttext=0x8CE00000 -std=c99

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@echo $(CXX) $<
	@$(CXX) -c $< -o $@ -Wl,-Ttext=0x8CE00000 
	
# romdisk rules
$(BUILD_DIR)/romdisk.img:
	@$(KOS_GENROMFS) -f $(BUILD_DIR)/romdisk.img -d romdisk

$(BUILD_DIR)/romdisk.o: $(BUILD_DIR)/romdisk.img
	@echo "Generated romdisk"
	@$(KOS_BASE)/utils/bin2o/bin2o $(BUILD_DIR)/romdisk.img romdisk $(BUILD_DIR)/romdisk.o

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p