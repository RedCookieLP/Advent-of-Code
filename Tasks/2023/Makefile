#----------------------------#
#   Makefile for AoC-2023    #
#----------------------------#
# Tab-Width: 4

#---------------#
#   Constants   #
#---vvv---vvv---#

ABS_MAKEFILE_PATH		= $(abspath $(lastword ${MAKEFILE_LIST}))
PROJECT_DIR				= $(dir ${ABS_MAKEFILE_PATH})

PART_1_FLAG				= DO_PART_1

#---^^^---^^^---#
#   Constants   #
#---------------#

#------------------------#
#   User Configuration   #
#---vvv------------vvv---#

COMPILER			= clang++
CFLAGS				= -Wall -Wextra -std=c++17 -static -O2 -m64
CFLAGS_DEBUG		= -Wall -Wextra -std=c++17 -static -O0 -g -m64

#---^^^------------^^^---#
#   User Configuration   #
#------------------------#

#-------------------#
#   Project-Files   #
#---vvv-------vvv---#

MAIN_CPP			= main.cpp
ENTRY_POINT_CPP		= ${PROJECT_DIR}entry-point.cpp
AOC_MODULE_H		= ${PROJECT_DIR}AoC-Module.h

#---^^^-------^^^---#
#   Project-Files   #
#-------------------#

#-------------------#
#   Build Targets   #
#---vvv-------vvv---#

# Pseudo-Targets for your/my convenience
# Please only use these, unless you know what you're doing...

Day_%_1: ${ABS_MAKEFILE_PATH}
	make -f "${ABS_MAKEFILE_PATH}" DAY_DIR="${PROJECT_DIR}${@:%_1=%}/" Part_1.exe

Day_%_1_Debug: ${ABS_MAKEFILE_PATH}
	make -f "${ABS_MAKEFILE_PATH}" DAY_DIR="${PROJECT_DIR}${@:%_1_Debug=%}/" Part_1_Debug.exe

Day_%_2: ${ABS_MAKEFILE_PATH}
	make -f "${ABS_MAKEFILE_PATH}" DAY_DIR="${PROJECT_DIR}${@:%_2=%}/" Part_2.exe

Day_%_2_Debug: ${ABS_MAKEFILE_PATH}
	make -f "${ABS_MAKEFILE_PATH}" DAY_DIR="${PROJECT_DIR}${@:%_2_Debug=%}/" Part_2_Debug.exe

# Actual working targets

Part_1.exe: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_1.exe since no target directory has been specified!"))
	$(COMPILER) -D${PART_1_FLAG} $(CFLAGS) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" "$<" -o "${DAY_DIR}Part_1.exe"

Part_1_Debug.exe: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_1_Debug.exe since no target directory has been specified!"))
	$(COMPILER) -D${PART_1_FLAG} $(CFLAGS_DEBUG) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" "$<" -o "${DAY_DIR}Part_1_Debug.exe"

Part_2.exe: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_2.exe since no target directory has been specified!"))
	$(COMPILER) $(CFLAGS) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" $< -o "${DAY_DIR}Part_2.exe"

Part_2_Debug.exe: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_2_Debug.exe since no target directory has been specified!"))
	$(COMPILER) $(CFLAGS_DEBUG) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" $< -o "${DAY_DIR}Part_2_Debug.exe"

#	
#	build: ${PROGRAM}
#	
#	build-debug: ${PROGRAM_DEBUG}
#	
#	build-all: build build-debug
#	
#	clean:
#		rm -rf ${PROGRAM} ${PROGRAM_DEBUG} ${LOGPARSER_OBJS} ${LOGPARSER_OBJS_DEBUG}
#	
#	run: build
#		echo -e '\x1B[32m--- Running $(notdir ${PROGRAM})... ---\x1B[0m'
#		${PROGRAM} ${RUN_ARGS}
#	
#	run-debug: build-debug
#		echo -e '\x1B[32m--- Running $(notdir ${PROGRAM_DEBUG})... ---\x1B[0m'
#		${PROGRAM_DEBUG} ${RUN_ARGS}
#	
#	.SECONDEXPANSION:
#	PERCENT = %
#	
#	${PROGRAM}: ${LOGPARSER_OBJS} ${LOGPARSER_HEADERS} ${ABS_MAKEFILE_PATH}
#		echo -e '\x1B[32m--- Linking OBJs to $(notdir $@)... ---\x1B[0m'
#		${LINKER} ${LINKFLAGS} ${LOGPARSER_OBJS} -o "$@"
#	
#	${PROGRAM_DEBUG}: ${LOGPARSER_OBJS_DEBUG} ${LOGPARSER_HEADERS} ${ABS_MAKEFILE_PATH}
#		echo -e '\x1B[32m--- Linking OBJs to $(notdir $@)... ---\x1B[0m'
#		${LINKER} ${LINKFLAGS} ${LOGPARSER_OBJS_DEBUG} -o "$@"
#	
#	${PROJECT_INT_DIR_DEBUG}%.o: $$(filter $${PERCENT}$$(notdir $$(patsubst $${PERCENT}.o,$${PERCENT}.cpp,$$@)), $${LOGPARSER_SOURCES}) ${LOGPARSER_HEADERS} ${ABS_MAKEFILE_PATH}
#		echo -e '\x1B[32m--- Compiling $(notdir $<) (Debug) to $(notdir $@)... ---\x1B[0m'
#		mkdir -p $(dir $@)
#		${COMPILER} ${CFLAGS_DEBUG} -c "$<" -o "$@"
#	
#	${PROJECT_INT_DIR}%.o: $$(filter $${PERCENT}$$(notdir $$(patsubst $${PERCENT}.o,$${PERCENT}.cpp,$$@)), $${LOGPARSER_SOURCES}) ${LOGPARSER_HEADERS} ${ABS_MAKEFILE_PATH}
#		echo -e '\x1B[32m--- Compiling $(notdir $<) to $(notdir $@)... ---\x1B[0m'
#		mkdir -p $(dir $@)
#		${COMPILER} ${CFLAGS} -c "$<" -o "$@"
#	
#	#---^^^-------^^^---#
#	#   Build Targets   #
#	#-------------------#
#	
MAKEFLAGS	+= --jobs 2 --silent --output-sync line
#	.PHONY: \
#		build \
#		build-debug \
#		build-all \
#		clean \
#		run \
#		run-debug
#	