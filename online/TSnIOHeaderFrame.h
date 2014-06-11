#ifndef SNS_TSnIOHeaderFrame
#define SNS_TSnIOHeaderFrame

#include "TSnIOObject.h"


class TSnIOHeaderFrame : public TSnIOObject {
 public:
   static const UInt_t  kMaxSizeOf;
  
   // these are not in an enum in order to ensure that they are 1 byte
   // MUST MATCH MBED!
   static const UChar_t kStatusCode      = 0x10;
   static const UChar_t kFileCode        = 0x20;
   static const UChar_t kFilenameCode    = 0x23;
   static const UChar_t kFileTrgStrtCode = 0x24;
   static const UChar_t kFileTrgStopCode = 0x25;
   static const UChar_t kFileNevtsCode   = 0x26;
   static const UChar_t kFileNpwrsCode   = 0x28;
   static const UChar_t kFileHeadrCode   = 0x2A;
   static const UChar_t kFileReadFailCode= 0x2C;
   static const UChar_t kMbedFileCode    = 0x30;
   static const UChar_t kMbedFilenameCode= 0x33;
   static const UChar_t kMbedFileChksCode= 0x35;
   static const UChar_t kConfigCode      = 0x40;
   static const UChar_t kEventCode       = 0x50;
   static const UChar_t kHndShkBits      = 0x60; // all handshakes have these bits set
   static const UChar_t kHnShOkComplCode = 0x61;
   static const UChar_t kHnShOkPartlCode = 0x64;
   static const UChar_t kHnShOkReqRnCode = 0x65;
   static const UChar_t kHnShOkStopCode  = 0x66;
   static const UChar_t kHnShOkDelRnCode = 0x67;
   static const UChar_t kHnShOkDelAlCode = 0x68;
   static const UChar_t kHnShFailPrtCode = 0x6A;
   static const UChar_t kHnShFailNonCode = 0x6C;
   static const UChar_t kHnShDemandCode  = 0x6E;
   static const UChar_t kHnShNoReplyCode = 0x6F;
   static const UChar_t kPowerCode       = 0x70;
   static const UChar_t kHeartbeatCode   = 0x78;
   static const UChar_t kNoConfigCode    = 0x80;
   static const UChar_t kSbdMsgCode      = 0x90;
   static const UChar_t kStringCode      = 0xA0;
   static const UChar_t kSignalStrCode   = 0xB0;

 public:
  TSnIOHeaderFrame() {}
  virtual ~TSnIOHeaderFrame() {}
  
  static
  UInt_t         GetMaxSizeOf() { return kMaxSizeOf; }
  
  static
  UInt_t         SizeOf() { return kMaxSizeOf; }
  
  
  template<typename T>
  static
  void ReadFrom(T& b,
                UChar_t& msgCode,
                UInt_t&  msgLen) {
     ReadCodeFrom(b, msgCode);
     msgLen  = TSnIOObject::ReadFrom<UInt_t>(b);
  }

  template<typename T>
  static
  void ReadCodeFrom(T& b,
                    UChar_t& msgCode) {
     msgCode = TSnIOObject::ReadFrom<UChar_t>(b);
  }

  static
  UChar_t PeekCodeFrom(const TSnIOBuffer& buf) {
     return TSnIOObject::PeekAt<Char_t>(buf);
  }

  static
  Int_t PyPeekCodeFrom(const TSnIOBuffer& buf) {
     return static_cast<Int_t>(PeekCodeFrom(buf));
  }
  
  static
  UInt_t PyReadCodeFrom(const TSnIOBuffer& buf,
                        Int_t& msgCode) {
     // for use in python, since UChar_t& binding does not work
     UChar_t c;
     const TSnIOBuffer::Pos_t op = buf.GetPosVal();
     ReadCodeFrom(buf, c);
     msgCode = static_cast<Int_t>(c);
     return buf.GetPosVal() - op;
  }
   
  static
  UInt_t PyReadFrom(const TSnIOBuffer& buf,
		    Int_t& msgCode,
		    Int_t& msgLen) {
     // for use in python, since UChar_t& binding does not work
     UChar_t c; UInt_t l;
     const TSnIOBuffer::Pos_t op = buf.GetPosVal();
     ReadFrom(buf, c, l);
     msgCode = static_cast<Int_t>(c);
     msgLen  = TryIntCast(l);
     return buf.GetPosVal() - op;
  }
  
  template<typename T>
  static
  void WriteTo(T& b,
               const UChar_t msgCode,
               const UInt_t  msgLen) {
    TSnIOObject::WriteTo(b, msgCode);
    TSnIOObject::WriteTo(b, msgLen);
  }

   static
   TSnIOBuffer GetIOByteArray(const UChar_t msgCode,
                              const UChar_t msgLen) {
      // make an IO buffer and fill it.
      TSnIOBuffer b;
      WriteTo(b, msgCode, msgLen);
      return b;
   }

   static
   UShort_t GetSbdMsgNumFromHdLen(const UInt_t hlen) {
      return (hlen>>16u);
   }
   
   static
   UShort_t GetSbdMsgTotFromHdLen(const UInt_t hlen) {
      return (hlen&0xFFFF);
   }
   
   static
   UInt_t GetSbdHdLenFrom(const UShort_t mnum,
			  const UShort_t mtot) {
      UInt_t h = mnum;
      h <<= 16u;
      h  |= mtot;
      return h;
   }

   static
   TString GetCodeAsString(const UChar_t code);

   // rootcint can't parse the templated functions, so wrap them
   static
   void ReadFromW(const TSnIOBuffer& b, UChar_t& msgCode, UInt_t& msgLen);
   static
   void ReadFromW(const Char_t*& b, UChar_t& msgCode, UInt_t& msgLen);
   static
   void ReadFromW(FILE*& b, UChar_t& msgCode, UInt_t& msgLen);
   static
   void ReadCodeFromW(const TSnIOBuffer& b, UChar_t& msgCode);
   static
   void ReadCodeFromW(const Char_t*& b, UChar_t& msgCode);
   static
   void ReadCodeFromW(FILE*& b, UChar_t& msgCode);
   static
   void WriteToW(TSnIOBuffer& b, const UChar_t msgCode, const UInt_t msgLen);
   static
   void WriteToW(Char_t*& b, const UChar_t msgCode, const UInt_t msgLen);
   static
   void WriteToW(FILE*& b, const UChar_t msgCode, const UInt_t msgLen);

   ClassDef(TSnIOHeaderFrame, 1); // read/write a header frame from/to a byte stream
};

#endif // SNS_TSnIOHeaderFrame
