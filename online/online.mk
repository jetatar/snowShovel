#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

onlineDH := \
	TSnIOBuffer.h \
	TSnIOObject.h \
	TSnIOConfigFrame.h \
	TSnIOEventFrame.h \
	TSnIOStatusFrame.h \
	TSnIOHeaderFrame.h \
	TSnIOHeartbeatFrame.h \
	TSnIOPowerFrame.h \
	TSnIOClockSetFrame.h \
	TSnIOSignalStrengthFrame.h \
	TSnIODataFile.h \
	TSnCommObject.h \
	TSnCommQueue.h \
	TSnCommConfPack.h \
	TSnCommMbedLocalFile.h \
	TSnCommStatusPack.h \
	TSnCommAlertPack.h \
	TSnCommIncoming.h \
	TSnCommIncFile.h \
	TSnCommIncStatus.h \
	TSnCommIncHandshake.h \
	TSnCommProtocolMsg.h \
	TSnCommDeleteDataMsg.h \
	TSnCommActor.h \
	TSnCommAction.h \
	TSnCommStatusAction.h \
	TSnCommFileAction.h \

onlineLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -ldbdat -levdat \

onlineLIBDEP := \
	dbdat evdat \
