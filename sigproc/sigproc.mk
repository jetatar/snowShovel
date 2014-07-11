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
	TSnFFTCalcMod.h \
	TSnFindWaveformEnvelopeMod.h \
	TSnSmoothDataLstSqrCnvMod.h \
	TSnTreeBuilderMod.h \
	TSnWaveletDenoiseMod.h \
	TSnPruneBadEvtsMod.h \

sigprocLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -lsigprocdat -ldbdat -levdat -lonline -lutil -ltam \

sigprocLIBDEP := \
	sigprocdat dbdat evdat online util tam \

# include files (TParameter) so we can put certain templates in the dictionary
sigprocDHEXTRA := \
	TSnEventMetadata.h \
	TSnEventHeader.h \
	TSnClockSetInfo.h \
	TSnHeartbeat.h \
	TSnPowerReading.h \
	TSnRawWaveform.h \
	TSnDAQConfig.h \
	TSnRunInfo.h \
	TSnNetAdrSet.h \
	TSnCalWvData.h \
	TSnCalFFTData.h \
	TSnStopPosnSet.h \
	TParameter.h \
	TObjArray.h \

