#include "TSnCAGuiActorFrame.h"

#include "TSnCAGuiActsList.h"
#include "TSnCAGuiActsAvailList.h"

#include <TGLabel.h>
#include <TGButton.h>



ClassImp(TSnCAGuiActorFrame);

TSnCAGuiActorFrame::~TSnCAGuiActorFrame() {
   delete fAvailLbl;
   delete fActorLbl;
   delete fAvailList;
   delete fActorList;
   delete fToActorBtn;
   delete fFromActorBtn;
   delete fActnUpBtn;
   delete fActnDownBtn;
   delete fActDescLbl;
   delete fSaveBtn;
}

void TSnCAGuiActorFrame::BuildGui(const Char_t* a) {

   delete fAvailLbl;
   fAvailLbl = new TGLabel(this,"Available Actions");
   fAvailLbl->SetTextJustify(36);
   fAvailLbl->SetMargins(0,0,0,0);
   fAvailLbl->SetWrapLength(-1);
   AddFrame(fAvailLbl, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fAvailLbl->MoveResize(56,16,168,32);

   delete fActorLbl;
   fActorLbl = new TGLabel(this,"Actions in Actor");
   fActorLbl->SetTextJustify(36);
   fActorLbl->SetMargins(0,0,0,0);
   fActorLbl->SetWrapLength(-1);
   AddFrame(fActorLbl, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fActorLbl->MoveResize(336,16,168,32);
   
   delete fAvailList;
   fAvailList = new TSnCAGuiActsAvailList(a, this);
   fAvailList->FillActions();
   fAvailList->Resize(168,200);
   AddFrame(fAvailList, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fAvailList->MoveResize(56,56,168,200);
   
   delete fActorList;
   fActorList = new TSnCAGuiActsList(this);
   fActorList->Resize(168,200);
   AddFrame(fActorList, new TGLayoutHints(kLHintsNormal));
   fActorList->MoveResize(336,56,168,200);
   
   delete fToActorBtn;
   fToActorBtn = new TGTextButton(this, ">>>");
   fToActorBtn->SetTextJustify(36);
   fToActorBtn->SetMargins(0,0,0,0);
   fToActorBtn->SetWrapLength(-1);
   fToActorBtn->Resize(70,40);
   AddFrame(fToActorBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fToActorBtn->MoveResize(248,72,70,40);
   
   delete fFromActorBtn;
   fFromActorBtn = new TGTextButton(this, "<<<");
   fFromActorBtn->SetTextJustify(36);
   fFromActorBtn->SetMargins(0,0,0,0);
   fFromActorBtn->SetWrapLength(-1);
   fFromActorBtn->Resize(70,40);
   AddFrame(fFromActorBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fFromActorBtn->MoveResize(248,144,70,40);
   
   delete fActnUpBtn;
   fActnUpBtn = new TGTextButton(this, "Raise");
   fActnUpBtn->SetTextJustify(36);
   fActnUpBtn->SetMargins(0,0,0,0);
   fActnUpBtn->SetWrapLength(-1);
   fActnUpBtn->Resize(70,40);
   AddFrame(fActnUpBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fActnUpBtn->MoveResize(512,72,70,40);
   
   delete fActnDownBtn;
   fActnDownBtn = new TGTextButton(this, "Lower");
   fActnDownBtn->SetTextJustify(36);
   fActnDownBtn->SetMargins(0,0,0,0);
   fActnDownBtn->SetWrapLength(-1);
   fActnDownBtn->Resize(70,40);
   AddFrame(fActnDownBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fActnDownBtn->MoveResize(512,144,70,40);
   
   delete fActDescLbl;
   fActDescLbl = new TGLabel(this, "Action Description");
   fActDescLbl->SetTextJustify(36);
   fActDescLbl->SetMargins(0,0,0,0);
   fActDescLbl->SetWrapLength(-1);
   AddFrame(fActDescLbl, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fActDescLbl->MoveResize(336,272,240,80);
   
   delete fSaveBtn;
   fSaveBtn = new TGTextButton(this, "Save Actor");
   fSaveBtn->SetTextJustify(36);
   fSaveBtn->SetMargins(0,0,0,0);
   fSaveBtn->SetWrapLength(-1);
   fSaveBtn->Resize(70,40);
   AddFrame(fSaveBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fSaveBtn->MoveResize(56,272,240,80);
   
   SetLayoutManager(new TGVerticalLayout(this));
   SetLayoutBroken(kTRUE);
}

