#include "TSnIOHeaderFrame.h"

#include <TString.h>

#include <stdexcept>


ClassImp(TSnIOHeaderFrame);

const UInt_t TSnIOHeaderFrame::kMaxSizeOf = 
  sizeof(UChar_t)+sizeof(UInt_t);

void TSnIOHeaderFrame::ReadFromW(const TSnIOBuffer& b,
                                 UChar_t& msgCode, UInt_t& msgLen) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   ReadFrom(b, msgCode, msgLen);
}
   
void TSnIOHeaderFrame::ReadFromW(const Char_t*& b,
                                 UChar_t& msgCode, UInt_t& msgLen) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   ReadFrom(b, msgCode, msgLen);
}
   
void TSnIOHeaderFrame::ReadFromW(FILE*& b,
                                 UChar_t& msgCode, UInt_t& msgLen) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   ReadFrom(b, msgCode, msgLen);
}
   
void TSnIOHeaderFrame::ReadCodeFromW(const TSnIOBuffer& b, UChar_t& msgCode) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   ReadCodeFrom(b, msgCode);
}

   
void TSnIOHeaderFrame::ReadCodeFromW(const Char_t*& b, UChar_t& msgCode) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   ReadCodeFrom(b, msgCode);
}
   
void TSnIOHeaderFrame::ReadCodeFromW(FILE*& b, UChar_t& msgCode) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   ReadCodeFrom(b, msgCode);
}
   
void TSnIOHeaderFrame::WriteToW(TSnIOBuffer& b,
                                const UChar_t msgCode, const UInt_t msgLen) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, msgCode, msgLen);
}

   
void TSnIOHeaderFrame::WriteToW(Char_t*& b,
                                const UChar_t msgCode, const UInt_t msgLen) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, msgCode, msgLen);
}

void TSnIOHeaderFrame::WriteToW(FILE*& b,
                                const UChar_t msgCode, const UInt_t msgLen) {
   // calls the templated function, since rootcint can't
   // parse the template directly.
   // this function will be available in CINT and PyRoot
   WriteTo(b, msgCode, msgLen);
}


TString TSnIOHeaderFrame::GetCodeAsString(const UChar_t code) {
   switch (code) {
      case kStatusCode:
         return "StatusCode";
      case kFileCode:
         return "FileCode";
      case kFilenameCode:
         return "FilenameCode";
      case kFileTrgStrtCode:
         return "FileTrgStrtCode";
      case kFileTrgStopCode:
         return "FileTrgStopCode";
      case kFileNevtsCode:
         return "FileNevtsCode";
      case kFileNpwrsCode:
         return "FileNpwrsCode";
      case kFileHeadrCode:
         return "FileHeadrCode";
      case kFileReadFailCode:
         return "FileReadFailCode";
      case kMbedFileCode:
         return "MbedFileCode";
      case kMbedFilenameCode:
         return "MbedFilenameCode";
      case kMbedFileChksCode:
         return "MbedFileChksCode";
      case kConfigCode:
         return "ConfigCode";
      case kEventCode:
         return "EventCode";
      case kHndShkBits:
         return "HndShkBits";
      case kHnShOkComplCode:
         return "HnShOkComplCode";
      case kHnShOkPartlCode:
         return "HnShOkPartlCode";
      case kHnShOkReqRnCode:
         return "HnShOkReqRnCode";
      case kHnShOkStopCode:
         return "HnShOkStopCode";
      case kHnShOkDelRnCode:
         return "HnShOkDelRnCode";
      case kHnShOkDelAlCode:
         return "HnShOkDelAlCode";
      case kHnShFailPrtCode:
         return "HnShFailPrtCode";
      case kHnShFailNonCode:
         return "HnShFailNonCode";
      case kHnShDemandCode:
         return "HnShDemandCode";
      case kHnShNoReplyCode:
         return "HnShNoReplyCode";
      case kPowerCode:
         return "PowerCode";
      case kHeartbeatCode:
         return "HeartbeatCode";
      case kNoConfigCode:
         return "NoConfigCode";
      case kSbdMsgCode:
         return "SbdMsgCode";
      case kStringCode:
         return "StringCode";
      default:
         return "Unknown Code";
   };
}

