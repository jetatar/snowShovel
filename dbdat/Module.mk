# $Id$
#
# Module.mk -- Makefile for dbdat
# (don't manually edit this file)
#

MODDIR := dbdat

dbdatS  := $(filter-out dbdat/G__%,$(wildcard dbdat/*.cxx))
dbdatF  := $(wildcard dbdat/*.f)

dbdatH  := $(filter-out dbdat/%LinkDef.h,$(wildcard dbdat/*.h*))
dbdatH  := $(filter-out dbdat/G__%.h,$(dbdatH))
dbdatI  := $(wildcard dbdat/*.inc)

dbdatLIB     := lib/libdbdat.$(SOEXT)

dbdatPAR     := par/dbdat.par
dbdatPARINIT := $(shell if test -e "dbdat/dbdat_setup.C"; then echo "dbdat/dbdat_setup.C"; else echo ""; fi)

dbdatMAP     := dbdat/dbdat.rootmap

dbdatOMOD        = $(foreach mm,$(patsubst %,%O,$(dbdatMODULES)),$($(mm)))
dbdatHMOD        = $(foreach mm,$(patsubst %,%H,$(dbdatMODULES)),$($(mm)))
dbdatIMOD        = $(foreach mm,$(patsubst %,%I,$(dbdatMODULES)),$($(mm)))
dbdatLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(dbdatMODULES)),$($(mm)))
dbdatPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(dbdatMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include dbdat/dbdat.mk



# derive source dependent output
dbdatO   := $(dbdatS:.cxx=.o) $(if $(wildcard dbdat/dbdatLinkDef.h),dbdat/G__dbdat.o) $(dbdatF:.f=.o)
dbdatDEP := $(dbdatO:.o=.d)

ifeq ($(dbdatMAPLIB),)
dbdatMAPLIB := $(dbdatLIB)
else
dbdatMAPLIB := lib/$(dbdatMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst dbdat/%.h,include/%.h,$(dbdatH))
ALLHDRS     += $(patsubst dbdat/%.hxx,include/%.hxx,$(dbdatH))
ALLHDRS     += $(patsubst dbdat/%.inc,include/%.inc,$(dbdatI))
ifeq ($(dbdatNOLIB),)
ALLLIBS	    += $(dbdatLIB)
ALLPARS     += $(dbdatPAR)
ALLMAPS     += $(dbdatMAP)
endif

# include all dependency files
INCLUDEFILES += $(dbdatDEP)


#
# Populate central include dir
#
include/%.h:    dbdat/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  dbdat/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  dbdat/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs dbdat-libs pars dbdat-pars clean dbdat-clean


ifeq ($(dbdatNOLIB),)
libs:: dbdat-libs

dbdat-libs: $(dbdatLIB)

pars:: dbdat-pars

dbdat-pars: $(dbdatPAR)
endif

extras:: $(dbdatEXTRAS)

clean:: dbdat-clean

dbdat-clean: $(dbdatLOCALCLEAN)
	rm -f $(dbdatLIB) $(dbdatPAR) $(dbdatO) $(dbdatDEP) dbdat/G__dbdat.h dbdat/G__dbdat.cxx dbdat/dbdat.rootmap

ifeq ($(dbdatNOLIB),)
dbdat-all: dbdat-libs dbdat-pars $(dbdatEXTRAS)
else
dbdat-all: $(dbdatEXTRAS)
endif

dbdat:: dbdat-all

ifneq ($(dbdatF),)
dbdatLIBEXTRA += $(F77LIBS)
endif

ifeq ($(dbdatNOLIB),)
$(dbdatLIB): $(dbdatLIBDEP) $(dbdatO) $(dbdatOMOD) dbdat/Module.mk dbdat/dbdat.mk
	@echo "Making libdbdat.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libdbdat.$(SOEXT) $@ "$(dbdatO) $(dbdatOMOD)" \
	"$(dbdatLIBEXTRA) $(dbdatLIBEXTRAMOD)"


$(dbdatPAR): $(dbdatLIB) $(dbdatH) $(dbdatI) $(dbdatPAREXTRA) $(dbdatPAREXTRAMOD) $(dbdatPARINIT)
	@if test -e "$(dbdatPARINIT)"; then \
	        echo "Making $(dbdatPAR) using $(dbdatPARINIT)"; \
         else echo "Making $(dbdatPAR)" ; fi
	@$(MAKEPAR) $(dbdatPAR) $(dbdatLIB) "$(dbdatH) $(dbdatI) $(dbdatHMOD) $(dbdatIMOD)" \
	"$(dbdatPAREXTRA) $(dbdatPAREXTRAMOD)" dbdat/dbdat_setup.C
else
$(dbdatLIB): $(dbdatLIBDEP) $(dbdatO) $(dbdatOMOD) dbdat/Module.mk dbdat/dbdat.mk
	@echo "Not making libdbdat.$(SOEXT)"

$(dbdatPAR): $(dbdatLIB) $(dbdatH) $(dbdatI) $(dbdatPAREXTRA) $(dbdatPAREXTRAMOD) $(dbdatPARINIT)
	@echo "Not making $(dbdatPAR)"
endif

#
# Dictionary rule
#

dbdatDH := $(patsubst %,dbdat/%,$(dbdatDH))

dbdat/G__dbdat.h dbdat/G__dbdat.cxx: config.mk $(ROOTCINT) dbdat/dbdatLinkDef.h $(dbdatDH)
	        $(ROOTCINT) -f dbdat/G__dbdat.cxx -c $(dbdatCF) $(CPPFLAGS) $(dbdatDH) $(dbdatDHEXTRA) \
		dbdat/dbdatLinkDef.h


#
# Dependency generation rules
#

dbdat/G__%.d: dbdat/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(dbdatCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

dbdat/%.d: dbdat/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(dbdatCXXFLAGSEXTRA)" $< > $@

dbdat/%.d: dbdat/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(dbdatCXXFLAGSEXTRA)" $< > $@

dbdat/%.d: dbdat/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(dbdatCXXFLAGSEXTRA),)
# Special rule for dictionaries
dbdat/G__%.o: dbdat/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(dbdatCXXFLAGSEXTRA) -o $@ -c $<

dbdat/%.o: dbdat/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(dbdatCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(dbdatCFLAGSEXTRA),)
dbdat/%.o: dbdat/%.c
	$(CC) $(OPT) $(CFLAGS) $(dbdatCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(dbdatF77FLAGSEXTRA),)
dbdat/%.o: dbdat/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(dbdatF77FLAGSEXTRA) -o $@ -c $<
endif

dbdat-map: $(dbdatMAP)

ifeq ($(dbdatNOLIB),)
$(dbdatMAP): dbdat/dbdat.mk dbdat/dbdatLinkDef.h
	$(MAKEMAP) -o dbdat/dbdat.rootmap -l $(dbdatMAPLIB) -c dbdat/dbdatLinkDef.h
else
$(dbdatMAP): dbdat/dbdat.mk dbdat/dbdatLinkDef.h
	@echo "Not making $(dbdatMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
