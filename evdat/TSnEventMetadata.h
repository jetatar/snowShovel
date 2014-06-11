#ifndef SNS_TSnEventMetadata
#define SNS_TSnEventMetadata

#include <TObject.h>
#include <TString.h>

class TSnEventMetadata : public TObject {
 private:
   TString       fStnId; // station ID = mac address of the mbed
   UInt_t        fRun;   // run number
   UShort_t      fSeq;   // sequence number
   
 public:
   TSnEventMetadata() : fRun(0), fSeq(0) {}
   TSnEventMetadata(const Char_t* stnid, const UInt_t run, const UShort_t seq) :
      fStnId(stnid), fRun(run), fSeq(seq) {}
   virtual ~TSnEventMetadata() {}
   
   virtual void   Print(Option_t* option="") const;
   virtual Bool_t IsEqual(const TObject* obj) const;

   const Char_t*  GetStationId() const { return fStnId.Data(); }
   UInt_t         GetRunNum() const { return fRun; }
   UShort_t       GetSeqNum() const { return fSeq; }
   
   void           SetStationId(const Char_t* s) { fStnId = s; }
   void           SetRunNum(const UInt_t r) { fRun = r; }
   void           SetSeqNum(const UShort_t s) { fSeq = s; }
   
   
   ClassDef(TSnEventMetadata, 1); // stores (redundant) event meta info for easy access in data trees

};

#endif // SNS_TSnEventMetadata
