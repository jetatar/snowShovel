# $Id$
#
# Module.mk -- Makefile for db
# (don't manually edit this file)
#

MODDIR := db

dbS  := $(filter-out db/G__%,$(wildcard db/*.cxx))
dbF  := $(wildcard db/*.f)

dbH  := $(filter-out db/%LinkDef.h,$(wildcard db/*.h*))
dbH  := $(filter-out db/G__%.h,$(dbH))
dbI  := $(wildcard db/*.inc)

dbLIB     := lib/libdb.$(SOEXT)

dbPAR     := par/db.par
dbPARINIT := $(shell if test -e "db/db_setup.C"; then echo "db/db_setup.C"; else echo ""; fi)

dbMAP     := db/db.rootmap

dbOMOD        = $(foreach mm,$(patsubst %,%O,$(dbMODULES)),$($(mm)))
dbHMOD        = $(foreach mm,$(patsubst %,%H,$(dbMODULES)),$($(mm)))
dbIMOD        = $(foreach mm,$(patsubst %,%I,$(dbMODULES)),$($(mm)))
dbLIBEXTRAMOD = $(foreach mm,$(patsubst %,%LIBEXTRA,$(dbMODULES)),$($(mm)))
dbPAREXTRAMOD = $(foreach mm,$(patsubst %,%PAREXTRA,$(dbMODULES)),$($(mm)))


#
# include module-dependent definitions
#
include db/db.mk



# derive source dependent output
dbO   := $(dbS:.cxx=.o) $(if $(wildcard db/dbLinkDef.h),db/G__db.o) $(dbF:.f=.o)
dbDEP := $(dbO:.o=.d)

ifeq ($(dbMAPLIB),)
dbMAPLIB := $(dbLIB)
else
dbMAPLIB := lib/$(dbMAPLIB)
endif

# used in the main Makefile
ALLHDRS     += $(patsubst db/%.h,include/%.h,$(dbH))
ALLHDRS     += $(patsubst db/%.hxx,include/%.hxx,$(dbH))
ALLHDRS     += $(patsubst db/%.inc,include/%.inc,$(dbI))
ifeq ($(dbNOLIB),)
ALLLIBS	    += $(dbLIB)
ALLPARS     += $(dbPAR)
ALLMAPS     += $(dbMAP)
endif

# include all dependency files
INCLUDEFILES += $(dbDEP)


#
# Populate central include dir
#
include/%.h:    db/%.h
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.hxx:  db/%.hxx
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@

include/%.inc:  db/%.inc
		$(INCCOPYCMD) $($(TOPVAR))/$< $($(TOPVAR))/$@


#
# Targets that do not correspond to a file
#

.PHONY: libs db-libs pars db-pars clean db-clean


ifeq ($(dbNOLIB),)
libs:: db-libs

db-libs: $(dbLIB)

pars:: db-pars

db-pars: $(dbPAR)
endif

extras:: $(dbEXTRAS)

clean:: db-clean

db-clean: $(dbLOCALCLEAN)
	rm -f $(dbLIB) $(dbPAR) $(dbO) $(dbDEP) db/G__db.h db/G__db.cxx db/db.rootmap

ifeq ($(dbNOLIB),)
db-all: db-libs db-pars $(dbEXTRAS)
else
db-all: $(dbEXTRAS)
endif

db:: db-all

ifneq ($(dbF),)
dbLIBEXTRA += $(F77LIBS)
endif

ifeq ($(dbNOLIB),)
$(dbLIB): $(dbLIBDEP) $(dbO) $(dbOMOD) db/Module.mk db/db.mk
	@echo "Making libdb.$(SOEXT)"
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	"$(SOFLAGS)" libdb.$(SOEXT) $@ "$(dbO) $(dbOMOD)" \
	"$(dbLIBEXTRA) $(dbLIBEXTRAMOD)"


$(dbPAR): $(dbLIB) $(dbH) $(dbI) $(dbPAREXTRA) $(dbPAREXTRAMOD) $(dbPARINIT)
	@if test -e "$(dbPARINIT)"; then \
	        echo "Making $(dbPAR) using $(dbPARINIT)"; \
         else echo "Making $(dbPAR)" ; fi
	@$(MAKEPAR) $(dbPAR) $(dbLIB) "$(dbH) $(dbI) $(dbHMOD) $(dbIMOD)" \
	"$(dbPAREXTRA) $(dbPAREXTRAMOD)" db/db_setup.C
else
$(dbLIB): $(dbLIBDEP) $(dbO) $(dbOMOD) db/Module.mk db/db.mk
	@echo "Not making libdb.$(SOEXT)"

$(dbPAR): $(dbLIB) $(dbH) $(dbI) $(dbPAREXTRA) $(dbPAREXTRAMOD) $(dbPARINIT)
	@echo "Not making $(dbPAR)"
endif

#
# Dictionary rule
#

dbDH := $(patsubst %,db/%,$(dbDH))

db/G__db.h db/G__db.cxx: config.mk $(ROOTCINT) db/dbLinkDef.h $(dbDH)
	        $(ROOTCINT) -f db/G__db.cxx -c $(dbCF) $(CPPFLAGS) $(dbDH) $(dbDHEXTRA) \
		db/dbLinkDef.h


#
# Dependency generation rules
#

db/G__%.d: db/G__%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(dbCXXFLAGSEXTRA) -I$(CINTDIR)/lib/prec_stl -I$(CINTDIR)/stl" $< > $@

db/%.d: db/%.cxx
	$(MAKEDEP) $@ "$(CXXFLAGS) $(dbCXXFLAGSEXTRA)" $< > $@

db/%.d: db/%.c
	$(MAKEDEP) $@ "$(CFLAGS) $(dbCXXFLAGSEXTRA)" $< > $@

db/%.d: db/%.f
	$(F77DEP) $< > $@


#
# Local compilation rule, automatically enable when needed
#

ifneq ($(dbCXXFLAGSEXTRA),)
# Special rule for dictionaries
db/G__%.o: db/G__%.cxx
	$(CXX) $(NOOPT) -I. $(CXXFLAGS) $(dbCXXFLAGSEXTRA) -o $@ -c $<

db/%.o: db/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(dbCXXFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(dbCFLAGSEXTRA),)
db/%.o: db/%.c
	$(CC) $(OPT) $(CFLAGS) $(dbCFLAGSEXTRA) -o $@ -c $<
endif

ifneq ($(dbF77FLAGSEXTRA),)
db/%.o: db/%.f
	$(F77) $(F77OPT) $(F77FLAGS) $(dbF77FLAGSEXTRA) -o $@ -c $<
endif

db-map: $(dbMAP)

ifeq ($(dbNOLIB),)
$(dbMAP): db/db.mk db/dbLinkDef.h
	$(MAKEMAP) -o db/db.rootmap -l $(dbMAPLIB) -c db/dbLinkDef.h
else
$(dbMAP): db/db.mk db/dbLinkDef.h
	@echo "Not making $(dbMAP)"
endif

#
# Local Variables:
# mode:Makefile
# End:
#
# vim: set filetype=Makefile 
#
