#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

commactsDH := \
	TSnCommActHiRate.h \
	TSnCommActConfResend.h \
	TSnCommActSendQueued.h \
	TSnCommActSendNoConf.h \
	TSnCommActSendPeriodic.h \
	TSnCommFiActTransComplete.h \
	TSnCommFiActStopTrans.h \
	TSnCommFiActDeleteRun.h \
	TSnCommFiActDeleteAllData.h \
	TSnCommFiActRequestRun.h \
	TSnCommFiActNoReplyHndshk.h \
	TSnCommFiActSendQueued.h \
	TSnCAGuiActorFrame.h \
	TSnCAGuiActsList.h \
	TSnCAGuiActsAvailList.h \

commactsLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -ldbdat -levdat -lonline -ldb \

commactsLIBDEP := \
	dbdat evdat online db \

