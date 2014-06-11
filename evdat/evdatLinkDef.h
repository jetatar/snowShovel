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

#pragma link C++ class TSnCompressor+;
#pragma link C++ class TSnEvent+;
#pragma link C++ class TSnEventHeader+;
#pragma link C++ class TSnEventMetadata+;
#pragma link C++ class TSnRawWaveform+;
#pragma link C++ class TSnStatusUpdate+;
#pragma link C++ class TSnStatusUpdate::TObsoletePars+;
#pragma link C++ class TSnTrgInfo+;
#pragma link C++ class TSnPowerReading+;
#pragma link C++ class TSnPowerReading::TObsoletePars+;
#pragma link C++ class TSnHeartbeat+;
// for TSnClockSetInfo, we want to clear the cached fAbsCache even
// if we're not streaming it, so we add a special rule for reading
#pragma link C++ class TSnClockSetInfo+;
#pragma read sourceClass="TSnClockSetInfo" targetClass="TSnClockSetInfo" source="" version="[1-]" target="fAbsCache" code="{ if (fAbsCache!=0) { delete fAbsCache; fAbsCache=0; } }"

#pragma link C++ class TSnWindData+;

#ifdef USE_FOR_AUTLOADING // add the namespaces to the rootmap
#pragma link C++ class NSnConstants;
#pragma link C++ class NSnHashUtils;
#pragma link C++ class NSnCRCUtils+;
#else
#pragma link C++ namespace NSnConstants;
#pragma link C++ namespace NSnHashUtils;
#pragma link C++ namespace NSnCRCUtils+;
#endif

#pragma link C++ enum TSnTrgInfo::ETrgBit_t;

#pragma link C++ typedef TSnCompressor::Byte_t;

#pragma link C++ function NSnConstants::WrapSampleNum<UChar_t>;
#pragma link C++ function NSnConstants::WrapSampleNum<Short_t>;
#pragma link C++ function NSnConstants::WrapSampleNum<UShort_t>;
#pragma link C++ function NSnConstants::WrapSampleNum<Int_t>;
#pragma link C++ function NSnConstants::WrapSampleNum<UInt_t>;
#pragma link C++ function NSnConstants::WrapSampleNum<Long64_t>;
#pragma link C++ function NSnConstants::WrapSampleNum<ULong64_t>;

#pragma link C++ function NSnCRCUtils::GetCRC32for<Char_t>;
#pragma link C++ function NSnCRCUtils::GetCRC32for<UChar_t>;
#pragma link C++ function NSnCRCUtils::GetCRC32for<Short_t>;
#pragma link C++ function NSnCRCUtils::GetCRC32for<UShort_t>;
#pragma link C++ function NSnCRCUtils::GetCRC32for<Int_t>;
#pragma link C++ function NSnCRCUtils::GetCRC32for<UInt_t>;
#pragma link C++ function NSnCRCUtils::GetCRC32for<Long64_t>;
#pragma link C++ function NSnCRCUtils::GetCRC32for<ULong64_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<Char_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<UChar_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<Short_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<UShort_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<Int_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<UInt_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<Long64_t>;
#pragma link C++ function NSnCRCUtils::GetUpdatedCRC32for<ULong64_t>;

#endif // __CINT__
