PRJ_NAME = ProjectSample
CC = g++

PRJ_SOURCE_CODE := ./src
PRJ_OUTPUT_DIR  := ./misc
PRJ_INCS_DIR  := ./include/
INCLUDES        += -I${PRJ_INCS_DIR}

### Derived Variables ###
CPP_SRC_FILES   := $(shell find -L $(PRJ_SOURCE_CODE) -type f -name "*.cpp"  -print)                # All C++ files
CPP_OBJECTS     := $(patsubst $(PRJ_SOURCE_CODE)/%.cpp,$(PRJ_OUTPUT_DIR)/%.o, $(CPP_SRC_FILES))    # All C++ objects

### Targets ###
all : $(CPP_OBJECTS)
	@echo "*******************************************************************************************************\r\n"
	@echo "Executing target '$@' for project '${PRJ_NAME}'"
	$(CC) -o $(PRJ_NAME) $(CPP_OBJECTS)
	@echo "Finished executing the target '$@' for project '${PRJ_NAME}'"


# Generic C/C++ files build target (Object generation)
${PRJ_OUTPUT_DIR}/%.o : $(PRJ_SOURCE_CODE)/%.cpp
	@echo "*******************************************************************************************************\r\n"
	@echo "Building '$<' for project '${PRJ_NAME}'"
	$(CC) $(INCLUDES) -c $< -o $@
	@echo "Built '$<' for project '${PRJ_NAME}'"

clean :
	rm ${PRJ_OUTPUT_DIR}/*.o
	rm $(PRJ_NAME)

