#ifndef SNS_TSnCAGuiActsAvailList
#define SNS_TSnCAGuiActsAvailList

#include <TString.h>

#include "TSnCAGuiActsList.h"

class TSnCAGuiActsAvailList : public TSnCAGuiActsList {
 private:
   TString      fActBase; // name of action base class (i.e. TSnCommStatusAction)
   
   void         AddActionNoCheck(const Char_t* a);
   
 public:
   TSnCAGuiActsAvailList() {}
   TSnCAGuiActsAvailList(const Char_t* base,
                         const TGWindow* p = 0,
                         const Int_t id = -1, 
                         const UInt_t options = kSunkenFrame|kDoubleBorder,
                         const Pixel_t back = GetWhitePixel()) :
      TSnCAGuiActsList(p, id, options, back),
      fActBase(base) {}
   virtual ~TSnCAGuiActsAvailList() {}
   
   void          FillActions();
   virtual void  AddAction(const Char_t* a);
      
   
   TString       GetActBase() const { return fActBase; }
   const Char_t* GetActBaseStr() const { return fActBase.Data(); }
   
   void          SetActBase(const Char_t* base) {
      RemoveAll();
      fActBase = base;
      FillActions();
   }
   
   ClassDef(TSnCAGuiActsAvailList, 1); // a gui list box with available actions
};

#endif // SNS_TSnCAGuiActsAvailList
