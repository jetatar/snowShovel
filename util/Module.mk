# $Id$
#
# Module.mk -- Makefile for util
# (don't manually edit this file)
#

MODDIR := util

utilS  := $(filter-out util/G__%,$(wildcard util/*.cxx))
utilF  := $(wildcard util/*.f)

utilH  := $(filter-out util/%LinkDef.h,$(wildcard util/*.h*))
utilH  := $(filter-out util/G__%.h,$(utilH))
utilI  := $(wildcard util/*.inc)

utilLIB     := lib/libutil.$(SOEXT)

utilPAR     := par/util.par
utilPARINIT := $(shell if test -e "util/util_setup.C"; then echo "util/util_setup.C"; else echo ""; fi)

utilMAP     := util/util.rootmap

utilOMOD        = $(foreach mm,$(patsubst %,%O,$(utilMODULES)),$($(mm)))
utilHMOD        = $(foreach mm,$(patsubst %,%H,$(utilMODULES)),$($(mm)))
utilIMOD        = $(foreach mm,$(patsubst %,%I,$(utilMODULES)),$($(mm)))
utilLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(utilMODULES)),$($(mm)))
utilPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(utilMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include util/util.mk



# derive source dependent output
utilO   := $(utilS:.cxx=.o) $(if $(wildcard util/utilLinkDef.h),util/G__util.o) $(utilF:.f=.o)
utilDEP := $(utilO:.o=.d)

ifeq ($(utilMAPLIB),)
utilMAPLIB := $(utilLIB)
else
utilMAPLIB := lib/$(utilMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst util/%.h,include/%.h,$(utilH))
ALLHDRS     += $(patsubst util/%.hxx,include/%.hxx,$(utilH))
ALLHDRS     += $(patsubst util/%.inc,include/%.inc,$(utilI))
ifeq ($(utilNOLIB),)
ALLLIBS	    += $(utilLIB)
ALLPARS     += $(utilPAR)
ALLMAPS     += $(utilMAP)
endif

# include all dependency files
INCLUDEFILES += $(utilDEP)


#
# Populate central include dir
#
include/%.h:    util/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  util/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  util/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs util-libs pars util-pars clean util-clean


ifeq ($(utilNOLIB),)
libs:: util-libs

util-libs: $(utilLIB)

pars:: util-pars

util-pars: $(utilPAR)
endif

extras:: $(utilEXTRAS)

clean:: util-clean

util-clean: $(utilLOCALCLEAN)
	rm -f $(utilLIB) $(utilPAR) $(utilO) $(utilDEP) util/G__util.h util/G__util.cxx util/util.rootmap

ifeq ($(utilNOLIB),)
util-all: util-libs util-pars $(utilEXTRAS)
else
util-all: $(utilEXTRAS)
endif

util:: util-all

ifneq ($(utilF),)
utilLIBEXTRA += $(F77LIBS)
endif

ifeq ($(utilNOLIB),)
$(utilLIB): $(utilLIBDEP) $(utilO) $(utilOMOD) util/Module.mk util/util.mk
	@echo "Making libutil.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libutil.$(SOEXT) $@ "$(utilO) $(utilOMOD)" \
	"$(utilLIBEXTRA) $(utilLIBEXTRAMOD)"


$(utilPAR): $(utilLIB) $(utilH) $(utilI) $(utilPAREXTRA) $(utilPAREXTRAMOD) $(utilPARINIT)
	@if test -e "$(utilPARINIT)"; then \
	        echo "Making $(utilPAR) using $(utilPARINIT)"; \
         else echo "Making $(utilPAR)" ; fi
	@$(MAKEPAR) $(utilPAR) $(utilLIB) "$(utilH) $(utilI) $(utilHMOD) $(utilIMOD)" \
	"$(utilPAREXTRA) $(utilPAREXTRAMOD)" util/util_setup.C
else
$(utilLIB): $(utilLIBDEP) $(utilO) $(utilOMOD) util/Module.mk util/util.mk
	@echo "Not making libutil.$(SOEXT)"

$(utilPAR): $(utilLIB) $(utilH) $(utilI) $(utilPAREXTRA) $(utilPAREXTRAMOD) $(utilPARINIT)
	@echo "Not making $(utilPAR)"
endif

#
# Dictionary rule
#

utilDH := $(patsubst %,util/%,$(utilDH))

util/G__util.h util/G__util.cxx: config.mk $(ROOTCINT) util/utilLinkDef.h $(utilDH)
	        $(ROOTCINT) -f util/G__util.cxx -c $(utilCF) $(CPPFLAGS) $(utilDH) $(utilDHEXTRA) \
		util/utilLinkDef.h


#
# Dependency generation rules
#

util/G__%.d: util/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(utilCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

util/%.d: util/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(utilCXXFLAGSEXTRA)" $< > $@

util/%.d: util/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(utilCXXFLAGSEXTRA)" $< > $@

util/%.d: util/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(utilCXXFLAGSEXTRA),)
# Special rule for dictionaries
util/G__%.o: util/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(utilCXXFLAGSEXTRA) -o $@ -c $<

util/%.o: util/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(utilCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(utilCFLAGSEXTRA),)
util/%.o: util/%.c
	$(CC) $(OPT) $(CFLAGS) $(utilCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(utilF77FLAGSEXTRA),)
util/%.o: util/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(utilF77FLAGSEXTRA) -o $@ -c $<
endif

util-map: $(utilMAP)

ifeq ($(utilNOLIB),)
$(utilMAP): util/util.mk util/utilLinkDef.h
	$(MAKEMAP) -o util/util.rootmap -l $(utilMAPLIB) -c util/utilLinkDef.h
else
$(utilMAP): util/util.mk util/utilLinkDef.h
	@echo "Not making $(utilMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
