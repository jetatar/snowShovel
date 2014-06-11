#include "TSnCAGuiActsAvailList.h"

#include <TClass.h>
#include <TClassTable.h>

#include <stdexcept>

ClassImp(TSnCAGuiActsAvailList);

void TSnCAGuiActsAvailList::FillActions() {
   // uses TClassTable to build a list of classes inheriting
   // from the specified base. the necessary libraries must
   // already be loaded.
   
   TClassTable::Init();
   
   const Char_t* s=0;
   const Int_t nclasses = gClassTable->Classes();
   for (Int_t i=0; i<nclasses; ++i) {
      s = TClassTable::At(i);
      AddAction(s);
   }
}

void TSnCAGuiActsAvailList::AddAction(const Char_t* a) {
   // add the action to the list if it inherits from the specified base

   TClass* c = TClass::GetClass(a, kTRUE, kTRUE);
   TClass* base = TClass::GetClass(fActBase.Data(), kTRUE, kTRUE);
   if ((c!=0) && (base!=0)) {
      if (c!=base) {
         if (c->InheritsFrom(base)) {
            TSnCAGuiActsList::AddAction(a);
         }
      }
   } else {
      throw std::runtime_error(Form("<TSnCAGuiActsAvailList::AddAction>: "
                                    "Could not get classes [%s] and [%s]",
                                    a, fActBase.Data()));
   }
}

