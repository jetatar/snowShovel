#include "TSnIOStatusFrame.h"

ClassImp(TSnIOStatusFrame);

const UChar_t TSnIOStatusFrame::kIOvers   = 4; // MUST INCREMENT if WriteTo chgs
const UInt_t  TSnIOStatusFrame::kMaxSizeOfV1 = // is this 1 byte too big?
        sizeof(UChar_t) + sizeof(ULong64_t)
     + (sizeof(UInt_t)*3u) + (sizeof(UShort_t))
     + (sizeof(UChar_t)*3u) + TSnDAQConfig::kMaxLblLen
     + TSnIOEventFrame::kMaxSizeOf;
const UInt_t  TSnIOStatusFrame::kMaxSizeOfV2 =
  sizeof(ULong64_t) + (3u*sizeof(UInt_t)) + (3u*sizeof(UShort_t))
  + (3u*sizeof(UChar_t)) + (2u*sizeof(Float_t))
  + TSnDAQConfig::kMaxLblLen;
const UInt_t  TSnIOStatusFrame::kMaxSizeOfV3 =
   kMaxSizeOfV2 + sizeof(UInt_t) + sizeof(Float_t);
const UInt_t  TSnIOStatusFrame::kMaxSizeOfV4 =
   kMaxSizeOfV3 - sizeof(UInt_t);
const UInt_t  TSnIOStatusFrame::kMaxSizeOfV5 = kMaxSizeOfV4 + sizeof(UChar_t);
const UInt_t  TSnIOStatusFrame::kMaxSizeOf = kMaxSizeOfV1;

UInt_t TSnIOStatusFrame::GetMaxSizeOf(const UChar_t rv) {
     if (rv==1) {
	return kMaxSizeOfV1;
     } else if (rv==2) {
	return kMaxSizeOfV2;
     } else if (rv==3) {
	return kMaxSizeOfV3;
     } else if (rv==4) {
        return kMaxSizeOfV4;
     } else if (rv==5) {
        return kMaxSizeOfV5;
     } else {
	throw std::runtime_error(Form("<TSnIOStatusFrame::GetMaxSizeOf>: "
				      "Unknown read version [%hhu]", rv));
     }
   }

UInt_t TSnIOStatusFrame::SizeOf(const UChar_t rv,
                                const UInt_t confLblLen,
                                const UChar_t loseLSB,
                                const UChar_t loseMSB) {
   // number of bytes read/written during i/o
   if (rv>0) {
      const UInt_t maxsize = GetMaxSizeOf(rv);
      const UInt_t msz = maxsize - TSnDAQConfig::kMaxLblLen
                         + confLblLen;
      if (rv==1) {
	if ((loseLSB==0) && (loseMSB==0)) {
	  return msz;
	} else {
	  return msz - TSnIOEventFrame::kMaxSizeOf 
	    + TSnIOEventFrame::SizeOf(TSnIOEventFrame::kIOvers,
				      loseLSB, loseMSB);
	}
      } else if (rv==2 || rv==3 || rv==4 || rv==5) {
	return msz;
      }
   }
   throw std::runtime_error(Form("<TSnIOStatusFrame::SizeOf>: "
				 "Unknown read version [%hhu]", rv));
   return 0;
}

UInt_t TSnIOStatusFrame::SizeOf(const UChar_t rv, const TSnDAQConfig& conf) {
   return SizeOf(rv, conf.GetLabelLen(),
                 conf.GetWvLoseLSB(), conf.GetWvLoseMSB());
}

UChar_t TSnIOStatusFrame::ReadFromW(const TSnIOBuffer& b, 
                                    TSnStatusUpdate& stat,TSnEvent& evt) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, stat, evt);
}
   
UChar_t TSnIOStatusFrame::ReadFromW(const TSnIOBuffer& b,
                                    TSnStatusUpdate& stat,TSnEvent* evt) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, stat, evt);
}

   
UChar_t TSnIOStatusFrame::ReadFromW(const Char_t*& b,
                                    TSnStatusUpdate& stat,TSnEvent& evt) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, stat, evt);
}
   
UChar_t TSnIOStatusFrame::ReadFromW(const Char_t*& b,
                                    TSnStatusUpdate& stat,TSnEvent* evt) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   return ReadFrom(b, stat, evt);
}
   
void TSnIOStatusFrame::WriteToW(TSnIOBuffer& b, const TSnStatusUpdate& stat,
                                const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, stat, Rv);
}
   
void TSnIOStatusFrame::WriteToW(Char_t*& b, const TSnStatusUpdate& stat,
                                const UChar_t Rv) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, stat, Rv);
}
   
