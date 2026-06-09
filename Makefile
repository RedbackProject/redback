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
ifeq ($(NavierStokes),1)
  $(info >>> Compiling with NavierStokes module)
  NAVIER_STOKES := yes
  libmesh_CXXFLAGS += -DNAVIER_STOKES_ENABLED
endif
ifeq ($(PorousFlow),1)
  $(info >>> Compiling with PorousFlow module)
  POROUS_FLOW := yes
  libmesh_CXXFLAGS += -DPOROUS_FLOW_ENABLED
endif
include $(MOOSE_DIR)/modules/modules.mk
###############################################################################

# dep apps
APPLICATION_DIR    := $(CURDIR)
APPLICATION_NAME   := redback
BUILD_EXEC         := yes
DEP_APPS           := $(shell $(FRAMEWORK_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))
include            $(FRAMEWORK_DIR)/app.mk

###############################################################################
# Additional special case targets should be added here

LIBIGL_DIR := $(HOME)/projects/libigl/include
LIBIGL_CONTENT := $(shell ls $(LIBIGL_DIR) 2>/dev/null)

ifneq ($(LIBIGL_CONTENT),)
  $(info >>> LIBIGL detected, enabling LIBIGL flags)
  app_INCLUDES += -I$(LIBIGL_DIR)
  libmesh_CXXFLAGS += -DLIBIGL_ENABLED
endif