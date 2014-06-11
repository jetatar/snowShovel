tamDH := $(patsubst tam/%,%,$(tamH))

tamLIBEXTRA :=         `root-config --glibs` 
##### bug in gcc 4.5.2 on linux ######
# see https://bugs.launchpad.net/ubuntu/+source/gcc-4.5/+bug/773712
ifeq ($(PLATFORM),linux)
ifeq ($(GCC_VERS_FULL),gcc-4.5.2)
ifneq ($(filter -O%,$(OPT)),)
   $(MODDIR)/TAMSelector.o: OPT = -O -g
endif
endif
endif
