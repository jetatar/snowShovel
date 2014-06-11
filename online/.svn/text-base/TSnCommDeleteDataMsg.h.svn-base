#ifndef SNS_TSnCommDeleteDataMsg
#define SNS_TSnCommDeleteDataMsg

#include "TSnCommProtocolMsg.h"

class TSnCommDeleteDataMsg : public TSnCommProtocolMsg {
 public:
   static const Char_t* kMagicString; // the magic string so the station knows it didn't receive a bad byte with the delete data code

 protected:
   TSnCommDeleteDataMsg(const TSnCommDeleteDataMsg& m) :
      TSnCommProtocolMsg(m) {}
   
 public:
   TSnCommDeleteDataMsg(const UChar_t code=0, const UInt_t payl=0) :
      TSnCommProtocolMsg(code, payl) {}
   virtual ~TSnCommDeleteDataMsg() {}
   
   virtual UInt_t GetIOByteArraySize(const Bool_t withHeader) const;
   virtual UInt_t WriteIOByteArrayTo(TSnIOBuffer& b) const;
   
   // important! don't forget to define this! we don't want to use
   // a base class' version
   virtual TSnCommObject* NewClone() const 
      { return new TSnCommDeleteDataMsg(*this); }
   
   virtual Bool_t  IsEqual(const TObject* obj) const;

   ClassDef(TSnCommDeleteDataMsg, 1); // send a delete data signal (with magic string)
};

#endif // SNS_TSnCommDeleteDataMsg
