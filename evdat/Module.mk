# $Id$
#
# Module.mk -- Makefile for evdat
# (don't manually edit this file)
#

MODDIR := evdat

evdatS  := $(filter-out evdat/G__%,$(wildcard evdat/*.cxx))
evdatF  := $(wildcard evdat/*.f)

evdatH  := $(filter-out evdat/%LinkDef.h,$(wildcard evdat/*.h*))
evdatH  := $(filter-out evdat/G__%.h,$(evdatH))
evdatI  := $(wildcard evdat/*.inc)

evdatLIB     := lib/libevdat.$(SOEXT)

evdatPAR     := par/evdat.par
evdatPARINIT := $(shell if test -e "evdat/evdat_setup.C"; then echo "evdat/evdat_setup.C"; else echo ""; fi)

evdatMAP     := evdat/evdat.rootmap

evdatOMOD        = $(foreach mm,$(patsubst %,%O,$(evdatMODULES)),$($(mm)))
evdatHMOD        = $(foreach mm,$(patsubst %,%H,$(evdatMODULES)),$($(mm)))
evdatIMOD        = $(foreach mm,$(patsubst %,%I,$(evdatMODULES)),$($(mm)))
evdatLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(evdatMODULES)),$($(mm)))
evdatPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(evdatMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include evdat/evdat.mk



# derive source dependent output
evdatO   := $(evdatS:.cxx=.o) $(if $(wildcard evdat/evdatLinkDef.h),evdat/G__evdat.o) $(evdatF:.f=.o)
evdatDEP := $(evdatO:.o=.d)

ifeq ($(evdatMAPLIB),)
evdatMAPLIB := $(evdatLIB)
else
evdatMAPLIB := lib/$(evdatMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst evdat/%.h,include/%.h,$(evdatH))
ALLHDRS     += $(patsubst evdat/%.hxx,include/%.hxx,$(evdatH))
ALLHDRS     += $(patsubst evdat/%.inc,include/%.inc,$(evdatI))
ifeq ($(evdatNOLIB),)
ALLLIBS	    += $(evdatLIB)
ALLPARS     += $(evdatPAR)
ALLMAPS     += $(evdatMAP)
endif

# include all dependency files
INCLUDEFILES += $(evdatDEP)


#
# Populate central include dir
#
include/%.h:    evdat/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  evdat/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  evdat/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs evdat-libs pars evdat-pars clean evdat-clean


ifeq ($(evdatNOLIB),)
libs:: evdat-libs

evdat-libs: $(evdatLIB)

pars:: evdat-pars

evdat-pars: $(evdatPAR)
endif

extras:: $(evdatEXTRAS)

clean:: evdat-clean

evdat-clean: $(evdatLOCALCLEAN)
	rm -f $(evdatLIB) $(evdatPAR) $(evdatO) $(evdatDEP) evdat/G__evdat.h evdat/G__evdat.cxx evdat/evdat.rootmap

ifeq ($(evdatNOLIB),)
evdat-all: evdat-libs evdat-pars $(evdatEXTRAS)
else
evdat-all: $(evdatEXTRAS)
endif

evdat:: evdat-all

ifneq ($(evdatF),)
evdatLIBEXTRA += $(F77LIBS)
endif

ifeq ($(evdatNOLIB),)
$(evdatLIB): $(evdatLIBDEP) $(evdatO) $(evdatOMOD) evdat/Module.mk evdat/evdat.mk
	@echo "Making libevdat.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libevdat.$(SOEXT) $@ "$(evdatO) $(evdatOMOD)" \
	"$(evdatLIBEXTRA) $(evdatLIBEXTRAMOD)"


$(evdatPAR): $(evdatLIB) $(evdatH) $(evdatI) $(evdatPAREXTRA) $(evdatPAREXTRAMOD) $(evdatPARINIT)
	@if test -e "$(evdatPARINIT)"; then \
	        echo "Making $(evdatPAR) using $(evdatPARINIT)"; \
         else echo "Making $(evdatPAR)" ; fi
	@$(MAKEPAR) $(evdatPAR) $(evdatLIB) "$(evdatH) $(evdatI) $(evdatHMOD) $(evdatIMOD)" \
	"$(evdatPAREXTRA) $(evdatPAREXTRAMOD)" evdat/evdat_setup.C
else
$(evdatLIB): $(evdatLIBDEP) $(evdatO) $(evdatOMOD) evdat/Module.mk evdat/evdat.mk
	@echo "Not making libevdat.$(SOEXT)"

$(evdatPAR): $(evdatLIB) $(evdatH) $(evdatI) $(evdatPAREXTRA) $(evdatPAREXTRAMOD) $(evdatPARINIT)
	@echo "Not making $(evdatPAR)"
endif

#
# Dictionary rule
#

evdatDH := $(patsubst %,evdat/%,$(evdatDH))

evdat/G__evdat.h evdat/G__evdat.cxx: config.mk $(ROOTCINT) evdat/evdatLinkDef.h $(evdatDH)
	        $(ROOTCINT) -f evdat/G__evdat.cxx -c $(evdatCF) $(CPPFLAGS) $(evdatDH) $(evdatDHEXTRA) \
		evdat/evdatLinkDef.h


#
# Dependency generation rules
#

evdat/G__%.d: evdat/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(evdatCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

evdat/%.d: evdat/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(evdatCXXFLAGSEXTRA)" $< > $@

evdat/%.d: evdat/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(evdatCXXFLAGSEXTRA)" $< > $@

evdat/%.d: evdat/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(evdatCXXFLAGSEXTRA),)
# Special rule for dictionaries
evdat/G__%.o: evdat/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(evdatCXXFLAGSEXTRA) -o $@ -c $<

evdat/%.o: evdat/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(evdatCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(evdatCFLAGSEXTRA),)
evdat/%.o: evdat/%.c
	$(CC) $(OPT) $(CFLAGS) $(evdatCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(evdatF77FLAGSEXTRA),)
evdat/%.o: evdat/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(evdatF77FLAGSEXTRA) -o $@ -c $<
endif

evdat-map: $(evdatMAP)

ifeq ($(evdatNOLIB),)
$(evdatMAP): evdat/evdat.mk evdat/evdatLinkDef.h
	$(MAKEMAP) -o evdat/evdat.rootmap -l $(evdatMAPLIB) -c evdat/evdatLinkDef.h
else
$(evdatMAP): evdat/evdat.mk evdat/evdatLinkDef.h
	@echo "Not making $(evdatMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
