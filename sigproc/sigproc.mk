#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

sigprocDH := \
	TSnRawTreeMaker.h \
	TSnBasicEvtSelMod.h \
	TSnRejectBadClocksMod.h \
	TSnRejectEarlyEvtsMod.h \
	TSnFPNCalcMod.h \
	TSnFPNSubMod.h \
	TSnDigitizerCalibMod.h \
	TSnSaveCalibDataMod.h \
	TSnCalcAbsTimeMod.h \
	TSnFindTimeLimitsMod.h \
	TSnFindEvtNumLimitsMod.h \
	TSnPlotEvtRateMod.h \
	TSnPlotWaveformsMod.h \
	TSnEvtGuiCanvFrame.h \
	TSnEvtGuiMainFrame.h \
	TSnEvtGuiSeleFrame.h \
	TSnGCanvasSaveDialog.h \
	TSnEvtGuiDataGuru.h \
	TSnRejectBadVoltTimesMod.h \
	TSnPlotPowerReadingsMod.h \
	TSnPlotAdcDists.h \
	TSnCalDatModule.h \
	TSnFilterWaveformMod.h \
	TSnStopFindRms.h \
	TSnShiftCalWvDataMod.h \
	TSnMakeShiftTree.h \
	TSnPruneBadEvtsMod.h \
	TSnFFTCalcMod.h \
	TSnFindWaveformEnvelopeMod.h \

sigprocLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -lsigprocdat -ldbdat -levdat -lonline -lutil -ltam \

sigprocLIBDEP := \
	sigprocdat dbdat evdat online util tam \

# include TParameter so we can make the other parameter types
sigprocDHEXTRA := \
	TParameter.h \

