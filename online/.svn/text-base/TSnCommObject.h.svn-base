#ifndef SNS_TSnCommObject
#define SNS_TSnCommObject

#ifndef ROOT_TObject
#include <TObject.h>
#endif
class TString;

class TSnIOBuffer;

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
//
// TSnCommObject
//
// ABC defining the interface of communication objects that can be stored in
// a comm queue.
//
////////////////////////////////////////////////////////////////////////////////

class TSnCommObject : public TObject {
 public:
   TSnCommObject() {}
   virtual ~TSnCommObject() {}

   // these functions allow direct communication with a station
   virtual UInt_t GetIOByteArraySize(const Bool_t withHeader) const=0;
   virtual UInt_t WriteIOByteArrayTo(TSnIOBuffer& b) const=0;
   virtual TSnIOBuffer GetIOByteArray() const;
   
   virtual TSnCommObject* NewClone() const=0;

   virtual Bool_t  IsOkForStatusVersion(const Int_t sv) const=0;
   
   virtual Bool_t  IsEqual(const TObject* obj) const=0;
   virtual ULong_t Hash() const=0;
   
   virtual TString GetLabel() const=0;
   
   virtual void   Print(Option_t *option="") const=0;
   virtual void   ls(Option_t *option="") const;
   
   ClassDef(TSnCommObject, 1); // ABC for data that can be sent to stations
};

#endif // SNS_TSnCommObject
