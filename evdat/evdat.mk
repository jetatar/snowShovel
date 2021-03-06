#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

evdatDH := \
	TSnCompressor.h \
	TSnEvent.h \
	TSnEventHeader.h \
	TSnEventMetadata.h \
	TSnRawWaveform.h \
	TSnStatusUpdate.h \
	TSnTrgInfo.h \
	NSnCRCUtils.h \
	NSnConstants.h \
	TSnPowerReading.h \
	TSnHeartbeat.h \
	TSnClockSetInfo.h \
	NSnHashUtils.h \
	TSnWindData.h \

evdatLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -lutil \

evdatLIBDEP := \
	util \
