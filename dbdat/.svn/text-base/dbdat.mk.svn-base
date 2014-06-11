#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

dbdatDH := \
	TSnComponent.h \
	TSnCompAfar.h \
	TSnCompAmp.h \
	TSnCompAntenna.h \
	TSnCompFpga.h \
	TSnCompDigitizer.h \
	TSnCompDaughtercard.h \
	TSnCompIridium.h \
	TSnCompMbed.h \
	TSnCompMotherboard.h \
	TSnDacSet.h \
	TSnPlaSet.h \
	TSnAmpOnSet.h \
	TSnDAQConfig.h \
	TSnRunInfo.h \
	TSnNetAdrSet.h \
	TSnCommWinSet.h \
	TSnTriggerSetup.h \
	TSnSignalStrengthInfo.h \

dbdatLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -lutil \

dbdatLIBDEP := \
	util \

# include TParameter so we can make the other parameter types
dbdatDHEXTRA := \
	TParameter.h \

