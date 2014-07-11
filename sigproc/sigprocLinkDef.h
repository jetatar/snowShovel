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
#pragma link C++ class TSnFFTCalcMod+;
#pragma link C++ class TSnFindWaveformEnvelopeMod+;
#pragma link C++ class TSnSmoothDataLstSqrCnvMod+;
#pragma link C++ class TSnTreeBuilderMod+;
#pragma link C++ class TSnWaveletDenoiseMod+;
#pragma link C++ class TSnPruneBadEvtsMod+;

// define some common branches so that these will work in CINT or pyroot
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnEventMetadata>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnEventHeader>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnClockSetInfo>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnHeartbeat>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnPowerReading>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnRawWaveform>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnDAQConfig>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnRunInfo>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnNetAdrSet>;
#pragma link C++ function TSnTreeBuilderMod::CopyBranchFromTree<TSnCalWvData>;
#pragma link C++ function TSnTreeBuilderMod::AddBranchForEvtObj<TSnCalWvData>;
#pragma link C++ function TSnTreeBuilderMod::AddBranchForEvtObj<TSnCalFFTData>;
#pragma link C++ function TSnTreeBuilderMod::AddBranchForEvtObj<TSnStopPosnSet>;
#pragma link C++ function TSnTreeBuilderMod::AddBranchForEvtObj< TParameter<Double_t> >;
#pragma link C++ function TSnTreeBuilderMod::AddBranchForEvtObj<TObjArray>;

// define the one we need, that ROOT didn't define
#pragma link C++ class TParameter<UInt_t>+;


#endif // __CINT__
