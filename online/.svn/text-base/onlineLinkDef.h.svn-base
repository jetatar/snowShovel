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

#pragma link C++ class TSnIOBuffer+;
#pragma link C++ class TSnIOObject+;
#pragma link C++ class TSnIOConfigFrame+;
#pragma link C++ class TSnIOEventFrame+;
#pragma link C++ class TSnIOStatusFrame+;
#pragma link C++ class TSnIOHeaderFrame+;
#pragma link C++ class TSnIOPowerFrame+;
#pragma link C++ class TSnIOHeartbeatFrame+;
#pragma link C++ class TSnIOClockSetFrame+;
#pragma link C++ class TSnIOSignalStrengthFrame+;
#pragma link C++ class TSnIODataFile+;
#pragma link C++ class TSnCommObject+;
#pragma link C++ class TSnCommQueue+;
#pragma link C++ class TSnCommConfPack+;
// reset the cached checksum upon read even tho this variable isn't streamed
#pragma link C++ class TSnCommMbedLocalFile+;
#pragma read sourceClass="TSnCommMbedLocalFile" targetClass="TSnCommMbedLocalFile" source="" version="[1-]" target="fChk" code="{ fChk=0; }" 
#pragma link C++ class TSnCommStatusPack+;
#pragma link C++ class TSnCommAlertPack+;
#pragma link C++ class TSnCommIncoming+;
#pragma link C++ class TSnCommIncFile+;
#pragma link C++ class TSnCommIncStatus+;
#pragma link C++ class TSnCommIncHandshake+;
#pragma link C++ class TSnCommProtocolMsg+;
#pragma link C++ class TSnCommDeleteDataMsg+;
#pragma link C++ class TSnCommActor+;
#pragma link C++ class TSnCommAction+;
#pragma link C++ class TSnCommStatusAction+;
#pragma link C++ class TSnCommFileAction+;

#pragma link C++ typedef TSnIOBuffer::Pos_t;
#pragma link C++ typedef TSnIOBuffer::iterator;
#pragma link C++ typedef TSnIOBuffer::const_iterator;

// template functions galore!
#pragma link C++ function TSnIOObject::ReadFrom<Char_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<UChar_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<Short_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<UShort_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<Int_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<UInt_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<Long64_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<ULong64_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<Float_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<Double_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::ReadFrom<Char_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<UChar_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<Short_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<UShort_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<Int_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<UInt_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<Long64_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<ULong64_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<Float_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<Double_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::ReadFrom<Char_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<UChar_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<Short_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<UShort_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<Int_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<UInt_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<Long64_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<ULong64_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<Float_t>(FILE*);
#pragma link C++ function TSnIOObject::ReadFrom<Double_t>(FILE*);

#pragma link C++ function TSnIOObject::WriteTo<Char_t>(TSnIOBuffer&, const Char_t);
#pragma link C++ function TSnIOObject::WriteTo<UChar_t>(TSnIOBuffer&, const UChar_t);
#pragma link C++ function TSnIOObject::WriteTo<Short_t>(TSnIOBuffer&, const Short_t);
#pragma link C++ function TSnIOObject::WriteTo<UShort_t>(TSnIOBuffer&, const UShort_t);
#pragma link C++ function TSnIOObject::WriteTo<Int_t>(TSnIOBuffer&, const Int_t);
#pragma link C++ function TSnIOObject::WriteTo<UInt_t>(TSnIOBuffer&, const UInt_t);
#pragma link C++ function TSnIOObject::WriteTo<Long64_t>(TSnIOBuffer&, const Long64_t);
#pragma link C++ function TSnIOObject::WriteTo<ULong64_t>(TSnIOBuffer&, const ULong64_t);
#pragma link C++ function TSnIOObject::WriteTo<Float_t>(TSnIOBuffer&, const Float_t);
#pragma link C++ function TSnIOObject::WriteTo<Double_t>(TSnIOBuffer&, const Double_t);
#pragma link C++ function TSnIOObject::WriteTo<Char_t>(Char_t*&, const Char_t);
#pragma link C++ function TSnIOObject::WriteTo<UChar_t>(Char_t*&, const UChar_t);
#pragma link C++ function TSnIOObject::WriteTo<Short_t>(Char_t*&, const Short_t);
#pragma link C++ function TSnIOObject::WriteTo<UShort_t>(Char_t*&, const UShort_t);
#pragma link C++ function TSnIOObject::WriteTo<Int_t>(Char_t*&, const Int_t);
#pragma link C++ function TSnIOObject::WriteTo<UInt_t>(Char_t*&, const UInt_t);
#pragma link C++ function TSnIOObject::WriteTo<Long64_t>(Char_t*&, const Long64_t);
#pragma link C++ function TSnIOObject::WriteTo<ULong64_t>(Char_t*&, const ULong64_t);
#pragma link C++ function TSnIOObject::WriteTo<Float_t>(Char_t*&, const Float_t);
#pragma link C++ function TSnIOObject::WriteTo<Double_t>(Char_t*&, const Double_t);
#pragma link C++ function TSnIOObject::WriteTo<Char_t>(FILE*, const Char_t);
#pragma link C++ function TSnIOObject::WriteTo<UChar_t>(FILE*, const UChar_t);
#pragma link C++ function TSnIOObject::WriteTo<Short_t>(FILE*, const Short_t);
#pragma link C++ function TSnIOObject::WriteTo<UShort_t>(FILE*, const UShort_t);
#pragma link C++ function TSnIOObject::WriteTo<Int_t>(FILE*, const Int_t);
#pragma link C++ function TSnIOObject::WriteTo<UInt_t>(FILE*, const UInt_t);
#pragma link C++ function TSnIOObject::WriteTo<Long64_t>(FILE*, const Long64_t);
#pragma link C++ function TSnIOObject::WriteTo<ULong64_t>(FILE*, const ULong64_t);
#pragma link C++ function TSnIOObject::WriteTo<Float_t>(FILE*, const Float_t);
#pragma link C++ function TSnIOObject::WriteTo<Double_t>(FILE*, const Double_t);

#pragma link C++ function TSnIOObject::WriteStrTo<TSnIOBuffer>(TSnIOBuffer&,const Char_t* const,const UInt_t);
#pragma link C++ function TSnIOObject::WriteStrTo<Char_t*>(Char_t*&,const Char_t* const,const UInt_t);
#pragma link C++ function TSnIOObject::WriteStrTo<FILE*>(FILE*&,const Char_t* const,const UInt_t);

#pragma link C++ function TSnIOObject::PeekAt<Char_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<UChar_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<Short_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<UShort_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<Int_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<UInt_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<Long64_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<ULong64_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<Float_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<Double_t>(const TSnIOBuffer&);
#pragma link C++ function TSnIOObject::PeekAt<Char_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<UChar_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<Short_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<UShort_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<Int_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<UInt_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<Long64_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<ULong64_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<Float_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<Double_t>(const Char_t*&);
#pragma link C++ function TSnIOObject::PeekAt<Char_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<UChar_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<Short_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<UShort_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<Int_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<UInt_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<Long64_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<ULong64_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<Float_t>(FILE*);
#pragma link C++ function TSnIOObject::PeekAt<Double_t>(FILE*);

// the template functions for the other IO classes are called via wrapper
// functions because rootcint can't parse the templates
// (it takes 'const TSnIOBuffer' -> 'constTSnIOBuffer' in places,
// resulting in a syntax error)

#pragma link C++ typedef TSnIOBuffer::Pos_t;
#pragma link C++ typedef TSnIOBuffer::iterator;
#pragma link C++ typedef TSnIOBuffer::const_iterator;
#pragma link C++ function TSnIOBuffer::IncrementPos<TSnIOBuffer::iterator>(const TSnIOBuffer::Pos_t, const TSnIOBuffer::iterator, const TSnIOBuffer::iterator) const;
#pragma link C++ function TSnIOBuffer::IncrementPos<TSnIOBuffer::const_iterator>(const TSnIOBuffer::Pos_t, const TSnIOBuffer::const_iterator, const TSnIOBuffer::const_iterator) const;
#pragma link C++ function TSnIOBuffer::DecrementPos<TSnIOBuffer::iterator>(const TSnIOBuffer::Pos_t, const TSnIOBuffer::iterator, const TSnIOBuffer::iterator) const;
#pragma link C++ function TSnIOBuffer::DecrementPos<TSnIOBuffer::const_iterator>(const TSnIOBuffer::Pos_t, const TSnIOBuffer::const_iterator, const TSnIOBuffer::const_iterator) const;

#endif // __CINT__
