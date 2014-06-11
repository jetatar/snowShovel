#include "TSnCAGuiActsList.h"

#include <TObjArray.h>
#include <TClass.h>

#include "TSnCommAction.h"

#include <stdexcept>

ClassImp(TSnCAGuiActsList);

TSnCAGuiActsList::TSnCAGuiActsList(const TGWindow* p,
                                   const Int_t id, 
                                   const UInt_t options,
                                   const Pixel_t back) :
   TGListBox(p, id, options, back),
   fActList(new TObjArray) {
   fActList->SetOwner(kTRUE);
}

TSnCAGuiActsList::~TSnCAGuiActsList() {
   delete fActList;
}


void TSnCAGuiActsList::AddAction(const Char_t* a) {
   // add the action to the end of the list
   
   TObject* e = fActList->FindObject(a);
   if (e==0) {
      TClass* c = TClass::GetClass(a, kTRUE, kTRUE);
      if (c!=0) {
         if (c->InheritsFrom(TSnCommAction::Class())) {
            TSnCommAction* atn = reinterpret_cast<TSnCommAction*>(c->New());
            if (atn!=0) {
               const Int_t i = fActList->AddAtFree(atn);
               AddEntry(atn->GetLabel(), i);
            } else {
               throw std::runtime_error(Form("<TSnCAGuiActsList::AddAction>: "
                                             "Could not make instance of "
                                             "class [%s]",a));
            }
         } else {
            throw std::runtime_error(Form("<TSnCAGuiActsList::AddAction>: "
                                          "Class [%s] is not a TSnCommAction",
                                          a));
         }
      } else {
         throw std::runtime_error(Form("<TSnCAGuiActsList::AddAction>: "
                                       "Could not get class [%s].",a));
      }
   }
   
}

void TSnCAGuiActsList::RemoveSelected() {
   // remove the selected entry from the list

   const Int_t i = GetSelected();
   delete fActList->RemoveAt(i);
   RemoveEntry(i);
}
