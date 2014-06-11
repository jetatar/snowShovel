#include "TSnIOSignalStrengthFrame.h"

ClassImp(TSnIOSignalStrengthFrame);

const UChar_t TSnIOSignalStrengthFrame::kIOvers    = 1; // MUST INCREASE if WriteTo changes
const UInt_t  TSnIOSignalStrengthFrame::kMaxSizeOf =
   2u*sizeof(UChar_t) + sizeof(Float_t) + sizeof(UInt_t);

UChar_t TSnIOSignalStrengthFrame::ReadFromW(const TSnIOBuffer& b,
                                            TSnSignalStrengthInfo& p) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, p);
}

UChar_t TSnIOSignalStrengthFrame::ReadFromW(const Char_t*& b,
                                            TSnSignalStrengthInfo& p) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, p);
}

UChar_t TSnIOSignalStrengthFrame::ReadFromW(FILE*& b,
                                            TSnSignalStrengthInfo& p) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, p);
}

void TSnIOSignalStrengthFrame::WriteToW(TSnIOBuffer& b,
                                        const TSnSignalStrengthInfo& p,
                                        const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, p, Rv);
}

void TSnIOSignalStrengthFrame::WriteToW(Char_t*& b,
                                        const TSnSignalStrengthInfo& p,
                                        const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, p, Rv);
}

void TSnIOSignalStrengthFrame::WriteToW(FILE*& b,
                                        const TSnSignalStrengthInfo& p,
                                        const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, p, Rv);
}


