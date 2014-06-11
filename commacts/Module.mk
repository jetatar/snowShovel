# $Id$
#
# Module.mk -- Makefile for commacts
# (don't manually edit this file)
#

MODDIR := commacts

commactsS  := $(filter-out commacts/G__%,$(wildcard commacts/*.cxx))
commactsF  := $(wildcard commacts/*.f)

commactsH  := $(filter-out commacts/%LinkDef.h,$(wildcard commacts/*.h*))
commactsH  := $(filter-out commacts/G__%.h,$(commactsH))
commactsI  := $(wildcard commacts/*.inc)

commactsLIB     := lib/libcommacts.$(SOEXT)

commactsPAR     := par/commacts.par
commactsPARINIT := $(shell if test -e "commacts/commacts_setup.C"; then echo "commacts/commacts_setup.C"; else echo ""; fi)

commactsMAP     := commacts/commacts.rootmap

commactsOMOD        = $(foreach mm,$(patsubst %,%O,$(commactsMODULES)),$($(mm)))
commactsHMOD        = $(foreach mm,$(patsubst %,%H,$(commactsMODULES)),$($(mm)))
commactsIMOD        = $(foreach mm,$(patsubst %,%I,$(commactsMODULES)),$($(mm)))
commactsLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(commactsMODULES)),$($(mm)))
commactsPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(commactsMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include commacts/commacts.mk



# derive source dependent output
commactsO   := $(commactsS:.cxx=.o) $(if $(wildcard commacts/commactsLinkDef.h),commacts/G__commacts.o) $(commactsF:.f=.o)
commactsDEP := $(commactsO:.o=.d)

ifeq ($(commactsMAPLIB),)
commactsMAPLIB := $(commactsLIB)
else
commactsMAPLIB := lib/$(commactsMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst commacts/%.h,include/%.h,$(commactsH))
ALLHDRS     += $(patsubst commacts/%.hxx,include/%.hxx,$(commactsH))
ALLHDRS     += $(patsubst commacts/%.inc,include/%.inc,$(commactsI))
ifeq ($(commactsNOLIB),)
ALLLIBS	    += $(commactsLIB)
ALLPARS     += $(commactsPAR)
ALLMAPS     += $(commactsMAP)
endif

# include all dependency files
INCLUDEFILES += $(commactsDEP)


#
# Populate central include dir
#
include/%.h:    commacts/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  commacts/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  commacts/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs commacts-libs pars commacts-pars clean commacts-clean


ifeq ($(commactsNOLIB),)
libs:: commacts-libs

commacts-libs: $(commactsLIB)

pars:: commacts-pars

commacts-pars: $(commactsPAR)
endif

extras:: $(commactsEXTRAS)

clean:: commacts-clean

commacts-clean: $(commactsLOCALCLEAN)
	rm -f $(commactsLIB) $(commactsPAR) $(commactsO) $(commactsDEP) commacts/G__commacts.h commacts/G__commacts.cxx commacts/commacts.rootmap

ifeq ($(commactsNOLIB),)
commacts-all: commacts-libs commacts-pars $(commactsEXTRAS)
else
commacts-all: $(commactsEXTRAS)
endif

commacts:: commacts-all

ifneq ($(commactsF),)
commactsLIBEXTRA += $(F77LIBS)
endif

ifeq ($(commactsNOLIB),)
$(commactsLIB): $(commactsLIBDEP) $(commactsO) $(commactsOMOD) commacts/Module.mk commacts/commacts.mk
	@echo "Making libcommacts.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libcommacts.$(SOEXT) $@ "$(commactsO) $(commactsOMOD)" \
	"$(commactsLIBEXTRA) $(commactsLIBEXTRAMOD)"


$(commactsPAR): $(commactsLIB) $(commactsH) $(commactsI) $(commactsPAREXTRA) $(commactsPAREXTRAMOD) $(commactsPARINIT)
	@if test -e "$(commactsPARINIT)"; then \
	        echo "Making $(commactsPAR) using $(commactsPARINIT)"; \
         else echo "Making $(commactsPAR)" ; fi
	@$(MAKEPAR) $(commactsPAR) $(commactsLIB) "$(commactsH) $(commactsI) $(commactsHMOD) $(commactsIMOD)" \
	"$(commactsPAREXTRA) $(commactsPAREXTRAMOD)" commacts/commacts_setup.C
else
$(commactsLIB): $(commactsLIBDEP) $(commactsO) $(commactsOMOD) commacts/Module.mk commacts/commacts.mk
	@echo "Not making libcommacts.$(SOEXT)"

$(commactsPAR): $(commactsLIB) $(commactsH) $(commactsI) $(commactsPAREXTRA) $(commactsPAREXTRAMOD) $(commactsPARINIT)
	@echo "Not making $(commactsPAR)"
endif

#
# Dictionary rule
#

commactsDH := $(patsubst %,commacts/%,$(commactsDH))

commacts/G__commacts.h commacts/G__commacts.cxx: config.mk $(ROOTCINT) commacts/commactsLinkDef.h $(commactsDH)
	        $(ROOTCINT) -f commacts/G__commacts.cxx -c $(commactsCF) $(CPPFLAGS) $(commactsDH) $(commactsDHEXTRA) \
		commacts/commactsLinkDef.h


#
# Dependency generation rules
#

commacts/G__%.d: commacts/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(commactsCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

commacts/%.d: commacts/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(commactsCXXFLAGSEXTRA)" $< > $@

commacts/%.d: commacts/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(commactsCXXFLAGSEXTRA)" $< > $@

commacts/%.d: commacts/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(commactsCXXFLAGSEXTRA),)
# Special rule for dictionaries
commacts/G__%.o: commacts/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(commactsCXXFLAGSEXTRA) -o $@ -c $<

commacts/%.o: commacts/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(commactsCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(commactsCFLAGSEXTRA),)
commacts/%.o: commacts/%.c
	$(CC) $(OPT) $(CFLAGS) $(commactsCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(commactsF77FLAGSEXTRA),)
commacts/%.o: commacts/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(commactsF77FLAGSEXTRA) -o $@ -c $<
endif

commacts-map: $(commactsMAP)

ifeq ($(commactsNOLIB),)
$(commactsMAP): commacts/commacts.mk commacts/commactsLinkDef.h
	$(MAKEMAP) -o commacts/commacts.rootmap -l $(commactsMAPLIB) -c commacts/commactsLinkDef.h
else
$(commactsMAP): commacts/commacts.mk commacts/commactsLinkDef.h
	@echo "Not making $(commactsMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
