#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

recoDH := \
	NSnChanCorl.h \
	TSnMinimizer.h \
	TSnFittingModule.h \
	TSnChanTimeCorlMod.h \
	TSnFitChanOffsetMod.h \
	TSnChanTimeCorlAltEvtsMod.h \
	TSnFitChanOffsetAltEvtsMod.h \
	TSnAngleFromChanCorlMod.h \
	TSnAngleFromSingleChanOffsets.h \
	TSnSaveRecoResultsMod.h \
	TSnSaveCorlDataMod.h \

recoLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -lrecodat -lsigprocdat -lsigproc \
	-ldbdat -levdat -lutil -ltam -lgeo \

recoLIBDEP := \
	recodat sigprocdat sigproc dbdat evdat util tam geo \

