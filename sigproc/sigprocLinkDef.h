//
// $Id$
//
//

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

//#pragma link C++ function myFunc(TMyArg);
//#pragma link C++ global gMGlobal;
//#pragma link C++ typedef MyTypedef_t;

#pragma link C++ class TSnRawTreeMaker+;
#pragma link C++ class TSnBasicEvtSelMod+;
#pragma link C++ class TSnRejectBadClocksMod+;
#pragma link C++ class TSnRejectEarlyEvtsMod+;
#pragma link C++ class TSnFPNCalcMod+;
#pragma link C++ class TSnFPNSubMod+;
#pragma link C++ class TSnDigitizerCalibMod+;
#pragma link C++ class TSnSaveCalibDataMod+;
#pragma link C++ class TSnCalcAbsTimeMod+;
#pragma link C++ class TSnFindTimeLimitsMod+;
#pragma link C++ class TSnFindEvtNumLimitsMod+;
#pragma link C++ class TSnPlotEvtRateMod+;
#pragma link C++ class TSnPlotWaveformsMod+;
#pragma link C++ class TSnEvtGuiMainFrame+;
#pragma link C++ class TSnEvtGuiCanvFrame+;
#pragma link C++ class TSnEvtGuiSeleFrame+;
#pragma link C++ class TSnGCanvasSaveDialog+;
#pragma link C++ class TSnEvtGuiDataGuru+;
#pragma link C++ class TSnRejectBadVoltTimesMod+;
#pragma link C++ class TSnPlotPowerReadingsMod+;
#pragma link C++ class TSnPlotAdcDists+;
#pragma link C++ class TSnCalDatModule+;
#pragma link C++ class TSnCalDatModule::AdrWrap_t+;
#pragma link C++ class TSnFilterWaveformMod+;
#pragma link C++ class TSnStopFindRms+;
#pragma link C++ class TSnShiftCalWvDataMod+;
#pragma link C++ class TSnMakeShiftTree+;
#pragma link C++ class TSnPruneBadEvtsMod+;
#pragma link C++ class TSnFFTCalcMod+;
#pragma link C++ class TSnFindWaveformEnvelopeMod+;

// define the one we need, that ROOT didn't define
#pragma link C++ class TParameter<UInt_t>+;


#endif // __CINT__