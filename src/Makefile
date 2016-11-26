# Set to @ if you want to suppress command echo
CMD_ECHO = @

# Program name
BIN = raspicv

# Important directories
USERLAND_DIR =
BUILD_DIR = ../build

# Include paths
INC = -I. \
      -I$(USERLAND_DIR) \
      -I$(USERLAND_DIR)/interface/vcos/pthreads \
      -I$(USERLAND_DIR)/interface/vmcs_host/linux \
      -I$(USERLAND_DIR)/host_applications/linux/libs/bcm_host/include \
      -I$(USERLAND_DIR)/host_applications/linux/apps/raspicam

# C ource files outside src directory
SRC_C = $(USERLAND_DIR)/host_applications/linux/apps/raspicam/RaspiCamControl.c \
        $(USERLAND_DIR)/host_applications/linux/apps/raspicam/RaspiCLI.c \
        $(USERLAND_DIR)/host_applications/linux/apps/raspicam/RaspiPreview.c

# Defines required by included libraries
DEF = 
DEF += -DDEBUG

# Compiler and linker flags
ARCHFLAGS =
OPTFLAGS = -O3 -flto
DBGFLAGS = -ggdb

# CC: Place functions and data into separate sections to allow dead code removal
# by the linker (-f*-sections).
CFLAGS = $(ARCHFLAGS) $(DBGFLAGS) $(OPTFLAGS) -std=gnu99 -Wall -Wno-format \
         -Wno-error=unused-function -Wno-error=unused-variable \
         -ffunction-sections -fdata-sections

CXXFLAGS = $(ARCHFLAGS) $(DBGFLAGS) $(OPTFLAGS) `pkg-config --cflags opencv` \
           -std=c++0x -Wno-format -Werror -ffunction-sections -fdata-sections

# LD: Remove unused sections, link with MMAL, generate map
LDFLAGS = $(ARCHFLAGS) $(DBGFLAGS) $(OPTFLAGS) -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(BIN).map
LDFLAGS += `pkg-config --libs opencv`
LDFLAGS += -L/opt/vc/lib

LDLIBFLAGS = -Wl,--start-group -lpthread -lvcos -lbcm_host -lmmal -lmmal_core -lmmal_util -Wl,--end-group

# Generate object list from source files and add their dirs to search path
SRC_C += $(wildcard *.c) 
FILENAMES_C = $(notdir $(SRC_C))
OBJS_C = $(addprefix $(BUILD_DIR)/, $(FILENAMES_C:.c=.o))
vpath %.c $(dir $(SRC_C))

SRC_CXX += $(wildcard *.cpp) 
FILENAMES_CXX = $(notdir $(SRC_CXX))
OBJS_CXX = $(addprefix $(BUILD_DIR)/, $(FILENAMES_CXX:.cpp=.o))
vpath %.cxx $(dir $(SRC_CXX))

# Tool selection
CC = gcc
CXX = g++
LD = g++
NM = nm
OBJCOPY = objcopy
OBJDUMP = objdump
SIZE = size

all: $(BUILD_DIR) $(BUILD_DIR)/$(BIN)
	@echo ""
	$(CMD_ECHO) @$(SIZE) $(BUILD_DIR)/$(BIN)

$(BUILD_DIR):
	$(CMD_ECHO) mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c
	@echo "Compiling C file: $(notdir $<)"
	$(CMD_ECHO) $(CC) $(CFLAGS) $(DEF) $(INC) -c -o $@ $<

$(BUILD_DIR)/%.o: %.cpp
	@echo "Compiling C++ file: $(notdir $<)"
	$(CMD_ECHO) $(CXX) $(CXXFLAGS) $(DEF) $(INC) -c -o $@ $<

$(BUILD_DIR)/$(BIN): $(OBJS_C) $(OBJS_CXX)
	@echo "Linking binary: $(notdir $@)"
	$(CMD_ECHO) $(LD) $(LDFLAGS) -o $@ $^ $(LDLIBFLAGS)

run: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) -v -w 640 -h 480 -fps 30 -t 0 -o /dev/null \
	-x /dev/null -r /dev/null -rf gray

clean:
	rm -f $(BUILD_DIR)/$(BIN) $(BUILD_DIR)/*.map $(BUILD_DIR)/*.o
