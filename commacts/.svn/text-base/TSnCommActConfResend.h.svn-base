#ifndef SNS_TSnCommActConfResend
#define SNS_TSnCommActConfResend

#include "TSnCommStatusAction.h"


class TSnCommActConfResend : public TSnCommStatusAction {
 private:
   UShort_t     fMaxResends;    // max number of consecutive times to attempt resending the conf pack (default: 5). if 0, never stop trying.
   
   UShort_t     fConsecResends; // the current number of consecutive resends
   
   virtual TSnCommObject* operator()(const TSnCommIncStatus& is);
                                        
 public:
   TSnCommActConfResend() : fMaxResends(5), fConsecResends(0) {}
   TSnCommActConfResend(TSnCommActor* a, const UShort_t maxresends=5) :
      TSnCommStatusAction(a), fMaxResends(maxresends), fConsecResends(0) {
   }
   virtual ~TSnCommActConfResend() {}

   UShort_t     GetMaxResends() const { return fMaxResends; }
   UShort_t     GetConsecResends() const { return fConsecResends; }
   virtual TString        GetLabel() const { return "Resend Conf If Not On Stn"; }
   
   void         SetMaxResends(const UShort_t m) { fMaxResends = m; }
   void         ResetConsecCounter() { fConsecResends = 0; }
   
   ClassDef(TSnCommActConfResend, 1); // resend 
};

#endif // SNS_TSnCommActConfResend
