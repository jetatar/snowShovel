#ifndef SNS_TSnCommFiActTransComplete
#define SNS_TSnCommFiActTransComplete

#include "TSnCommFileAction.h"
class TSnCommActor;

class TSnCommFiActTransComplete : public TSnCommFileAction {
 private:
   Float_t   fMinEvtFrc; // (exclusive) min fraction of events that must be received for the "partial ok" signal to be sent
   Float_t   fMinPwrFrc; // (exclusive) min fraction of power readings that must be received for the "partial ok" signal to be sent

   virtual TSnCommObject* operator()(const TSnCommIncFile& in);

 public:
   TSnCommFiActTransComplete() : fMinEvtFrc(1.0), fMinPwrFrc(1.0) {}
   TSnCommFiActTransComplete(TSnCommActor* a,
                             const Float_t minEvtFrc=1.0,
                             const Float_t minPwrFrc=1.0) :
      TSnCommFileAction(a), fMinEvtFrc(minEvtFrc), fMinPwrFrc(minPwrFrc) {}
   virtual ~TSnCommFiActTransComplete() {}
   
   Float_t   GetMinEvtFrc() const { return fMinEvtFrc; }
   Float_t   GetMinPwrFrc() const { return fMinPwrFrc; }
   virtual TString        GetLabel() const { return "Respond To File Transfer"; }
   
   void      SetMinEvtFrc(const Float_t m) { fMinEvtFrc = m; }
   void      SetMinPwrFrc(const Float_t m) { fMinPwrFrc = m; }
   
   ClassDef(TSnCommFiActTransComplete, 1); // send response based on how complete incoming file is
};

#endif // SNS_TSnCommFiActTransComplete
