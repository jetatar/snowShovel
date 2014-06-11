#include "TSnIOHeartbeatFrame.h"

ClassImp(TSnIOHeartbeatFrame);

const UChar_t TSnIOHeartbeatFrame::kIOvers = 2;
const UInt_t  TSnIOHeartbeatFrame::kMaxSizeOf = 2u*sizeof(UInt_t)
                                                 + sizeof(UChar_t);


UChar_t TSnIOHeartbeatFrame::ReadFromW(const TSnIOBuffer& b,
                                       TTimeStamp& time, UInt_t& num) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, time, num);
}
   
UChar_t TSnIOHeartbeatFrame::ReadFromW(const Char_t*& b,
                                       TTimeStamp& time, UInt_t& num) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, time, num);
}
   
UChar_t TSnIOHeartbeatFrame::ReadFromW(FILE*& b,
                                       TTimeStamp& time, UInt_t& num) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, time, num);
}
   
void TSnIOHeartbeatFrame::WriteToW(TSnIOBuffer& b,
                                   const TTimeStamp& time, const UInt_t num,
                                   const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, time, num, Rv);
}

void TSnIOHeartbeatFrame::WriteToW(Char_t*& b,
                                   const TTimeStamp& time, const UInt_t num,
                                   const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, time, num, Rv);
} 
  
void TSnIOHeartbeatFrame::WriteToW(FILE*& b,
                                   const TTimeStamp& time, const UInt_t num,
                                   const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, time, num, Rv);
} 
   
void TSnIOHeartbeatFrame::WriteToW(TSnIOBuffer& b,
                                   const UInt_t time, const UInt_t num,
                                   const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, time, num, Rv);
} 
   
void TSnIOHeartbeatFrame::WriteToW(Char_t*& b,
                                   const UInt_t time, const UInt_t num,
                                   const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, time, num, Rv);
} 
   
void TSnIOHeartbeatFrame::WriteToW(FILE*& b,
                                   const UInt_t time, const UInt_t num,
                                   const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, time, num, Rv);
} 

