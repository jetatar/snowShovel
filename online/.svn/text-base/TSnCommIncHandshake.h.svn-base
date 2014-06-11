#ifndef SNS_TSnCommIncHandshake
#define SNS_TSnCommIncHandshake

#include "TSnCommIncFile.h"

class TSnCommIncHandshake : public TSnCommIncFile {
// inherits from TSnCommIncFile because the file actions
// will be used to respond to the handshake
// (requests for all data of a run, requests to delete data, etc.)

 private:
   UChar_t     fHnShCode; // the handshake code

 public:
   TSnCommIncHandshake(const UChar_t hnshCode=0) :
      fHnShCode(hnshCode) {}
   TSnCommIncHandshake(const UChar_t hnshCode,
                       const TTimeStamp& t,
                       const TSnStatusUpdate& s,
                       const Int_t stvers):
      TSnCommIncFile(t, s, stvers),
      fHnShCode(hnshCode) {}
   virtual ~TSnCommIncHandshake() {}
   
   UChar_t        GetHnShCode() const { return fHnShCode; }
   
   void           SetHnShCode(const UChar_t h) { fHnShCode = h; }

   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TSnCommIncHandshake, 1); // a pseudo-incoming-file that's really just a handshaking code
};

#endif // SNS_TSnCommIncHandshake
