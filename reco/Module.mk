# $Id$
#
# Module.mk -- Makefile for reco
# (don't manually edit this file)
#

MODDIR := reco

recoS  := $(filter-out reco/G__%,$(wildcard reco/*.cxx))
recoF  := $(wildcard reco/*.f)

recoH  := $(filter-out reco/%LinkDef.h,$(wildcard reco/*.h*))
recoH  := $(filter-out reco/G__%.h,$(recoH))
recoI  := $(wildcard reco/*.inc)

recoLIB     := lib/libreco.$(SOEXT)

recoPAR     := par/reco.par
recoPARINIT := $(shell if test -e "reco/reco_setup.C"; then echo "reco/reco_setup.C"; else echo ""; fi)

recoMAP     := reco/reco.rootmap

recoOMOD        = $(foreach mm,$(patsubst %,%O,$(recoMODULES)),$($(mm)))
recoHMOD        = $(foreach mm,$(patsubst %,%H,$(recoMODULES)),$($(mm)))
recoIMOD        = $(foreach mm,$(patsubst %,%I,$(recoMODULES)),$($(mm)))
recoLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(recoMODULES)),$($(mm)))
recoPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(recoMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include reco/reco.mk



# derive source dependent output
recoO   := $(recoS:.cxx=.o) $(if $(wildcard reco/recoLinkDef.h),reco/G__reco.o) $(recoF:.f=.o)
recoDEP := $(recoO:.o=.d)

ifeq ($(recoMAPLIB),)
recoMAPLIB := $(recoLIB)
else
recoMAPLIB := lib/$(recoMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst reco/%.h,include/%.h,$(recoH))
ALLHDRS     += $(patsubst reco/%.hxx,include/%.hxx,$(recoH))
ALLHDRS     += $(patsubst reco/%.inc,include/%.inc,$(recoI))
ifeq ($(recoNOLIB),)
ALLLIBS	    += $(recoLIB)
ALLPARS     += $(recoPAR)
ALLMAPS     += $(recoMAP)
endif

# include all dependency files
INCLUDEFILES += $(recoDEP)


#
# Populate central include dir
#
include/%.h:    reco/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  reco/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  reco/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs reco-libs pars reco-pars clean reco-clean


ifeq ($(recoNOLIB),)
libs:: reco-libs

reco-libs: $(recoLIB)

pars:: reco-pars

reco-pars: $(recoPAR)
endif

extras:: $(recoEXTRAS)

clean:: reco-clean

reco-clean: $(recoLOCALCLEAN)
	rm -f $(recoLIB) $(recoPAR) $(recoO) $(recoDEP) reco/G__reco.h reco/G__reco.cxx reco/reco.rootmap

ifeq ($(recoNOLIB),)
reco-all: reco-libs reco-pars $(recoEXTRAS)
else
reco-all: $(recoEXTRAS)
endif

reco:: reco-all

ifneq ($(recoF),)
recoLIBEXTRA += $(F77LIBS)
endif

ifeq ($(recoNOLIB),)
$(recoLIB): $(recoLIBDEP) $(recoO) $(recoOMOD) reco/Module.mk reco/reco.mk
	@echo "Making libreco.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libreco.$(SOEXT) $@ "$(recoO) $(recoOMOD)" \
	"$(recoLIBEXTRA) $(recoLIBEXTRAMOD)"


$(recoPAR): $(recoLIB) $(recoH) $(recoI) $(recoPAREXTRA) $(recoPAREXTRAMOD) $(recoPARINIT)
	@if test -e "$(recoPARINIT)"; then \
	        echo "Making $(recoPAR) using $(recoPARINIT)"; \
         else echo "Making $(recoPAR)" ; fi
	@$(MAKEPAR) $(recoPAR) $(recoLIB) "$(recoH) $(recoI) $(recoHMOD) $(recoIMOD)" \
	"$(recoPAREXTRA) $(recoPAREXTRAMOD)" reco/reco_setup.C
else
$(recoLIB): $(recoLIBDEP) $(recoO) $(recoOMOD) reco/Module.mk reco/reco.mk
	@echo "Not making libreco.$(SOEXT)"

$(recoPAR): $(recoLIB) $(recoH) $(recoI) $(recoPAREXTRA) $(recoPAREXTRAMOD) $(recoPARINIT)
	@echo "Not making $(recoPAR)"
endif

#
# Dictionary rule
#

recoDH := $(patsubst %,reco/%,$(recoDH))

reco/G__reco.h reco/G__reco.cxx: config.mk $(ROOTCINT) reco/recoLinkDef.h $(recoDH)
	        $(ROOTCINT) -f reco/G__reco.cxx -c $(recoCF) $(CPPFLAGS) $(recoDH) $(recoDHEXTRA) \
		reco/recoLinkDef.h


#
# Dependency generation rules
#

reco/G__%.d: reco/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(recoCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

reco/%.d: reco/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(recoCXXFLAGSEXTRA)" $< > $@

reco/%.d: reco/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(recoCXXFLAGSEXTRA)" $< > $@

reco/%.d: reco/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(recoCXXFLAGSEXTRA),)
# Special rule for dictionaries
reco/G__%.o: reco/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(recoCXXFLAGSEXTRA) -o $@ -c $<

reco/%.o: reco/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(recoCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(recoCFLAGSEXTRA),)
reco/%.o: reco/%.c
	$(CC) $(OPT) $(CFLAGS) $(recoCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(recoF77FLAGSEXTRA),)
reco/%.o: reco/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(recoF77FLAGSEXTRA) -o $@ -c $<
endif

reco-map: $(recoMAP)

ifeq ($(recoNOLIB),)
$(recoMAP): reco/reco.mk reco/recoLinkDef.h
	$(MAKEMAP) -o reco/reco.rootmap -l $(recoMAPLIB) -c reco/recoLinkDef.h
else
$(recoMAP): reco/reco.mk reco/recoLinkDef.h
	@echo "Not making $(recoMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
