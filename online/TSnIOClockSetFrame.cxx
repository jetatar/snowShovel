#include "TSnIOClockSetFrame.h"

ClassImp(TSnIOClockSetFrame);

const UChar_t TSnIOClockSetFrame::kIOvers    = 1; // MUST INCREASE if WriteTo changes
const UInt_t  TSnIOClockSetFrame::kMaxSizeOf =
   sizeof(UChar_t) + (3u*sizeof(UInt_t)) + sizeof(Int_t);

UChar_t TSnIOClockSetFrame::ReadFromW(const TSnIOBuffer& b,
                                      TSnClockSetInfo& p) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, p);
}

UChar_t TSnIOClockSetFrame::ReadFromW(const Char_t*& b, TSnClockSetInfo& p) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, p);
}

UChar_t TSnIOClockSetFrame::ReadFromW(FILE*& b, TSnClockSetInfo& p) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, p);
}

void TSnIOClockSetFrame::WriteToW(TSnIOBuffer& b, const TSnClockSetInfo& p,
                                  const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, p, Rv);
}

void TSnIOClockSetFrame::WriteToW(Char_t*& b, const TSnClockSetInfo& p,
                                  const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, p, Rv);
}

void TSnIOClockSetFrame::WriteToW(FILE*& b, const TSnClockSetInfo& p,
                                  const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, p, Rv);
}


