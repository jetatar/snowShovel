# $Id$
#
# Module.mk -- Makefile for online
# (don't manually edit this file)
#

MODDIR := online

onlineS  := $(filter-out online/G__%,$(wildcard online/*.cxx))
onlineF  := $(wildcard online/*.f)

onlineH  := $(filter-out online/%LinkDef.h,$(wildcard online/*.h*))
onlineH  := $(filter-out online/G__%.h,$(onlineH))
onlineI  := $(wildcard online/*.inc)

onlineLIB     := lib/libonline.$(SOEXT)

onlinePAR     := par/online.par
onlinePARINIT := $(shell if test -e "online/online_setup.C"; then echo "online/online_setup.C"; else echo ""; fi)

onlineMAP     := online/online.rootmap

onlineOMOD        = $(foreach mm,$(patsubst %,%O,$(onlineMODULES)),$($(mm)))
onlineHMOD        = $(foreach mm,$(patsubst %,%H,$(onlineMODULES)),$($(mm)))
onlineIMOD        = $(foreach mm,$(patsubst %,%I,$(onlineMODULES)),$($(mm)))
onlineLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(onlineMODULES)),$($(mm)))
onlinePAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(onlineMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include online/online.mk



# derive source dependent output
onlineO   := $(onlineS:.cxx=.o) $(if $(wildcard online/onlineLinkDef.h),online/G__online.o) $(onlineF:.f=.o)
onlineDEP := $(onlineO:.o=.d)

ifeq ($(onlineMAPLIB),)
onlineMAPLIB := $(onlineLIB)
else
onlineMAPLIB := lib/$(onlineMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst online/%.h,include/%.h,$(onlineH))
ALLHDRS     += $(patsubst online/%.hxx,include/%.hxx,$(onlineH))
ALLHDRS     += $(patsubst online/%.inc,include/%.inc,$(onlineI))
ifeq ($(onlineNOLIB),)
ALLLIBS	    += $(onlineLIB)
ALLPARS     += $(onlinePAR)
ALLMAPS     += $(onlineMAP)
endif

# include all dependency files
INCLUDEFILES += $(onlineDEP)


#
# Populate central include dir
#
include/%.h:    online/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  online/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  online/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs online-libs pars online-pars clean online-clean


ifeq ($(onlineNOLIB),)
libs:: online-libs

online-libs: $(onlineLIB)

pars:: online-pars

online-pars: $(onlinePAR)
endif

extras:: $(onlineEXTRAS)

clean:: online-clean

online-clean: $(onlineLOCALCLEAN)
	rm -f $(onlineLIB) $(onlinePAR) $(onlineO) $(onlineDEP) online/G__online.h online/G__online.cxx online/online.rootmap

ifeq ($(onlineNOLIB),)
online-all: online-libs online-pars $(onlineEXTRAS)
else
online-all: $(onlineEXTRAS)
endif

online:: online-all

ifneq ($(onlineF),)
onlineLIBEXTRA += $(F77LIBS)
endif

ifeq ($(onlineNOLIB),)
$(onlineLIB): $(onlineLIBDEP) $(onlineO) $(onlineOMOD) online/Module.mk online/online.mk
	@echo "Making libonline.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libonline.$(SOEXT) $@ "$(onlineO) $(onlineOMOD)" \
	"$(onlineLIBEXTRA) $(onlineLIBEXTRAMOD)"


$(onlinePAR): $(onlineLIB) $(onlineH) $(onlineI) $(onlinePAREXTRA) $(onlinePAREXTRAMOD) $(onlinePARINIT)
	@if test -e "$(onlinePARINIT)"; then \
	        echo "Making $(onlinePAR) using $(onlinePARINIT)"; \
         else echo "Making $(onlinePAR)" ; fi
	@$(MAKEPAR) $(onlinePAR) $(onlineLIB) "$(onlineH) $(onlineI) $(onlineHMOD) $(onlineIMOD)" \
	"$(onlinePAREXTRA) $(onlinePAREXTRAMOD)" online/online_setup.C
else
$(onlineLIB): $(onlineLIBDEP) $(onlineO) $(onlineOMOD) online/Module.mk online/online.mk
	@echo "Not making libonline.$(SOEXT)"

$(onlinePAR): $(onlineLIB) $(onlineH) $(onlineI) $(onlinePAREXTRA) $(onlinePAREXTRAMOD) $(onlinePARINIT)
	@echo "Not making $(onlinePAR)"
endif

#
# Dictionary rule
#

onlineDH := $(patsubst %,online/%,$(onlineDH))

online/G__online.h online/G__online.cxx: config.mk $(ROOTCINT) online/onlineLinkDef.h $(onlineDH)
	        $(ROOTCINT) -f online/G__online.cxx -c $(onlineCF) $(CPPFLAGS) $(onlineDH) $(onlineDHEXTRA) \
		online/onlineLinkDef.h


#
# Dependency generation rules
#

online/G__%.d: online/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(onlineCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

online/%.d: online/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(onlineCXXFLAGSEXTRA)" $< > $@

online/%.d: online/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(onlineCXXFLAGSEXTRA)" $< > $@

online/%.d: online/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(onlineCXXFLAGSEXTRA),)
# Special rule for dictionaries
online/G__%.o: online/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(onlineCXXFLAGSEXTRA) -o $@ -c $<

online/%.o: online/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(onlineCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(onlineCFLAGSEXTRA),)
online/%.o: online/%.c
	$(CC) $(OPT) $(CFLAGS) $(onlineCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(onlineF77FLAGSEXTRA),)
online/%.o: online/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(onlineF77FLAGSEXTRA) -o $@ -c $<
endif

online-map: $(onlineMAP)

ifeq ($(onlineNOLIB),)
$(onlineMAP): online/online.mk online/onlineLinkDef.h
	$(MAKEMAP) -o online/online.rootmap -l $(onlineMAPLIB) -c online/onlineLinkDef.h
else
$(onlineMAP): online/online.mk online/onlineLinkDef.h
	@echo "Not making $(onlineMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
