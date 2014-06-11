#ifndef SNS_TSnInterp1DWvData
#define SNS_TSnInterp1DWvData

#include <TNamed.h>

class TSnInterp1DWvData : public TNamed {
 public:
   TSnInterp1DWvData() {}
   TSnInterp1DWvData(const Char_t* name, const Char_t* title) :
      TNamed(name, title) {
      // normal ctor
   }
   virtual ~TSnInterp1DWvData() {}
   
   // the interface we want to have for any 1D interpolation
   virtual Double_t       FindXAtExtremum(const Bool_t min,
                                          Double_t* const ymax=0) const = 0;
   virtual Double_t       Eval(const Double_t x) const         = 0;
   virtual       TObject* GetInterpolator()                    = 0;
   virtual const TObject* GetInterpolator() const              = 0;
   virtual void           CopyFrom(const TSnInterp1DWvData& x) = 0;
   
   Double_t       FindXAtMaximum() const { return FindXAtExtremum(kFALSE); }
   Double_t       FindXAtMinimum() const { return FindXAtExtremum(kTRUE);  }
   
   virtual void           Draw(Option_t* option = "") {
      TObject* o = GetInterpolator();
      if (o!=0) {
         o->Draw(option);
      }
   }
   
   ClassDef(TSnInterp1DWvData, 1); // ABC defining interface for 1D interpolation of event data
};

#endif // SNS_TSnInterp1DWvData
