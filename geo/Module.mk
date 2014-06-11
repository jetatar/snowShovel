# $Id$
#
# Module.mk -- Makefile for geo
# (don't manually edit this file)
#

MODDIR := geo

geoS  := $(filter-out geo/G__%,$(wildcard geo/*.cxx))
geoF  := $(wildcard geo/*.f)

geoH  := $(filter-out geo/%LinkDef.h,$(wildcard geo/*.h*))
geoH  := $(filter-out geo/G__%.h,$(geoH))
geoI  := $(wildcard geo/*.inc)

geoLIB     := lib/libgeo.$(SOEXT)

geoPAR     := par/geo.par
geoPARINIT := $(shell if test -e "geo/geo_setup.C"; then echo "geo/geo_setup.C"; else echo ""; fi)

geoMAP     := geo/geo.rootmap

geoOMOD        = $(foreach mm,$(patsubst %,%O,$(geoMODULES)),$($(mm)))
geoHMOD        = $(foreach mm,$(patsubst %,%H,$(geoMODULES)),$($(mm)))
geoIMOD        = $(foreach mm,$(patsubst %,%I,$(geoMODULES)),$($(mm)))
geoLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(geoMODULES)),$($(mm)))
geoPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(geoMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include geo/geo.mk



# derive source dependent output
geoO   := $(geoS:.cxx=.o) $(if $(wildcard geo/geoLinkDef.h),geo/G__geo.o) $(geoF:.f=.o)
geoDEP := $(geoO:.o=.d)

ifeq ($(geoMAPLIB),)
geoMAPLIB := $(geoLIB)
else
geoMAPLIB := lib/$(geoMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst geo/%.h,include/%.h,$(geoH))
ALLHDRS     += $(patsubst geo/%.hxx,include/%.hxx,$(geoH))
ALLHDRS     += $(patsubst geo/%.inc,include/%.inc,$(geoI))
ifeq ($(geoNOLIB),)
ALLLIBS	    += $(geoLIB)
ALLPARS     += $(geoPAR)
ALLMAPS     += $(geoMAP)
endif

# include all dependency files
INCLUDEFILES += $(geoDEP)


#
# Populate central include dir
#
include/%.h:    geo/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  geo/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  geo/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs geo-libs pars geo-pars clean geo-clean


ifeq ($(geoNOLIB),)
libs:: geo-libs

geo-libs: $(geoLIB)

pars:: geo-pars

geo-pars: $(geoPAR)
endif

extras:: $(geoEXTRAS)

clean:: geo-clean

geo-clean: $(geoLOCALCLEAN)
	rm -f $(geoLIB) $(geoPAR) $(geoO) $(geoDEP) geo/G__geo.h geo/G__geo.cxx geo/geo.rootmap

ifeq ($(geoNOLIB),)
geo-all: geo-libs geo-pars $(geoEXTRAS)
else
geo-all: $(geoEXTRAS)
endif

geo:: geo-all

ifneq ($(geoF),)
geoLIBEXTRA += $(F77LIBS)
endif

ifeq ($(geoNOLIB),)
$(geoLIB): $(geoLIBDEP) $(geoO) $(geoOMOD) geo/Module.mk geo/geo.mk
	@echo "Making libgeo.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libgeo.$(SOEXT) $@ "$(geoO) $(geoOMOD)" \
	"$(geoLIBEXTRA) $(geoLIBEXTRAMOD)"


$(geoPAR): $(geoLIB) $(geoH) $(geoI) $(geoPAREXTRA) $(geoPAREXTRAMOD) $(geoPARINIT)
	@if test -e "$(geoPARINIT)"; then \
	        echo "Making $(geoPAR) using $(geoPARINIT)"; \
         else echo "Making $(geoPAR)" ; fi
	@$(MAKEPAR) $(geoPAR) $(geoLIB) "$(geoH) $(geoI) $(geoHMOD) $(geoIMOD)" \
	"$(geoPAREXTRA) $(geoPAREXTRAMOD)" geo/geo_setup.C
else
$(geoLIB): $(geoLIBDEP) $(geoO) $(geoOMOD) geo/Module.mk geo/geo.mk
	@echo "Not making libgeo.$(SOEXT)"

$(geoPAR): $(geoLIB) $(geoH) $(geoI) $(geoPAREXTRA) $(geoPAREXTRAMOD) $(geoPARINIT)
	@echo "Not making $(geoPAR)"
endif

#
# Dictionary rule
#

geoDH := $(patsubst %,geo/%,$(geoDH))

geo/G__geo.h geo/G__geo.cxx: config.mk $(ROOTCINT) geo/geoLinkDef.h $(geoDH)
	        $(ROOTCINT) -f geo/G__geo.cxx -c $(geoCF) $(CPPFLAGS) $(geoDH) $(geoDHEXTRA) \
		geo/geoLinkDef.h


#
# Dependency generation rules
#

geo/G__%.d: geo/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(geoCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

geo/%.d: geo/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(geoCXXFLAGSEXTRA)" $< > $@

geo/%.d: geo/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(geoCXXFLAGSEXTRA)" $< > $@

geo/%.d: geo/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(geoCXXFLAGSEXTRA),)
# Special rule for dictionaries
geo/G__%.o: geo/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(geoCXXFLAGSEXTRA) -o $@ -c $<

geo/%.o: geo/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(geoCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(geoCFLAGSEXTRA),)
geo/%.o: geo/%.c
	$(CC) $(OPT) $(CFLAGS) $(geoCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(geoF77FLAGSEXTRA),)
geo/%.o: geo/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(geoF77FLAGSEXTRA) -o $@ -c $<
endif

geo-map: $(geoMAP)

ifeq ($(geoNOLIB),)
$(geoMAP): geo/geo.mk geo/geoLinkDef.h
	$(MAKEMAP) -o geo/geo.rootmap -l $(geoMAPLIB) -c geo/geoLinkDef.h
else
$(geoMAP): geo/geo.mk geo/geoLinkDef.h
	@echo "Not making $(geoMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
