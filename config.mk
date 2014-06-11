#
# config.mk
#
# Created at: Sun Nov 24 13:06:14 PST 2013  
#         by: $Id$
#
# $Id$
#
# BuildSystemVersion: 1
#

TOPVAR=SNS

# Package variables
SNS_CONF=/data/users/jtatar/Work/snowShovel
ROOTSYS_CONF=/data/users/jtatar/Software/root
CERNLIB_CONF=
PQXX_CONF=

INCCOPYCMD=ln -sf
CCBASE=
CXXBASE=
F77BASE=
GCCVERSION=

#
# Main package
#
LIBDIR=/data/users/jtatar/Work/snowShovel/lib

# Compiler and Linker configuration
include build/config/linux.x86_64.mk


#
# ROOT configuration
#
ROOTLIB:=/data/users/jtatar/Software/root/lib
ROOTINC:=/data/users/jtatar/Software/root/include


#
# Validate external package configuration
#
ifneq ($(SNS_CONF),)
ifneq ($(SNS),$(SNS_CONF))
dummy:=$(error $$SNS has changed, please run './configure' or set correctly)
endif
endif

ifneq ($(ROOTSYS_CONF),)
ifneq ($(ROOTSYS),$(ROOTSYS_CONF))
dummy:=$(error $$ROOTSYS has changed, please run './configure' or set correctly)
endif
endif

ifneq ($(CERNLIB_CONF),)
ifneq ($(CERNLIB),$(CERNLIB_CONF))
dummy:=$(error $$CERNLIB has changed, please run './configure' or set correctly)
endif
endif

ifneq ($(PQXX_CONF),)
ifneq ($(PQXXLIB),$(PQXX_CONF))
dummy:=$(error $$PQXXLIB has changed, please run './configure' or set correctly)
endif
endif

