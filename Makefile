TARGET_EXEC ?= pngpack

BUILD_DIR ?= ./build
SRC_DIRS ?= ./jsoncpp/src/lib_json ./pngpacker
EXCLUDE_DIRS ?= ./lodepng/examples

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -or -name "*.c" -or -name "*.s") ./lodepng/lodepng.cpp 
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := ./pngpacker ./jsoncpp/include ./lodepng $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
GIT_COMMIT := $(shell git rev-parse --short HEAD)$(shell git diff-index --quiet HEAD -- || echo ' (dirty)')
GIT_TAG := $(shell git describe --abbrev=0 --tags)
COPYRIGHT_YEAR := $(shell date +"%Y")
VERSION_DEFINITIONS := -DGIT_TAG="\"$(GIT_TAG)\"" -DGIT_COMMIT="\"$(GIT_COMMIT)\"" -DCOPYRIGHT_YEAR="\"$(COPYRIGHT_YEAR)\""
CPPFLAGS ?= $(INC_FLAGS) -fdata-sections -ffunction-sections -std=c++11 $(VERSION_DEFINITIONS) $(EXTRA_CPPFLAGS)
LDFLAGS ?= -Wl,-dead_strip $(EXTRA_LDFLAGS)

all: $(BUILD_DIR)/$(TARGET_EXEC)$(EXE_EXT) $(BUILD_DIR)/$(TARGET_EXEC)-upx$(EXE_EXT)
$(BUILD_DIR)/$(TARGET_EXEC)$(EXE_EXT): $(OBJS)
	$(CROSS)g++ $(OBJS) -o $@ $(LDFLAGS)
	$(CROSS)strip $@

$(BUILD_DIR)/$(TARGET_EXEC)-upx$(EXE_EXT): $(BUILD_DIR)/$(TARGET_EXEC)$(EXE_EXT)
	-rm "$@"
	upx "$<" -o "$@" --lzma

test: testunpack testpack
testunpack: all
	clear
	mkdir -p test/packed/
	-rm test/packed/*
	$(BUILD_DIR)/$(TARGET_EXEC)$(EXE_EXT) u test/packed.json test/packed.png test/packed

testpack: all
	clear
	mkdir -p test/packed/
	$(BUILD_DIR)/$(TARGET_EXEC)$(EXE_EXT) p test/packed.json test/repacked.png test/packed

# assembly
$(BUILD_DIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	$(CROSS)as $(ASFLAGS) -c $< -o $@
	
# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CROSS)g++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: test testunpack testpack all clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
