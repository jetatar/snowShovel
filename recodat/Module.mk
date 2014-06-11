# $Id$
#
# Module.mk -- Makefile for recodat
# (don't manually edit this file)
#

MODDIR := recodat

recodatS  := $(filter-out recodat/G__%,$(wildcard recodat/*.cxx))
recodatF  := $(wildcard recodat/*.f)

recodatH  := $(filter-out recodat/%LinkDef.h,$(wildcard recodat/*.h*))
recodatH  := $(filter-out recodat/G__%.h,$(recodatH))
recodatI  := $(wildcard recodat/*.inc)

recodatLIB     := lib/librecodat.$(SOEXT)

recodatPAR     := par/recodat.par
recodatPARINIT := $(shell if test -e "recodat/recodat_setup.C"; then echo "recodat/recodat_setup.C"; else echo ""; fi)

recodatMAP     := recodat/recodat.rootmap

recodatOMOD        = $(foreach mm,$(patsubst %,%O,$(recodatMODULES)),$($(mm)))
recodatHMOD        = $(foreach mm,$(patsubst %,%H,$(recodatMODULES)),$($(mm)))
recodatIMOD        = $(foreach mm,$(patsubst %,%I,$(recodatMODULES)),$($(mm)))
recodatLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(recodatMODULES)),$($(mm)))
recodatPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(recodatMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include recodat/recodat.mk



# derive source dependent output
recodatO   := $(recodatS:.cxx=.o) $(if $(wildcard recodat/recodatLinkDef.h),recodat/G__recodat.o) $(recodatF:.f=.o)
recodatDEP := $(recodatO:.o=.d)

ifeq ($(recodatMAPLIB),)
recodatMAPLIB := $(recodatLIB)
else
recodatMAPLIB := lib/$(recodatMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst recodat/%.h,include/%.h,$(recodatH))
ALLHDRS     += $(patsubst recodat/%.hxx,include/%.hxx,$(recodatH))
ALLHDRS     += $(patsubst recodat/%.inc,include/%.inc,$(recodatI))
ifeq ($(recodatNOLIB),)
ALLLIBS	    += $(recodatLIB)
ALLPARS     += $(recodatPAR)
ALLMAPS     += $(recodatMAP)
endif

# include all dependency files
INCLUDEFILES += $(recodatDEP)


#
# Populate central include dir
#
include/%.h:    recodat/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  recodat/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  recodat/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs recodat-libs pars recodat-pars clean recodat-clean


ifeq ($(recodatNOLIB),)
libs:: recodat-libs

recodat-libs: $(recodatLIB)

pars:: recodat-pars

recodat-pars: $(recodatPAR)
endif

extras:: $(recodatEXTRAS)

clean:: recodat-clean

recodat-clean: $(recodatLOCALCLEAN)
	rm -f $(recodatLIB) $(recodatPAR) $(recodatO) $(recodatDEP) recodat/G__recodat.h recodat/G__recodat.cxx recodat/recodat.rootmap

ifeq ($(recodatNOLIB),)
recodat-all: recodat-libs recodat-pars $(recodatEXTRAS)
else
recodat-all: $(recodatEXTRAS)
endif

recodat:: recodat-all

ifneq ($(recodatF),)
recodatLIBEXTRA += $(F77LIBS)
endif

ifeq ($(recodatNOLIB),)
$(recodatLIB): $(recodatLIBDEP) $(recodatO) $(recodatOMOD) recodat/Module.mk recodat/recodat.mk
	@echo "Making librecodat.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" librecodat.$(SOEXT) $@ "$(recodatO) $(recodatOMOD)" \
	"$(recodatLIBEXTRA) $(recodatLIBEXTRAMOD)"


$(recodatPAR): $(recodatLIB) $(recodatH) $(recodatI) $(recodatPAREXTRA) $(recodatPAREXTRAMOD) $(recodatPARINIT)
	@if test -e "$(recodatPARINIT)"; then \
	        echo "Making $(recodatPAR) using $(recodatPARINIT)"; \
         else echo "Making $(recodatPAR)" ; fi
	@$(MAKEPAR) $(recodatPAR) $(recodatLIB) "$(recodatH) $(recodatI) $(recodatHMOD) $(recodatIMOD)" \
	"$(recodatPAREXTRA) $(recodatPAREXTRAMOD)" recodat/recodat_setup.C
else
$(recodatLIB): $(recodatLIBDEP) $(recodatO) $(recodatOMOD) recodat/Module.mk recodat/recodat.mk
	@echo "Not making librecodat.$(SOEXT)"

$(recodatPAR): $(recodatLIB) $(recodatH) $(recodatI) $(recodatPAREXTRA) $(recodatPAREXTRAMOD) $(recodatPARINIT)
	@echo "Not making $(recodatPAR)"
endif

#
# Dictionary rule
#

recodatDH := $(patsubst %,recodat/%,$(recodatDH))

recodat/G__recodat.h recodat/G__recodat.cxx: config.mk $(ROOTCINT) recodat/recodatLinkDef.h $(recodatDH)
	        $(ROOTCINT) -f recodat/G__recodat.cxx -c $(recodatCF) $(CPPFLAGS) $(recodatDH) $(recodatDHEXTRA) \
		recodat/recodatLinkDef.h


#
# Dependency generation rules
#

recodat/G__%.d: recodat/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(recodatCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

recodat/%.d: recodat/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(recodatCXXFLAGSEXTRA)" $< > $@

recodat/%.d: recodat/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(recodatCXXFLAGSEXTRA)" $< > $@

recodat/%.d: recodat/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(recodatCXXFLAGSEXTRA),)
# Special rule for dictionaries
recodat/G__%.o: recodat/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(recodatCXXFLAGSEXTRA) -o $@ -c $<

recodat/%.o: recodat/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(recodatCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(recodatCFLAGSEXTRA),)
recodat/%.o: recodat/%.c
	$(CC) $(OPT) $(CFLAGS) $(recodatCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(recodatF77FLAGSEXTRA),)
recodat/%.o: recodat/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(recodatF77FLAGSEXTRA) -o $@ -c $<
endif

recodat-map: $(recodatMAP)

ifeq ($(recodatNOLIB),)
$(recodatMAP): recodat/recodat.mk recodat/recodatLinkDef.h
	$(MAKEMAP) -o recodat/recodat.rootmap -l $(recodatMAPLIB) -c recodat/recodatLinkDef.h
else
$(recodatMAP): recodat/recodat.mk recodat/recodatLinkDef.h
	@echo "Not making $(recodatMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
