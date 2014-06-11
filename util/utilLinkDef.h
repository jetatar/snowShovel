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

#pragma link C++ class TSnAxisDetails+;
#pragma link C++ class TSnAxisDetails::TAxDet+;
#pragma link C++ class TSnPlotBrowser+;
#pragma link C++ class TSnMath+;
#pragma link C++ class TSnSpectral+;

#pragma link C++ enum  TSnPlotBrowser::EPadOpts;
#pragma link C++ enum  TSnSpectral::EWinType_t;

#pragma link C++ function TSnMath::GetAveVarFast<Float_t>;
#pragma link C++ function TSnMath::GetAveVarFast<Double_t>;
#pragma link C++ function TSnMath::GetAveRMSFast<Float_t>;
#pragma link C++ function TSnMath::GetAveRMSFast<Double_t>;
#pragma link C++ function TSnMath::GetAveVarTwoPass<Float_t>;
#pragma link C++ function TSnMath::GetAveVarTwoPass<Double_t>;
#pragma link C++ function TSnMath::GetAveRMSTwoPass<Float_t>;
#pragma link C++ function TSnMath::GetAveRMSTwoPass<Double_t>;
#pragma link C++ function TSnMath::GetWeightedAveVarFast<Float_t>;
#pragma link C++ function TSnMath::GetWeightedAveVarFast<Double_t>;
#pragma link C++ function TSnMath::GetWeightedAveRMSFast<Float_t>;
#pragma link C++ function TSnMath::GetWeightedAveRMSFast<Double_t>;
#pragma link C++ function TSnMath::GetWeightedAveVarTwoPass<Float_t>;
#pragma link C++ function TSnMath::GetWeightedAveVarTwoPass<Double_t>;
#pragma link C++ function TSnMath::GetWeightedAveRMSTwoPass<Float_t>;
#pragma link C++ function TSnMath::GetWeightedAveRMSTwoPass<Double_t>;
#pragma link C++ function TSnMath::GetRange<Float_t>;
#pragma link C++ function TSnMath::GetRange<Double_t>;
#pragma link C++ function TSnMath::GetAngleDiff<Float_t>;
#pragma link C++ function TSnMath::GetAngleDiff<Double_t>;
#pragma link C++ function TSnMath::GetAngleDiffDeg<Float_t>;
#pragma link C++ function TSnMath::GetAngleDiffDeg<Double_t>;
#pragma link C++ function TSnMath::SearchOneToOneMapping<Float_t>;
#pragma link C++ function TSnMath::SearchOneToOneMapping<Double_t>;
#pragma link C++ function TSnMath::GetBitsAsString<Char_t>;
#pragma link C++ function TSnMath::GetBitsAsString<UChar_t>;
#pragma link C++ function TSnMath::GetBitsAsString<Short_t>;
#pragma link C++ function TSnMath::GetBitsAsString<UShort_t>;
#pragma link C++ function TSnMath::GetBitsAsString<Int_t>;
#pragma link C++ function TSnMath::GetBitsAsString<UInt_t>;
#pragma link C++ function TSnMath::GetBitsAsString<Long64_t>;
#pragma link C++ function TSnMath::GetBitsAsString<ULong64_t>;
#pragma link C++ function TSnMath::PrintBits<Char_t>;
#pragma link C++ function TSnMath::PrintBits<UChar_t>;
#pragma link C++ function TSnMath::PrintBits<Short_t>;
#pragma link C++ function TSnMath::PrintBits<UShort_t>;
#pragma link C++ function TSnMath::PrintBits<Int_t>;
#pragma link C++ function TSnMath::PrintBits<UInt_t>;
#pragma link C++ function TSnMath::PrintBits<Long64_t>;
#pragma link C++ function TSnMath::PrintBits<ULong64_t>;


#pragma link C++ function TSnSpectral::SquareWin<Float_t>;
#pragma link C++ function TSnSpectral::SquareWin<Double_t>;
#pragma link C++ function TSnSpectral::WelchWin<Float_t>;
#pragma link C++ function TSnSpectral::WelchWin<Double_t>;
#pragma link C++ function TSnSpectral::BartlettWin<Float_t>;
#pragma link C++ function TSnSpectral::BartlettWin<Double_t>;
#pragma link C++ function TSnSpectral::DiscreteCpxFFT<Float_t>;
#pragma link C++ function TSnSpectral::DiscreteCpxFFT<Double_t>;
#pragma link C++ function TSnSpectral::RealFFT<Float_t>;
#pragma link C++ function TSnSpectral::RealFFT<Double_t>;
#pragma link C++ function TSnSpectral::RealFFTSimul<Float_t>;
#pragma link C++ function TSnSpectral::RealFFTSimul<Double_t>;
#pragma link C++ function TSnSpectral::Correlate<Float_t>;
#pragma link C++ function TSnSpectral::Correlate<Double_t>;
#pragma link C++ function TSnSpectral::NewCorrelateGraph<Float_t>;
#pragma link C++ function TSnSpectral::NewCorrelateGraph<Double_t>;
#pragma link C++ function TSnSpectral::Spectrum<TBranch, Float_t>;
#pragma link C++ function TSnSpectral::Spectrum<TBranch, Double_t>;
#pragma link C++ function TSnSpectral::Spectrum<TH1F, Float_t>;
#pragma link C++ function TSnSpectral::Spectrum<TH1D, Double_t>;
#pragma link C++ function TSnSpectral::SlowLomb<Float_t>;
#pragma link C++ function TSnSpectral::SlowLomb<Double_t>;
#pragma link C++ function TSnSpectral::FastLomb<Float_t>;
#pragma link C++ function TSnSpectral::FastLomb<Double_t>;
#pragma link C++ function TSnSpectral::HilbertCpxAnalyticSignal<Float_t>;
#pragma link C++ function TSnSpectral::HilbertCpxAnalyticSignal<Double_t>;
#pragma link C++ function TSnSpectral::EnvelopeReal<Float_t>;
#pragma link C++ function TSnSpectral::EnvelopeReal<Double_t>;
#pragma link C++ function TSnSpectral::Spread<Float_t>;
#pragma link C++ function TSnSpectral::Spread<Double_t>;
#pragma link C++ function TSnSpectral::NewGraphFromRealFFT<Float_t>;
#pragma link C++ function TSnSpectral::NewGraphFromRealFFT<Double_t>;
#pragma link C++ function TSnSpectral::PearsonSmpCorrCoef<Float_t>;
#pragma link C++ function TSnSpectral::PearsonSmpCorrCoef<Double_t>;
#pragma link C++ function TSnSpectral::NewScanPrsCorrCoefGraph<Float_t>;
#pragma link C++ function TSnSpectral::NewScanPrsCorrCoefGraph<Double_t>;
#pragma link C++ function TSnSpectral::NewScanPrsCorrCoefGraphUneq<Float_t>;
#pragma link C++ function TSnSpectral::NewScanPrsCorrCoefGraphUneq<Double_t>;
#endif // __CINT__
