#include "TSnIOConfigFrame.h"

//#include "TSnDAQConfig.h"
//#include "TSnRunInfo.h"
//#include "NSnConstants.h"

#include <stdexcept>

ClassImp(TSnIOConfigFrame);

const UChar_t TSnIOConfigFrame::kIOvers = 8; // MUST INCREASE if WriteTo changes
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV1 = 
                  + (9u*sizeof(UInt_t)) + (6u*sizeof(UShort_t))
                  + (10u*sizeof(UChar_t))
                  + (3u*NSnConstants::kMaxPlasV1*sizeof(UShort_t))
                  + (NSnConstants::kTotDacs*sizeof(UShort_t))
                  + (TSnDAQConfig::kMaxLblLen*sizeof(UChar_t));
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV2 =
                  kMaxSizeOfV1 + sizeof(UInt_t) + sizeof(UChar_t);
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV3 =
                  kMaxSizeOfV2 + (2u*sizeof(UShort_t)) 
                  + (4u*TSnNetAdrSet::kIPLen*sizeof(char));
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV4 = // forced per to float
                  kMaxSizeOfV3 + (sizeof(Float_t)-sizeof(UShort_t));
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV5 = kMaxSizeOfV4;
// To/From Low Powr (V6)
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV6 = kMaxSizeOfV5 + sizeof(UShort_t);
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV7 = // add ConnTOmins, ListenTOmins
                  kMaxSizeOfV6 + (2u*sizeof(UChar_t));
const UInt_t  TSnIOConfigFrame::kMaxSizeOfV8 = // FirstEvt->FirstSeq
                  kMaxSizeOfV7 - sizeof(UInt_t) + sizeof(UShort_t);
const UInt_t  TSnIOConfigFrame::kMaxSizeOf = kMaxSizeOfV7;

UInt_t TSnIOConfigFrame::SizeOf(const UChar_t rv,
                                const Bool_t streamHiLoPlas,
                                const UChar_t nplas,
                                const UChar_t lblLen) {
   // private because it cannot be used to read from a buffer
   // (the label length and fStreamHiLoPlas are not known a priori)
   // returns the num of bytes needed to stream this object
   // = size of member vars + 1 for i/o version + extra PLA strings (maybe)
   const UInt_t maxsize = GetMaxSizeOf(rv);
   const Int_t lbldiff = TSnDAQConfig::kMaxLblLen - lblLen;
   UInt_t sz = maxsize - lbldiff;
   if ((lbldiff!=0) && (rv>=4)) {
      // NOTE: there is currently no work around for the serious problem
      // that when the \0 char was added to WriteTo, the IOvers was NOT
      // increased!! Reading the configs in Python presents no problems,
      // which is both fortunate (old configs can be read) and unfortunate
      // (it caused the problem to go unnoticed).
      // A workaround could be perfomed using the fact that the \0 change
      // went in on 2012-12-20 16:04:33 -0800
      //
      // Fortunately, the only "issue" is that SizeOf will return a value
      // that is 1 byte too large for early V4 configs, but this presents
      // no problem with reading old configs, either in C++ or in Python.
      sz += 1; // the \0 at the end of the string
   }
   static const UInt_t mhlp = 2u*NSnConstants::kMaxPlas*sizeof(UShort_t);
   const Int_t dp = (nplas-NSnConstants::kMaxPlas)*sizeof(UShort_t);
   const UChar_t fac = (streamHiLoPlas) ? 3u : 1u;
   sz += (fac*dp);
   if (streamHiLoPlas==kFALSE) {
      sz -= mhlp;
   }
   return sz;
}

UInt_t TSnIOConfigFrame::SizeOf(const UChar_t rv, const TSnDAQConfig& conf) {
   // returns the num of bytes needed to stream this object
   // = size of member vars + 1 for i/o version + extra PLA strings (maybe)
   //   + length of label string
   return SizeOf(rv,
                 conf.IsStreamingHiLoPlas(),
                 conf.GetPlaSet().GetNplas(),
                 conf.GetLabelLen());
}

UChar_t TSnIOConfigFrame::ReadFromW(const TSnIOBuffer& b,
                                    TSnDAQConfig& conf,
                                    TSnRunInfo& runf,
                                    TSnNetAdrSet& nets) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, conf, runf, nets);
}

UChar_t TSnIOConfigFrame::ReadFromW(const Char_t*& b,
                                    TSnDAQConfig& conf,
                                    TSnRunInfo& runf,
                                    TSnNetAdrSet& nets) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, conf, runf, nets);
}

UChar_t TSnIOConfigFrame::ReadFromW(FILE*& b,
                                    TSnDAQConfig& conf,
                                    TSnRunInfo& runf,
                                    TSnNetAdrSet& nets) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, conf, runf, nets);
}

void TSnIOConfigFrame::WriteToW(TSnIOBuffer& b,
                                const TSnDAQConfig& conf,
                                const TSnRunInfo& runf,
                                const TSnNetAdrSet& nets,
                                const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, conf, runf, nets, Rv);
}
   

void TSnIOConfigFrame::WriteToW(Char_t*& b,
                                const TSnDAQConfig& conf,
                                const TSnRunInfo& runf,
                                const TSnNetAdrSet& nets,
                                const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, conf, runf, nets, Rv);
}

void TSnIOConfigFrame::WriteToW(FILE*& b,
                                const TSnDAQConfig& conf,
                                const TSnRunInfo& runf,
                                const TSnNetAdrSet& nets,
                                const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, conf, runf, nets, Rv);
}


