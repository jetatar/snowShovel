# $Id$
#
# Module.mk -- Makefile for sigproc
# (don't manually edit this file)
#

MODDIR := sigproc

sigprocS  := $(filter-out sigproc/G__%,$(wildcard sigproc/*.cxx))
sigprocF  := $(wildcard sigproc/*.f)

sigprocH  := $(filter-out sigproc/%LinkDef.h,$(wildcard sigproc/*.h*))
sigprocH  := $(filter-out sigproc/G__%.h,$(sigprocH))
sigprocI  := $(wildcard sigproc/*.inc)

sigprocLIB     := lib/libsigproc.$(SOEXT)

sigprocPAR     := par/sigproc.par
sigprocPARINIT := $(shell if test -e "sigproc/sigproc_setup.C"; then echo "sigproc/sigproc_setup.C"; else echo ""; fi)

sigprocMAP     := sigproc/sigproc.rootmap

sigprocOMOD        = $(foreach mm,$(patsubst %,%O,$(sigprocMODULES)),$($(mm)))
sigprocHMOD        = $(foreach mm,$(patsubst %,%H,$(sigprocMODULES)),$($(mm)))
sigprocIMOD        = $(foreach mm,$(patsubst %,%I,$(sigprocMODULES)),$($(mm)))
sigprocLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(sigprocMODULES)),$($(mm)))
sigprocPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(sigprocMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include sigproc/sigproc.mk



# derive source dependent output
sigprocO   := $(sigprocS:.cxx=.o) $(if $(wildcard sigproc/sigprocLinkDef.h),sigproc/G__sigproc.o) $(sigprocF:.f=.o)
sigprocDEP := $(sigprocO:.o=.d)

ifeq ($(sigprocMAPLIB),)
sigprocMAPLIB := $(sigprocLIB)
else
sigprocMAPLIB := lib/$(sigprocMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst sigproc/%.h,include/%.h,$(sigprocH))
ALLHDRS     += $(patsubst sigproc/%.hxx,include/%.hxx,$(sigprocH))
ALLHDRS     += $(patsubst sigproc/%.inc,include/%.inc,$(sigprocI))
ifeq ($(sigprocNOLIB),)
ALLLIBS	    += $(sigprocLIB)
ALLPARS     += $(sigprocPAR)
ALLMAPS     += $(sigprocMAP)
endif

# include all dependency files
INCLUDEFILES += $(sigprocDEP)


#
# Populate central include dir
#
include/%.h:    sigproc/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  sigproc/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  sigproc/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs sigproc-libs pars sigproc-pars clean sigproc-clean


ifeq ($(sigprocNOLIB),)
libs:: sigproc-libs

sigproc-libs: $(sigprocLIB)

pars:: sigproc-pars

sigproc-pars: $(sigprocPAR)
endif

extras:: $(sigprocEXTRAS)

clean:: sigproc-clean

sigproc-clean: $(sigprocLOCALCLEAN)
	rm -f $(sigprocLIB) $(sigprocPAR) $(sigprocO) $(sigprocDEP) sigproc/G__sigproc.h sigproc/G__sigproc.cxx sigproc/sigproc.rootmap

ifeq ($(sigprocNOLIB),)
sigproc-all: sigproc-libs sigproc-pars $(sigprocEXTRAS)
else
sigproc-all: $(sigprocEXTRAS)
endif

sigproc:: sigproc-all

ifneq ($(sigprocF),)
sigprocLIBEXTRA += $(F77LIBS)
endif

ifeq ($(sigprocNOLIB),)
$(sigprocLIB): $(sigprocLIBDEP) $(sigprocO) $(sigprocOMOD) sigproc/Module.mk sigproc/sigproc.mk
	@echo "Making libsigproc.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libsigproc.$(SOEXT) $@ "$(sigprocO) $(sigprocOMOD)" \
	"$(sigprocLIBEXTRA) $(sigprocLIBEXTRAMOD)"


$(sigprocPAR): $(sigprocLIB) $(sigprocH) $(sigprocI) $(sigprocPAREXTRA) $(sigprocPAREXTRAMOD) $(sigprocPARINIT)
	@if test -e "$(sigprocPARINIT)"; then \
	        echo "Making $(sigprocPAR) using $(sigprocPARINIT)"; \
         else echo "Making $(sigprocPAR)" ; fi
	@$(MAKEPAR) $(sigprocPAR) $(sigprocLIB) "$(sigprocH) $(sigprocI) $(sigprocHMOD) $(sigprocIMOD)" \
	"$(sigprocPAREXTRA) $(sigprocPAREXTRAMOD)" sigproc/sigproc_setup.C
else
$(sigprocLIB): $(sigprocLIBDEP) $(sigprocO) $(sigprocOMOD) sigproc/Module.mk sigproc/sigproc.mk
	@echo "Not making libsigproc.$(SOEXT)"

$(sigprocPAR): $(sigprocLIB) $(sigprocH) $(sigprocI) $(sigprocPAREXTRA) $(sigprocPAREXTRAMOD) $(sigprocPARINIT)
	@echo "Not making $(sigprocPAR)"
endif

#
# Dictionary rule
#

sigprocDH := $(patsubst %,sigproc/%,$(sigprocDH))

sigproc/G__sigproc.h sigproc/G__sigproc.cxx: config.mk $(ROOTCINT) sigproc/sigprocLinkDef.h $(sigprocDH)
	        $(ROOTCINT) -f sigproc/G__sigproc.cxx -c $(sigprocCF) $(CPPFLAGS) $(sigprocDH) $(sigprocDHEXTRA) \
		sigproc/sigprocLinkDef.h


#
# Dependency generation rules
#

sigproc/G__%.d: sigproc/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(sigprocCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

sigproc/%.d: sigproc/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(sigprocCXXFLAGSEXTRA)" $< > $@

sigproc/%.d: sigproc/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(sigprocCXXFLAGSEXTRA)" $< > $@

sigproc/%.d: sigproc/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(sigprocCXXFLAGSEXTRA),)
# Special rule for dictionaries
sigproc/G__%.o: sigproc/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(sigprocCXXFLAGSEXTRA) -o $@ -c $<

sigproc/%.o: sigproc/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(sigprocCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(sigprocCFLAGSEXTRA),)
sigproc/%.o: sigproc/%.c
	$(CC) $(OPT) $(CFLAGS) $(sigprocCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(sigprocF77FLAGSEXTRA),)
sigproc/%.o: sigproc/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(sigprocF77FLAGSEXTRA) -o $@ -c $<
endif

sigproc-map: $(sigprocMAP)

ifeq ($(sigprocNOLIB),)
$(sigprocMAP): sigproc/sigproc.mk sigproc/sigprocLinkDef.h
	$(MAKEMAP) -o sigproc/sigproc.rootmap -l $(sigprocMAPLIB) -c sigproc/sigprocLinkDef.h
else
$(sigprocMAP): sigproc/sigproc.mk sigproc/sigprocLinkDef.h
	@echo "Not making $(sigprocMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
