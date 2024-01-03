PRJ_NAME = ProjectSample
CC = g++

PRJ_SOURCE_CODE := ./src
PRJ_OUTPUT_DIR  := ./build
PRJ_INCS_DIR  := /include/
CURRENT_DIR     := $(shell pwd)
INCLUDES        += -I${CURRENT_DIR}${PRJ_INCS_DIR}

WORKAROUNDFORGIO = -I/usr/include/glib-2.0/gio

 ### Compilation Flags ###
DEBUG_LEVEL         := 3
OPTIMIZATION_LEVEL  := 0
CFLAGS              := -O$(OPTIMIZATION_LEVEL) -g$(DEBUG_LEVEL) -Wall -std=c++17
LDFLAGS	            := $(shell pkg-config --libs --cflags glib-2.0)

### Derived Variables ###
CPP_SRC_FILES   := $(shell find -L $(PRJ_SOURCE_CODE) -type f -name "*.cpp"  -print)                # All C++ files
CPP_OBJECTS     := $(patsubst $(PRJ_SOURCE_CODE)/%.cpp,$(PRJ_OUTPUT_DIR)/%.o, $(CPP_SRC_FILES))    # All C++ objects

### Targets ###
all : $(CPP_OBJECTS)
	@echo "*******************************************************************************************************"
	@echo "Executing target '$@' for project '${PRJ_NAME}'"
	$(CC) -o $(PRJ_NAME) $(CPP_OBJECTS) $(LDFLAGS)
	@echo "Finished executing the target '$@' for project '${PRJ_NAME}'"


# Generic C/C++ files build target (Object generation)
${PRJ_OUTPUT_DIR}/%.o : $(PRJ_SOURCE_CODE)/%.cpp
	@echo "*******************************************************************************************************"
	@echo "Building '$<' for project '${PRJ_NAME}'"
	$(CC) $(CFLAGS) $(INCLUDES) $(WORKAROUNDFORGIO) $(LDFLAGS) -c $< -o $@
	@echo "Built '$<' for project '${PRJ_NAME}'"

clean :
	rm ${PRJ_OUTPUT_DIR}/*.o
	rm $(PRJ_NAME)

