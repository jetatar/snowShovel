#include "TSnIOObject.h"

ClassImp(TSnIOObject);

static UInt_t _kMaxInt = kMaxInt; // avoid signed/unsigned comparison warning

Int_t TSnIOObject::TryIntCast(const UInt_t l) {
  if (l>_kMaxInt) {
    throw std::runtime_error(Form(
         "<TSnIOObject::TryIntCast>: "
	 "Length read from buffer [%u] too large "
	 "to convert to integer.",
	 l));
  } else {
    return static_cast<Int_t>(l);  
  }
  return 0;
}

Char_t    TSnIOObject::ReadFromChar(const TSnIOBuffer& buffer) {
   return ReadFrom<Char_t>(buffer);
}

UChar_t   TSnIOObject::ReadFromUChar(const TSnIOBuffer& buffer) {
   return ReadFrom<UChar_t>(buffer);
}

Short_t   TSnIOObject::ReadFromShort(const TSnIOBuffer& buffer) {
   return ReadFrom<Short_t>(buffer);
}

UShort_t  TSnIOObject::ReadFromUShort(const TSnIOBuffer& buffer) {
   return ReadFrom<UShort_t>(buffer);
}

Int_t     TSnIOObject::ReadFromInt(const TSnIOBuffer& buffer) {
   return ReadFrom<Int_t>(buffer);
}

UInt_t    TSnIOObject::ReadFromUInt(const TSnIOBuffer& buffer) {
   return ReadFrom<UInt_t>(buffer);
}

Long64_t  TSnIOObject::ReadFromLong64(const TSnIOBuffer& buffer) {
   return ReadFrom<Long64_t>(buffer);
}

ULong64_t TSnIOObject::ReadFromULong64(const TSnIOBuffer& buffer) {
   return ReadFrom<ULong64_t>(buffer);
}

Float_t   TSnIOObject::ReadFromFloat(const TSnIOBuffer& buffer) {
   return ReadFrom<Float_t>(buffer);
}

Double_t  TSnIOObject::ReadFromDouble(const TSnIOBuffer& buffer) {
   return ReadFrom<Double_t>(buffer);
}

