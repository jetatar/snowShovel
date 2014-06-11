#ifndef SNS_TSnCommProtocolMsg
#define SNS_TSnCommProtocolMsg

#include "TSnCommObject.h"

class TSnCommProtocolMsg : public TSnCommObject {
 private:
   UChar_t     fMsgCode; // the protocol type code (see TSnIOHeaderFrame)
   UInt_t      fMsgPayl; // the protocol payload (typically length of the comm payload, but depends on the protocol code)

 protected:
   TSnCommProtocolMsg(const TSnCommProtocolMsg& p) : 
      TSnCommObject(p), fMsgCode(p.fMsgCode), fMsgPayl(p.fMsgPayl) {}
   
 public:
   TSnCommProtocolMsg(const UChar_t code=0, const UInt_t payl=0) :
      fMsgCode(code), fMsgPayl(payl) {}
   virtual ~TSnCommProtocolMsg() {}
   
   UChar_t     GetMsgCode() const { return fMsgCode; }
   UInt_t      GetMsgPayload() const { return fMsgPayl; }
   
   void        SetMsgCode(const UChar_t m) { fMsgCode = m; }
   void        SetMsgPayload(const UInt_t p) { fMsgPayl = p; }

   virtual UInt_t GetIOByteArraySize(const Bool_t withHeader) const;
   virtual UInt_t WriteIOByteArrayTo(TSnIOBuffer& b) const;
   
   virtual TSnCommObject* NewClone() const 
      { return new TSnCommProtocolMsg(*this); }

   virtual Bool_t  IsOkForStatusVersion(const Int_t sv) const;
   
   virtual Bool_t  IsEqual(const TObject* obj) const;
   virtual ULong_t Hash() const;
   
   virtual TString GetLabel() const;
   
   virtual void   Print(Option_t *option="") const;
   
   ClassDef(TSnCommProtocolMsg, 1); // a simple protocol-based message
};

#endif // SNS_TSnCommProtocolMsg

