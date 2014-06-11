# $Id$
#
# Module.mk -- Makefile for ana
# (don't manually edit this file)
#

MODDIR := ana

anaS  := $(filter-out ana/G__%,$(wildcard ana/*.cxx))
anaF  := $(wildcard ana/*.f)

anaH  := $(filter-out ana/%LinkDef.h,$(wildcard ana/*.h*))
anaH  := $(filter-out ana/G__%.h,$(anaH))
anaI  := $(wildcard ana/*.inc)

anaLIB     := lib/libana.$(SOEXT)

anaPAR     := par/ana.par
anaPARINIT := $(shell if test -e "ana/ana_setup.C"; then echo "ana/ana_setup.C"; else echo ""; fi)

anaMAP     := ana/ana.rootmap

anaOMOD        = $(foreach mm,$(patsubst %,%O,$(anaMODULES)),$($(mm)))
anaHMOD        = $(foreach mm,$(patsubst %,%H,$(anaMODULES)),$($(mm)))
anaIMOD        = $(foreach mm,$(patsubst %,%I,$(anaMODULES)),$($(mm)))
anaLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(anaMODULES)),$($(mm)))
anaPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(anaMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include ana/ana.mk



# derive source dependent output
anaO   := $(anaS:.cxx=.o) $(if $(wildcard ana/anaLinkDef.h),ana/G__ana.o) $(anaF:.f=.o)
anaDEP := $(anaO:.o=.d)

ifeq ($(anaMAPLIB),)
anaMAPLIB := $(anaLIB)
else
anaMAPLIB := lib/$(anaMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst ana/%.h,include/%.h,$(anaH))
ALLHDRS     += $(patsubst ana/%.hxx,include/%.hxx,$(anaH))
ALLHDRS     += $(patsubst ana/%.inc,include/%.inc,$(anaI))
ifeq ($(anaNOLIB),)
ALLLIBS	    += $(anaLIB)
ALLPARS     += $(anaPAR)
ALLMAPS     += $(anaMAP)
endif

# include all dependency files
INCLUDEFILES += $(anaDEP)


#
# Populate central include dir
#
include/%.h:    ana/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  ana/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  ana/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs ana-libs pars ana-pars clean ana-clean


ifeq ($(anaNOLIB),)
libs:: ana-libs

ana-libs: $(anaLIB)

pars:: ana-pars

ana-pars: $(anaPAR)
endif

extras:: $(anaEXTRAS)

clean:: ana-clean

ana-clean: $(anaLOCALCLEAN)
	rm -f $(anaLIB) $(anaPAR) $(anaO) $(anaDEP) ana/G__ana.h ana/G__ana.cxx ana/ana.rootmap

ifeq ($(anaNOLIB),)
ana-all: ana-libs ana-pars $(anaEXTRAS)
else
ana-all: $(anaEXTRAS)
endif

ana:: ana-all

ifneq ($(anaF),)
anaLIBEXTRA += $(F77LIBS)
endif

ifeq ($(anaNOLIB),)
$(anaLIB): $(anaLIBDEP) $(anaO) $(anaOMOD) ana/Module.mk ana/ana.mk
	@echo "Making libana.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libana.$(SOEXT) $@ "$(anaO) $(anaOMOD)" \
	"$(anaLIBEXTRA) $(anaLIBEXTRAMOD)"


$(anaPAR): $(anaLIB) $(anaH) $(anaI) $(anaPAREXTRA) $(anaPAREXTRAMOD) $(anaPARINIT)
	@if test -e "$(anaPARINIT)"; then \
	        echo "Making $(anaPAR) using $(anaPARINIT)"; \
         else echo "Making $(anaPAR)" ; fi
	@$(MAKEPAR) $(anaPAR) $(anaLIB) "$(anaH) $(anaI) $(anaHMOD) $(anaIMOD)" \
	"$(anaPAREXTRA) $(anaPAREXTRAMOD)" ana/ana_setup.C
else
$(anaLIB): $(anaLIBDEP) $(anaO) $(anaOMOD) ana/Module.mk ana/ana.mk
	@echo "Not making libana.$(SOEXT)"

$(anaPAR): $(anaLIB) $(anaH) $(anaI) $(anaPAREXTRA) $(anaPAREXTRAMOD) $(anaPARINIT)
	@echo "Not making $(anaPAR)"
endif

#
# Dictionary rule
#

anaDH := $(patsubst %,ana/%,$(anaDH))

ana/G__ana.h ana/G__ana.cxx: config.mk $(ROOTCINT) ana/anaLinkDef.h $(anaDH)
	        $(ROOTCINT) -f ana/G__ana.cxx -c $(anaCF) $(CPPFLAGS) $(anaDH) $(anaDHEXTRA) \
		ana/anaLinkDef.h


#
# Dependency generation rules
#

ana/G__%.d: ana/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(anaCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

ana/%.d: ana/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(anaCXXFLAGSEXTRA)" $< > $@

ana/%.d: ana/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(anaCXXFLAGSEXTRA)" $< > $@

ana/%.d: ana/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(anaCXXFLAGSEXTRA),)
# Special rule for dictionaries
ana/G__%.o: ana/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(anaCXXFLAGSEXTRA) -o $@ -c $<

ana/%.o: ana/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(anaCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(anaCFLAGSEXTRA),)
ana/%.o: ana/%.c
	$(CC) $(OPT) $(CFLAGS) $(anaCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(anaF77FLAGSEXTRA),)
ana/%.o: ana/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(anaF77FLAGSEXTRA) -o $@ -c $<
endif

ana-map: $(anaMAP)

ifeq ($(anaNOLIB),)
$(anaMAP): ana/ana.mk ana/anaLinkDef.h
	$(MAKEMAP) -o ana/ana.rootmap -l $(anaMAPLIB) -c ana/anaLinkDef.h
else
$(anaMAP): ana/ana.mk ana/anaLinkDef.h
	@echo "Not making $(anaMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
