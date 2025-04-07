######################## LOCAL VARIABLES ######################################

# Use the following files to set local makefile variables

# Make.local.username
-include Make.local.$(shell whoami)

# Make.local.hostname
-include Make.local.$(shell hostname|sed s/[0-9,\-]//g )

###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Optional Environment variables
# MOOSE_DIR        - Root directory of the MOOSE project
#
###############################################################################

# Use the MOOSE submodule if it exists and MOOSE_DIR is not set
MOOSE_SUBMODULE    := $(CURDIR)/moose
ifneq ($(wildcard $(MOOSE_SUBMODULE)/framework/Makefile),)
  MOOSE_DIR        ?= $(MOOSE_SUBMODULE)
else
  MOOSE_DIR        ?= $(shell dirname `pwd`)/moose
endif

# framework
FRAMEWORK_DIR      := $(MOOSE_DIR)/framework
include $(FRAMEWORK_DIR)/build.mk
include $(FRAMEWORK_DIR)/moose.mk

################################## MODULES ####################################
ALL_MODULES := no
SOLID_MECHANICS  := yes
include $(MOOSE_DIR)/modules/modules.mk
###############################################################################

# dep apps
APPLICATION_DIR    := $(CURDIR)
APPLICATION_NAME   := redback
BUILD_EXEC         := yes
DEP_APPS           := $(shell $(FRAMEWORK_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))

include $(FRAMEWORK_DIR)/app.mk
###############################################################################
# Additional special case targets should be added here
# ADDITIONAL_LIBS       := /Users/pou036/projects/dynamicLib/

COSSERAT_DYNLIB_DIR ?= $(HOME)/projects/cosserat_dynlib
COSSERAT_DYNLIB_DIR_EXISTS = $(shell [ -e $(COSSERAT_DYNLIB_DIR) ] && echo 1 || echo 0 )

ifeq ($(COSSERAT_DYNLIB_DIR_EXISTS), 1)
  EXTERNAL_FLAGS += -DCOSSERAT_DYNLIB_EXISTS
endif
