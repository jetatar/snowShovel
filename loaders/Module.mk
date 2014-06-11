# $Id$
#
# Module.mk -- Makefile for loaders
# (don't manually edit this file)
#

MODDIR := loaders

loadersS  := $(filter-out loaders/G__%,$(wildcard loaders/*.cxx))
loadersF  := $(wildcard loaders/*.f)

loadersH  := $(filter-out loaders/%LinkDef.h,$(wildcard loaders/*.h*))
loadersH  := $(filter-out loaders/G__%.h,$(loadersH))
loadersI  := $(wildcard loaders/*.inc)

loadersLIB     := lib/libloaders.$(SOEXT)

loadersPAR     := par/loaders.par
loadersPARINIT := $(shell if test -e "loaders/loaders_setup.C"; then echo "loaders/loaders_setup.C"; else echo ""; fi)

loadersMAP     := loaders/loaders.rootmap

loadersOMOD        = $(foreach mm,$(patsubst %,%O,$(loadersMODULES)),$($(mm)))
loadersHMOD        = $(foreach mm,$(patsubst %,%H,$(loadersMODULES)),$($(mm)))
loadersIMOD        = $(foreach mm,$(patsubst %,%I,$(loadersMODULES)),$($(mm)))
loadersLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(loadersMODULES)),$($(mm)))
loadersPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(loadersMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include loaders/loaders.mk



# derive source dependent output
loadersO   := $(loadersS:.cxx=.o) $(if $(wildcard loaders/loadersLinkDef.h),loaders/G__loaders.o) $(loadersF:.f=.o)
loadersDEP := $(loadersO:.o=.d)

ifeq ($(loadersMAPLIB),)
loadersMAPLIB := $(loadersLIB)
else
loadersMAPLIB := lib/$(loadersMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst loaders/%.h,include/%.h,$(loadersH))
ALLHDRS     += $(patsubst loaders/%.hxx,include/%.hxx,$(loadersH))
ALLHDRS     += $(patsubst loaders/%.inc,include/%.inc,$(loadersI))
ifeq ($(loadersNOLIB),)
ALLLIBS	    += $(loadersLIB)
ALLPARS     += $(loadersPAR)
ALLMAPS     += $(loadersMAP)
endif

# include all dependency files
INCLUDEFILES += $(loadersDEP)


#
# Populate central include dir
#
include/%.h:    loaders/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  loaders/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  loaders/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs loaders-libs pars loaders-pars clean loaders-clean


ifeq ($(loadersNOLIB),)
libs:: loaders-libs

loaders-libs: $(loadersLIB)

pars:: loaders-pars

loaders-pars: $(loadersPAR)
endif

extras:: $(loadersEXTRAS)

clean:: loaders-clean

loaders-clean: $(loadersLOCALCLEAN)
	rm -f $(loadersLIB) $(loadersPAR) $(loadersO) $(loadersDEP) loaders/G__loaders.h loaders/G__loaders.cxx loaders/loaders.rootmap

ifeq ($(loadersNOLIB),)
loaders-all: loaders-libs loaders-pars $(loadersEXTRAS)
else
loaders-all: $(loadersEXTRAS)
endif

loaders:: loaders-all

ifneq ($(loadersF),)
loadersLIBEXTRA += $(F77LIBS)
endif

ifeq ($(loadersNOLIB),)
$(loadersLIB): $(loadersLIBDEP) $(loadersO) $(loadersOMOD) loaders/Module.mk loaders/loaders.mk
	@echo "Making libloaders.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libloaders.$(SOEXT) $@ "$(loadersO) $(loadersOMOD)" \
	"$(loadersLIBEXTRA) $(loadersLIBEXTRAMOD)"


$(loadersPAR): $(loadersLIB) $(loadersH) $(loadersI) $(loadersPAREXTRA) $(loadersPAREXTRAMOD) $(loadersPARINIT)
	@if test -e "$(loadersPARINIT)"; then \
	        echo "Making $(loadersPAR) using $(loadersPARINIT)"; \
         else echo "Making $(loadersPAR)" ; fi
	@$(MAKEPAR) $(loadersPAR) $(loadersLIB) "$(loadersH) $(loadersI) $(loadersHMOD) $(loadersIMOD)" \
	"$(loadersPAREXTRA) $(loadersPAREXTRAMOD)" loaders/loaders_setup.C
else
$(loadersLIB): $(loadersLIBDEP) $(loadersO) $(loadersOMOD) loaders/Module.mk loaders/loaders.mk
	@echo "Not making libloaders.$(SOEXT)"

$(loadersPAR): $(loadersLIB) $(loadersH) $(loadersI) $(loadersPAREXTRA) $(loadersPAREXTRAMOD) $(loadersPARINIT)
	@echo "Not making $(loadersPAR)"
endif

#
# Dictionary rule
#

loadersDH := $(patsubst %,loaders/%,$(loadersDH))

loaders/G__loaders.h loaders/G__loaders.cxx: config.mk $(ROOTCINT) loaders/loadersLinkDef.h $(loadersDH)
	        $(ROOTCINT) -f loaders/G__loaders.cxx -c $(loadersCF) $(CPPFLAGS) $(loadersDH) $(loadersDHEXTRA) \
		loaders/loadersLinkDef.h


#
# Dependency generation rules
#

loaders/G__%.d: loaders/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(loadersCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

loaders/%.d: loaders/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(loadersCXXFLAGSEXTRA)" $< > $@

loaders/%.d: loaders/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(loadersCXXFLAGSEXTRA)" $< > $@

loaders/%.d: loaders/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(loadersCXXFLAGSEXTRA),)
# Special rule for dictionaries
loaders/G__%.o: loaders/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(loadersCXXFLAGSEXTRA) -o $@ -c $<

loaders/%.o: loaders/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(loadersCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(loadersCFLAGSEXTRA),)
loaders/%.o: loaders/%.c
	$(CC) $(OPT) $(CFLAGS) $(loadersCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(loadersF77FLAGSEXTRA),)
loaders/%.o: loaders/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(loadersF77FLAGSEXTRA) -o $@ -c $<
endif

loaders-map: $(loadersMAP)

ifeq ($(loadersNOLIB),)
$(loadersMAP): loaders/loaders.mk loaders/loadersLinkDef.h
	$(MAKEMAP) -o loaders/loaders.rootmap -l $(loadersMAPLIB) -c loaders/loadersLinkDef.h
else
$(loadersMAP): loaders/loaders.mk loaders/loadersLinkDef.h
	@echo "Not making $(loadersMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
