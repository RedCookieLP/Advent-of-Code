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
DEBUG_FLAG				= DEBUG
WINDOWS_FLAG			= WINDOOF

#---^^^---^^^---#
#   Constants   #
#---------------#

#------------------------#
#   User Configuration   #
#---vvv------------vvv---#

COMPILER			= clang++
CFLAGS				= -Wall -Wextra -std=c++17 -static -O2 -m64
CFLAGS_DEBUG		= -D${DEBUG_FLAG} -Wall -Wextra -std=c++17 -static -O0 -g -m64
# Yes, I use GNU make on windows
ifeq (${OS},Windows_NT)
RM					= del
CFLAGS				+= -D${WINDOWS_FLAG}
CFLAGS_DEBUG		+= -D${WINDOWS_FLAG}
else
RM					= rm
endif

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
	$(MAKE) -f ${ABS_MAKEFILE_PATH} DAY_DIR="${PROJECT_DIR}${@:%_1=%}/" Part_1

Day_%_1_Debug: ${ABS_MAKEFILE_PATH}
	$(MAKE) -f ${ABS_MAKEFILE_PATH} DAY_DIR="${PROJECT_DIR}${@:%_1_Debug=%}/" Part_1_Debug

Day_%_2: ${ABS_MAKEFILE_PATH}
	$(MAKE) -f ${ABS_MAKEFILE_PATH} DAY_DIR="${PROJECT_DIR}${@:%_2=%}/" Part_2

Day_%_2_Debug: ${ABS_MAKEFILE_PATH}
	$(MAKE) -f ${ABS_MAKEFILE_PATH} DAY_DIR="${PROJECT_DIR}${@:%_2_Debug=%}/" Part_2_Debug

# Clean (for OS reasons, this is separated)

ifeq ("${OS}","Windows_NT")
define reformat_path
	$(subst /,\,$(1))
endef
else
define reformat_path
	$(1)
endef
endif

clean: $(wildcard ${PROJECT_DIR}*/*.exe) $(wildcard ${PROJECT_DIR}*/*.ilk) $(wildcard ${PROJECT_DIR}*/*.pdb)
	$(if $^,${RM} $(call reformat_path,$(foreach exe,$^,"${exe}"),))

# Actual working targets

Part_1: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_1.exe since no target directory has been specified!"))
	$(COMPILER) -D${PART_1_FLAG} $(CFLAGS) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" "$<" -o "${DAY_DIR}$@.exe"

Part_1_Debug: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_1_Debug.exe since no target directory has been specified!"))
	$(COMPILER) -D${PART_1_FLAG} $(CFLAGS_DEBUG) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" "$<" -o "${DAY_DIR}$@.exe"

Part_2: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_2.exe since no target directory has been specified!"))
	$(COMPILER) $(CFLAGS) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" $< -o "${DAY_DIR}$@.exe"

Part_2_Debug: ${DAY_DIR}main.cpp ${ABS_MAKEFILE_PATH}
	$(if ${DAY_DIR},,$(error "Cannot build Part_2_Debug.exe since no target directory has been specified!"))
	$(COMPILER) $(CFLAGS_DEBUG) -I"$(dir ${AOC_MODULE_H})" "${ENTRY_POINT_CPP}" $< -o "${DAY_DIR}$@.exe"

# Default make-flags
MAKEFLAGS += --jobs 2 --silent --output-sync line
