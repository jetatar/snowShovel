#include "TSnIOEventFrame.h"

ClassImp(TSnIOEventFrame);

const UChar_t TSnIOEventFrame::kIOvers = 1; // MUST INCREMENT if WriteTo changes
const UInt_t  TSnIOEventFrame::kMaxSizeOf = 
        ((sizeof(UInt_t)*4u)+sizeof(Int_t)+sizeof(UShort_t)
        +(NSnConstants::kTotSamps*sizeof(UShort_t))+sizeof(UChar_t));


UInt_t TSnIOEventFrame::SizeOf(const UChar_t rv,
                               const UChar_t loseLSB,
                               const UChar_t loseMSB) {
   // size of member vars + size of packed waveform + 1 for i/o version
   if (rv>0) {
      if ((loseLSB==0) && (loseMSB==0)) {
         return kMaxSizeOf;
      } else {
         static const UInt_t ss = 
            kMaxSizeOf-(NSnConstants::kTotSamps*sizeof(UShort_t));
         const UInt_t sz = ss + 
            TSnCompressor::SizeOfPackedData<UShort_t>(loseLSB,
                                                      loseMSB,
                                                      NSnConstants::kTotSamps);
         return sz;
      }
   } else {
      throw std::runtime_error(Form(
         "<TSnIOEventFrame::SizeOf>: Unknown read version [%hhu]",rv));
   }
   return 0;
}

   // rootcint can't parse the templated functions, so wrap them
   
UChar_t TSnIOEventFrame::ReadFromW(const Char_t*& b, TSnEvent& evt, 
                                   const UChar_t loseLSB, 
                                   const UChar_t loseMSB,
                                   const UShort_t wvBaseline) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, evt, loseLSB, loseMSB, wvBaseline);
}
   
UChar_t TSnIOEventFrame::ReadFromW(const TSnIOBuffer& b, TSnEvent& evt, 
                                   const UChar_t loseLSB,
                                   const UChar_t loseMSB,
                                   const UShort_t wvBaseline) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, evt, loseLSB, loseMSB, wvBaseline);
}

void TSnIOEventFrame::WriteToW(Char_t*& b, const TSnEvent& evt, 
                               const UChar_t loseLSB, const UChar_t loseMSB,
                               const UShort_t wvBaseline,
                               const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, evt, loseLSB, loseMSB, wvBaseline, Rv);
}

   
void TSnIOEventFrame::WriteToW(TSnIOBuffer& b, const TSnEvent& evt, 
                               const UChar_t loseLSB, const UChar_t loseMSB,
                               const UShort_t wvBaseline,
                               const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, evt, loseLSB, loseMSB, wvBaseline, Rv);
}

