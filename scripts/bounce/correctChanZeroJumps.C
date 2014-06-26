#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TTree.h>
#include <TList.h>
#include <TIterator.h>
#include <TKey.h>
#include <TClass.h>
#include <TString.h>
#include <TMath.h>
#include <TGraph.h>

#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"
#include "TSnCalWvData.h"
#include "NSnConstants.h"
#endif

TFile* inf, * outf, * jumpf;
TTree* ct, * ot, * rt;

void correctChanZeroJumps(const Char_t* infn,
                          const Char_t* jumpfn,
                          const Char_t* outfn,
                          const Char_t* treenm="CalibTree",
                          const Char_t* databr="AmpOutData.") {
   // see elog:1212
   
   inf = TFile::Open(infn);
   
   outf = TFile::Open(outfn, "recreate");
   outf->cd();
   
   TList* keys = inf->GetListOfKeys();
   TIter nk(keys);
   TKey* k;
   TObject* o;
   TClass* c;
   while ( (k=dynamic_cast<TKey*>(nk()))!=0 ) {
      o = k->ReadObj();
      if (o!=0) {
         c = o->IsA();
         if (c!=0) {
            if (c->InheritsFrom("TTree")) {
               TTree* t  = dynamic_cast<TTree*>(o);
               TString tn(t->GetName());
               if (tn.CompareTo(treenm)!=0) {
                  TTree* cpyt = t->CloneTree(-1, "fast");
                  cpyt->SetDirectory(outf);
                  outf->cd();
                  cpyt->Write();
                  delete cpyt;
               } else {
                  ct = t; // our tree
               }
            }
         }
      }
   }
   
   if (ct==0) {
      Printf("No [%s] in [%s]",treenm,infn);
      delete inf;
      return;
   }
   
   const Long64_t nents = ct->GetEntries();
   if (nents<1) {
      Printf("No entries in [%s] in [%s]",treenm,infn);
      delete ct; ct = 0;
      return;
   }
   
   TSnEventMetadata* meta = new TSnEventMetadata;
   TSnEventHeader* head = new TSnEventHeader;
   TSnCalWvData* data = new TSnCalWvData;
   ct->SetBranchAddress("EventMetadata.",&meta);
   ct->SetBranchAddress("EventHeader.",&head);
   ct->SetBranchAddress(databr,&data);
   ot = ct->CloneTree(0);
   ot->SetDirectory(outf);
   
   jumpf = TFile::Open(jumpfn);
   rt = dynamic_cast<TTree*>(jumpf->Get("RelChanShifts"));
   if (rt==0) {
      Printf("No RelChanShifts in [%s]",jumpfn);
      delete ot; delete ct; delete inf; delete outf;
      return;
   }
   TString hn;
   Double_t chOffsets[NSnConstants::kNchans][NSnConstants::kNchans];
   for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (UChar_t xc=0; xc<ch; ++xc) {
         chOffsets[ch][xc] = 0;
         hn = Form("shiftCh%hhuCh%hhu",ch,xc);
         rt->SetBranchAddress(hn.Data(),&(chOffsets[ch][xc]));
      }
   }
   rt->BuildIndex("EventMetadata.fRun","EventHeader.fNum");
   
   for (Long64_t i=0; i<nents; ++i) {
      ct->GetEntry(i);
      
      const Long64_t ri = rt->GetEntryNumberWithIndex(meta->GetRunNum(),
                                                      head->GetEvtNum());
      if (ri>-1) {
         rt->GetEntry(ri);
         
         // just use the ch1-ch0 shift
         const Int_t smpoff = TMath::Nint( chOffsets[1][0] 
                                           * NSnConstants::kSampRate );
         if (smpoff!=0) {
            // shift ch0
            TGraph* g = 
               data->NewGraphForChan(0, kFALSE, kFALSE, smpoff, kTRUE);
            const Double_t* s = g->GetY();
            Float_t*        d = data->GetData();
            for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++s, ++d) {
               *d = static_cast<Float_t>(*s);
            }
            delete g;
         }
         
         ot->Fill();
      }
   }
   
   outf->Write();
   
}

