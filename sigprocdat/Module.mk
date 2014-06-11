# $Id$
#
# Module.mk -- Makefile for sigprocdat
# (don't manually edit this file)
#

MODDIR := sigprocdat

sigprocdatS  := $(filter-out sigprocdat/G__%,$(wildcard sigprocdat/*.cxx))
sigprocdatF  := $(wildcard sigprocdat/*.f)

sigprocdatH  := $(filter-out sigprocdat/%LinkDef.h,$(wildcard sigprocdat/*.h*))
sigprocdatH  := $(filter-out sigprocdat/G__%.h,$(sigprocdatH))
sigprocdatI  := $(wildcard sigprocdat/*.inc)

sigprocdatLIB     := lib/libsigprocdat.$(SOEXT)

sigprocdatPAR     := par/sigprocdat.par
sigprocdatPARINIT := $(shell if test -e "sigprocdat/sigprocdat_setup.C"; then echo "sigprocdat/sigprocdat_setup.C"; else echo ""; fi)

sigprocdatMAP     := sigprocdat/sigprocdat.rootmap

sigprocdatOMOD        = $(foreach mm,$(patsubst %,%O,$(sigprocdatMODULES)),$($(mm)))
sigprocdatHMOD        = $(foreach mm,$(patsubst %,%H,$(sigprocdatMODULES)),$($(mm)))
sigprocdatIMOD        = $(foreach mm,$(patsubst %,%I,$(sigprocdatMODULES)),$($(mm)))
sigprocdatLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(sigprocdatMODULES)),$($(mm)))
sigprocdatPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(sigprocdatMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include sigprocdat/sigprocdat.mk



# derive source dependent output
sigprocdatO   := $(sigprocdatS:.cxx=.o) $(if $(wildcard sigprocdat/sigprocdatLinkDef.h),sigprocdat/G__sigprocdat.o) $(sigprocdatF:.f=.o)
sigprocdatDEP := $(sigprocdatO:.o=.d)

ifeq ($(sigprocdatMAPLIB),)
sigprocdatMAPLIB := $(sigprocdatLIB)
else
sigprocdatMAPLIB := lib/$(sigprocdatMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst sigprocdat/%.h,include/%.h,$(sigprocdatH))
ALLHDRS     += $(patsubst sigprocdat/%.hxx,include/%.hxx,$(sigprocdatH))
ALLHDRS     += $(patsubst sigprocdat/%.inc,include/%.inc,$(sigprocdatI))
ifeq ($(sigprocdatNOLIB),)
ALLLIBS	    += $(sigprocdatLIB)
ALLPARS     += $(sigprocdatPAR)
ALLMAPS     += $(sigprocdatMAP)
endif

# include all dependency files
INCLUDEFILES += $(sigprocdatDEP)


#
# Populate central include dir
#
include/%.h:    sigprocdat/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  sigprocdat/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  sigprocdat/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs sigprocdat-libs pars sigprocdat-pars clean sigprocdat-clean


ifeq ($(sigprocdatNOLIB),)
libs:: sigprocdat-libs

sigprocdat-libs: $(sigprocdatLIB)

pars:: sigprocdat-pars

sigprocdat-pars: $(sigprocdatPAR)
endif

extras:: $(sigprocdatEXTRAS)

clean:: sigprocdat-clean

sigprocdat-clean: $(sigprocdatLOCALCLEAN)
	rm -f $(sigprocdatLIB) $(sigprocdatPAR) $(sigprocdatO) $(sigprocdatDEP) sigprocdat/G__sigprocdat.h sigprocdat/G__sigprocdat.cxx sigprocdat/sigprocdat.rootmap

ifeq ($(sigprocdatNOLIB),)
sigprocdat-all: sigprocdat-libs sigprocdat-pars $(sigprocdatEXTRAS)
else
sigprocdat-all: $(sigprocdatEXTRAS)
endif

sigprocdat:: sigprocdat-all

ifneq ($(sigprocdatF),)
sigprocdatLIBEXTRA += $(F77LIBS)
endif

ifeq ($(sigprocdatNOLIB),)
$(sigprocdatLIB): $(sigprocdatLIBDEP) $(sigprocdatO) $(sigprocdatOMOD) sigprocdat/Module.mk sigprocdat/sigprocdat.mk
	@echo "Making libsigprocdat.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libsigprocdat.$(SOEXT) $@ "$(sigprocdatO) $(sigprocdatOMOD)" \
	"$(sigprocdatLIBEXTRA) $(sigprocdatLIBEXTRAMOD)"


$(sigprocdatPAR): $(sigprocdatLIB) $(sigprocdatH) $(sigprocdatI) $(sigprocdatPAREXTRA) $(sigprocdatPAREXTRAMOD) $(sigprocdatPARINIT)
	@if test -e "$(sigprocdatPARINIT)"; then \
	        echo "Making $(sigprocdatPAR) using $(sigprocdatPARINIT)"; \
         else echo "Making $(sigprocdatPAR)" ; fi
	@$(MAKEPAR) $(sigprocdatPAR) $(sigprocdatLIB) "$(sigprocdatH) $(sigprocdatI) $(sigprocdatHMOD) $(sigprocdatIMOD)" \
	"$(sigprocdatPAREXTRA) $(sigprocdatPAREXTRAMOD)" sigprocdat/sigprocdat_setup.C
else
$(sigprocdatLIB): $(sigprocdatLIBDEP) $(sigprocdatO) $(sigprocdatOMOD) sigprocdat/Module.mk sigprocdat/sigprocdat.mk
	@echo "Not making libsigprocdat.$(SOEXT)"

$(sigprocdatPAR): $(sigprocdatLIB) $(sigprocdatH) $(sigprocdatI) $(sigprocdatPAREXTRA) $(sigprocdatPAREXTRAMOD) $(sigprocdatPARINIT)
	@echo "Not making $(sigprocdatPAR)"
endif

#
# Dictionary rule
#

sigprocdatDH := $(patsubst %,sigprocdat/%,$(sigprocdatDH))

sigprocdat/G__sigprocdat.h sigprocdat/G__sigprocdat.cxx: config.mk $(ROOTCINT) sigprocdat/sigprocdatLinkDef.h $(sigprocdatDH)
	        $(ROOTCINT) -f sigprocdat/G__sigprocdat.cxx -c $(sigprocdatCF) $(CPPFLAGS) $(sigprocdatDH) $(sigprocdatDHEXTRA) \
		sigprocdat/sigprocdatLinkDef.h


#
# Dependency generation rules
#

sigprocdat/G__%.d: sigprocdat/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(sigprocdatCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

sigprocdat/%.d: sigprocdat/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(sigprocdatCXXFLAGSEXTRA)" $< > $@

sigprocdat/%.d: sigprocdat/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(sigprocdatCXXFLAGSEXTRA)" $< > $@

sigprocdat/%.d: sigprocdat/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(sigprocdatCXXFLAGSEXTRA),)
# Special rule for dictionaries
sigprocdat/G__%.o: sigprocdat/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(sigprocdatCXXFLAGSEXTRA) -o $@ -c $<

sigprocdat/%.o: sigprocdat/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(sigprocdatCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(sigprocdatCFLAGSEXTRA),)
sigprocdat/%.o: sigprocdat/%.c
	$(CC) $(OPT) $(CFLAGS) $(sigprocdatCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(sigprocdatF77FLAGSEXTRA),)
sigprocdat/%.o: sigprocdat/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(sigprocdatF77FLAGSEXTRA) -o $@ -c $<
endif

sigprocdat-map: $(sigprocdatMAP)

ifeq ($(sigprocdatNOLIB),)
$(sigprocdatMAP): sigprocdat/sigprocdat.mk sigprocdat/sigprocdatLinkDef.h
	$(MAKEMAP) -o sigprocdat/sigprocdat.rootmap -l $(sigprocdatMAPLIB) -c sigprocdat/sigprocdatLinkDef.h
else
$(sigprocdatMAP): sigprocdat/sigprocdat.mk sigprocdat/sigprocdatLinkDef.h
	@echo "Not making $(sigprocdatMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
