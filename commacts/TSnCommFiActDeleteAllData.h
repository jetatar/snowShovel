#ifndef SNS_TSnCommFiActDeleteAllData
#define SNS_TSnCommFiActDeleteAllData

#include "TSnCommFileAction.h"


class TSnCommFiActDeleteAllData : public TSnCommFileAction {
 private:

   virtual TSnCommObject* operator()(const TSnCommIncFile& in);

 public:
   TSnCommFiActDeleteAllData() {}
   TSnCommFiActDeleteAllData(TSnCommActor* a) :
      TSnCommFileAction(a) {}
   virtual ~TSnCommFiActDeleteAllData() {}
   
   
   virtual TString        GetLabel() const { return "Delete All Data On Stn"; }

   ClassDef(TSnCommFiActDeleteAllData, 1); // send delete all data signal
};

#endif // SNS_TSnCommFiActDeleteAllData
