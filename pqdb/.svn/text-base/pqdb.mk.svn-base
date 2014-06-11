#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 


# rootcint cannot parse the pqxx files, so we can't build a dict for
# TSnPQSimpleKey<> or the transactors
pqdbDH := \
	TSnPQConnection.h \
	TSnPQDbKey.h \
	NSnPQSimpleKey.h \
	TSnPQIntKey.h \
	TSnPQShortKey.h \
	TSnPQStringKey.h \
	TSnPQComponentKey.h \
	TSnPQCompAfarKey.h \
	TSnPQCompAmpKey.h \
	TSnPQCompAntennaKey.h \
	TSnPQCompFpgaKey.h \
	TSnPQCompDigitizerKey.h \
	TSnPQCompDaughtercardKey.h \
	TSnPQCompIridiumKey.h \
	TSnPQCompMbedKey.h \
	TSnPQCompMotherboardKey.h \
	TSnPQDacSetKey.h \
	TSnPQPlaSetKey.h \
	TSnPQDAQConfigKey.h \
	TSnPQTrigSetupKey.h \
	TSnPQAmpOnSetKey.h \
	TSnPQCommWinSetKey.h \

# include TParameter so we can make the other parameter types
pqdbDHEXTRA := \
	TParameter.h \


pqdbLIBEXTRA := \
	`root-config --glibs` \
	-L$(PQXXLIB) -lpqxx \
	-L$(SNS)/lib -ldb -ldbdat \

# make pqxx includes available during C++ compiling
pqdbCXXFLAGSEXTRA := \
	-I$(PQXXINC) \

# make pqxx includes available during rootcint
pqdbCF := \
	-I$(PQXXINC) \

pqdbLIBDEP := \
	db dbdat \
