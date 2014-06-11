#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TFile.h>
#include <TList.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnBasicEvtSelMod.h"
#include "TSnTrgInfo.h"
#include "TSnStopFindRms.h"
#include "TSnShiftCalWvDataMod.h"
#include "TSnMakeShiftTree.h"

#include <stdexcept>

#endif

#include "loadtree.C"

TChain* nt(0);
TFile* outf(0);

void stopFindStudy(const Char_t* infn,
                   const Char_t* outfn,
                   const UShort_t stopSearchWin=4,
                   const UShort_t jitter=3,
                   const Bool_t useThermals=kTRUE,
                   const Bool_t useForced=kFALSE) {
   
   TString dbn;
   const Bool_t ok = tryLoadDataTree(infn, nt, &dbn);
   if (ok==kFALSE) {
      Printf("Could not load data from [%s].",infn);
      return;
   }
   nt->Add(infn);
   
   TAMSelector* sel = new TAMSelector;
   
   // the event selection modules
   TSnBasicEvtSelMod* bes = new TSnBasicEvtSelMod("BasicEvtSelMod");
   if (useThermals) {
      bes->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   }
   if (useForced) {
      bes->GetTrgBits().EnableTrig(TSnTrgInfo::kForced);
   }
   bes->SetCheckCRC(kTRUE);
   
   // stop finder
   TSnStopFindRms* sfr = new TSnStopFindRms("StopFindRmsMod",dbn.Data());
   sfr->SetSearchWidth(stopSearchWin);
   sfr->SetMaxJitter(jitter);
   
   // shift data by stops
   TString shiftNm(Form("Shifted%s",dbn.Data()));
   TSnShiftCalWvDataMod* scd = new TSnShiftCalWvDataMod("ShiftDataByStops",
                                                        dbn.Data(),
                                                        shiftNm.Data());
   
   // make stop shift tree
   TSnMakeShiftTree* mst = new TSnMakeShiftTree("MakeShiftTree",
                                                "StopShift",
                                "Tree with stop position and wrapped waveforms",
                                                dbn.Data(),
                                                shiftNm.Data());
   
   bes->Add(sfr);
   sfr->Add(scd);
   scd->Add(mst);
   
   // open output file before Process so maybe the tree can be cached
   outf = TFile::Open(outfn, "recreate");
   outf->SetCompressionLevel(2);
   outf->cd();
   Printf("starting ...");
   sel->AddInput(bes);
   nt->Process(sel);//, "",10);
   Printf("process done!");
   
   TList* output = sel->GetModOutput();
   if (output!=0) {
      outf->cd();
      output->Write();
   }
   
   outf->Close();
   delete outf; outf=0;
                                                
   
}
