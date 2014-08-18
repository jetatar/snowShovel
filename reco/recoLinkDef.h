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

#pragma link C++ class TSnMinimizer+;
#pragma link C++ class TSnFittingModule+;
#pragma link C++ class TSnChanTimeCorlMod+;
#pragma link C++ class TSnFitChanOffsetMod+;
#pragma link C++ class TSnChanTimeCorlAltEvtsMod+;
#pragma link C++ class TSnFitChanOffsetAltEvtsMod+;
#pragma link C++ class TSnAngleFromChanCorlMod+;
#pragma link C++ class TSnAngleFromSingleChanOffsets+;
#pragma link C++ class TSnSaveRecoResultsMod+;
#pragma link C++ class TSnSaveRecoResultsMod::TSnRecoResultWrapper+;
#pragma link C++ class TSnSaveCorlDataMod+;
#pragma link C++ class TSnFindPureThermalFromAutoCorrMod+;

#ifdef USE_FOR_AUTLOADING // add the namespace to the rootmap
#pragma link C++ class NSnChanCorl+;
#else
#pragma link C++ namespace NSnChanCorl+;
#endif

#endif // __CINT__
