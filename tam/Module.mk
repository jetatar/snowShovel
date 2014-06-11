# $Id$
#
# Module.mk -- Makefile for tam
# (don't manually edit this file)
#

MODDIR := tam

tamS  := $(filter-out tam/G__%,$(wildcard tam/*.cxx))
tamF  := $(wildcard tam/*.f)

tamH  := $(filter-out tam/%LinkDef.h,$(wildcard tam/*.h*))
tamH  := $(filter-out tam/G__%.h,$(tamH))
tamI  := $(wildcard tam/*.inc)

tamLIB     := lib/libtam.$(SOEXT)

tamPAR     := par/tam.par
tamPARINIT := $(shell if test -e "tam/tam_setup.C"; then echo "tam/tam_setup.C"; else echo ""; fi)

tamMAP     := tam/tam.rootmap

tamOMOD        = $(foreach mm,$(patsubst %,%O,$(tamMODULES)),$($(mm)))
tamHMOD        = $(foreach mm,$(patsubst %,%H,$(tamMODULES)),$($(mm)))
tamIMOD        = $(foreach mm,$(patsubst %,%I,$(tamMODULES)),$($(mm)))
tamLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(tamMODULES)),$($(mm)))
tamPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(tamMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include tam/tam.mk



# derive source dependent output
tamO   := $(tamS:.cxx=.o) $(if $(wildcard tam/tamLinkDef.h),tam/G__tam.o) $(tamF:.f=.o)
tamDEP := $(tamO:.o=.d)

ifeq ($(tamMAPLIB),)
tamMAPLIB := $(tamLIB)
else
tamMAPLIB := lib/$(tamMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst tam/%.h,include/%.h,$(tamH))
ALLHDRS     += $(patsubst tam/%.hxx,include/%.hxx,$(tamH))
ALLHDRS     += $(patsubst tam/%.inc,include/%.inc,$(tamI))
ifeq ($(tamNOLIB),)
ALLLIBS	    += $(tamLIB)
ALLPARS     += $(tamPAR)
ALLMAPS     += $(tamMAP)
endif

# include all dependency files
INCLUDEFILES += $(tamDEP)


#
# Populate central include dir
#
include/%.h:    tam/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  tam/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  tam/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs tam-libs pars tam-pars clean tam-clean


ifeq ($(tamNOLIB),)
libs:: tam-libs

tam-libs: $(tamLIB)

pars:: tam-pars

tam-pars: $(tamPAR)
endif

extras:: $(tamEXTRAS)

clean:: tam-clean

tam-clean: $(tamLOCALCLEAN)
	rm -f $(tamLIB) $(tamPAR) $(tamO) $(tamDEP) tam/G__tam.h tam/G__tam.cxx tam/tam.rootmap

ifeq ($(tamNOLIB),)
tam-all: tam-libs tam-pars $(tamEXTRAS)
else
tam-all: $(tamEXTRAS)
endif

tam:: tam-all

ifneq ($(tamF),)
tamLIBEXTRA += $(F77LIBS)
endif

ifeq ($(tamNOLIB),)
$(tamLIB): $(tamLIBDEP) $(tamO) $(tamOMOD) tam/Module.mk tam/tam.mk
	@echo "Making libtam.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libtam.$(SOEXT) $@ "$(tamO) $(tamOMOD)" \
	"$(tamLIBEXTRA) $(tamLIBEXTRAMOD)"


$(tamPAR): $(tamLIB) $(tamH) $(tamI) $(tamPAREXTRA) $(tamPAREXTRAMOD) $(tamPARINIT)
	@if test -e "$(tamPARINIT)"; then \
	        echo "Making $(tamPAR) using $(tamPARINIT)"; \
         else echo "Making $(tamPAR)" ; fi
	@$(MAKEPAR) $(tamPAR) $(tamLIB) "$(tamH) $(tamI) $(tamHMOD) $(tamIMOD)" \
	"$(tamPAREXTRA) $(tamPAREXTRAMOD)" tam/tam_setup.C
else
$(tamLIB): $(tamLIBDEP) $(tamO) $(tamOMOD) tam/Module.mk tam/tam.mk
	@echo "Not making libtam.$(SOEXT)"

$(tamPAR): $(tamLIB) $(tamH) $(tamI) $(tamPAREXTRA) $(tamPAREXTRAMOD) $(tamPARINIT)
	@echo "Not making $(tamPAR)"
endif

#
# Dictionary rule
#

tamDH := $(patsubst %,tam/%,$(tamDH))

tam/G__tam.h tam/G__tam.cxx: config.mk $(ROOTCINT) tam/tamLinkDef.h $(tamDH)
	        $(ROOTCINT) -f tam/G__tam.cxx -c $(tamCF) $(CPPFLAGS) $(tamDH) $(tamDHEXTRA) \
		tam/tamLinkDef.h


#
# Dependency generation rules
#

tam/G__%.d: tam/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(tamCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

tam/%.d: tam/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(tamCXXFLAGSEXTRA)" $< > $@

tam/%.d: tam/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(tamCXXFLAGSEXTRA)" $< > $@

tam/%.d: tam/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(tamCXXFLAGSEXTRA),)
# Special rule for dictionaries
tam/G__%.o: tam/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(tamCXXFLAGSEXTRA) -o $@ -c $<

tam/%.o: tam/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(tamCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(tamCFLAGSEXTRA),)
tam/%.o: tam/%.c
	$(CC) $(OPT) $(CFLAGS) $(tamCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(tamF77FLAGSEXTRA),)
tam/%.o: tam/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(tamF77FLAGSEXTRA) -o $@ -c $<
endif

tam-map: $(tamMAP)

ifeq ($(tamNOLIB),)
$(tamMAP): tam/tam.mk tam/tamLinkDef.h
	$(MAKEMAP) -o tam/tam.rootmap -l $(tamMAPLIB) -c tam/tamLinkDef.h
else
$(tamMAP): tam/tam.mk tam/tamLinkDef.h
	@echo "Not making $(tamMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
