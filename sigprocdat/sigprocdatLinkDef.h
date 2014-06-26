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

#pragma link C++ class TSnCalibObject+;
#pragma link C++ class TSnCalibSet+;
#pragma link C++ class TSnPerSampCalibSet+;
#pragma link C++ class TSnPerCardCalibSet+;
#pragma link C++ class TSnFPNCalibVal+;
#pragma link C++ class TSnFPNCalibSet+;
#pragma link C++ class TSnGainCalibVal+;
#pragma link C++ class TSnDualGainCalib+;
#pragma link C++ class TSnDualGainCalibSet+;
#pragma link C++ class TSnCalWvData+;
#pragma link C++ class TSnInterp1DWvData+;
#pragma link C++ class TSnInterp1DSpline3+;
#pragma link C++ class TSnInterp1DLstSqConvo+;
#pragma link C++ class TSnStopPosnSet+;
// TSnCalFFTData has a cache variable that we need to zero out upon a read
// even tho it's not streamed, so use a read directive.
#pragma link C++ class TSnCalFFTData+;
#pragma read sourceClass="TSnCalFFTData" targetClass="TSnCalFFTData" source="" version="[1-]" target="" code="{ newObj->ClearCache(); }" 
#pragma link C++ class TSnWavelet+;
#pragma link C++ class TSnWaveletHaar+;
#pragma link C++ class TSnWaveletDaub4+;
#pragma link C++ class TSnWaveletDaub12+;
#pragma link C++ class TSnWaveletDaub20+;



#endif // __CINT__
