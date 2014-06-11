#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TList.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnConfigTreeLoader.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnRejectBadClocksMod.h"
#include "TSnFindTimeLimitsMod.h"
#include "TSnPlotAdcDists.h"
#include "TSnRawTreeMaker.h"

#endif

#include "findTimeAxis.C"

TChain* dt(0);

void adcDistPlots(const Char_t* infn,
                  const Char_t* outfn,
                  const Int_t    ntbins,
                  const Double_t tmin,
                  const Double_t tmax,
                  const Bool_t  makeFpnSubPlots=kTRUE,
                  const Bool_t  rejectBadClocks=kTRUE,
                  const Bool_t  checkCRC=kTRUE,
                  const UInt_t  verbosity=0,
                  const Bool_t  flattenOutput=kTRUE);

void adcDistPlots(const Char_t* infn,
                  const Char_t* outfn,
                  const Bool_t  makeFpnSubPlots=kTRUE,
                  const Bool_t  rejectBadClocks=kTRUE,
                  const Bool_t  checkCRC=kTRUE,
                  const UInt_t  verbosity=0,
                  const Bool_t  flattenOutput=kTRUE) {
   
   Int_t ntbins(0);
   Double_t tmin(0), tmax(0);
   findTimeAxis(ntbins, tmin, tmax, infn,
                rejectBadClocks, kTRUE, kTRUE,
                checkCRC, verbosity);
   
   adcDistPlots(infn, outfn, ntbins, tmin, tmax, makeFpnSubPlots,
                rejectBadClocks, checkCRC, verbosity,
                flattenOutput);
   
}

void adcDistPlots(const Char_t* infn,
                  const Char_t* outfn,
                  const Int_t    ntbins,
                  const Double_t tmin,
                  const Double_t tmax,
                  const Bool_t  makeFpnSubPlots,
                  const Bool_t  rejectBadClocks,
                  const Bool_t  checkCRC,
                  const UInt_t  verbosity,
                  const Bool_t  flattenOutput) {
   
   const Bool_t ok = tryLoadDataTree(infn, dt);
   if (ok==kFALSE) {
      Printf("Could not load data from [%s].",infn);
      return;
   }
   Printf("Using time binning: %d bins over (%12.12g, %12.12g)",
          ntbins, tmin, tmax);
   
   
   // loop through the chain using TAM
   TAMSelector* sel = new TAMSelector;
   sel->SetVerbosity(verbosity);
   
   // two ADC plot modules will be used, each inside a different
   // event selection: one for forced triggers and one for thermal triggers.
   
   // forced triggers
   TSnBasicEvtSelMod* frcmod = new TSnBasicEvtSelMod("FrcTrgSelMod");
   frcmod->GetTrgBits().EnableTrig(TSnTrgInfo::kForced);
   frcmod->SetCheckCRC(checkCRC);
   TSnRejectBadClocksMod* fbcmod = (rejectBadClocks) ?
      new TSnRejectBadClocksMod("FrcRejBadClocksMod")   : 0;
   TSnPlotAdcDists* fpamod = new TSnPlotAdcDists("FrcAdcPlotMod",
      ntbins, tmin, tmax, "forced trg","Frc");
   fpamod->SetMakeFpnSubPlots(makeFpnSubPlots);
   if (fbcmod!=0) {
      frcmod->Add(fbcmod);
      fbcmod->Add(fpamod);
   } else {
      frcmod->Add(fpamod);
   }
   frcmod->SetAllVerbosity(verbosity);
   sel->AddInput(frcmod);
   
   // thermal triggers
   TSnBasicEvtSelMod* thmmod = new TSnBasicEvtSelMod("ThmTrgSelMod");
   thmmod->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   thmmod->SetCheckCRC(checkCRC);
   TSnRejectBadClocksMod* tbcmod = (rejectBadClocks) ?
      new TSnRejectBadClocksMod("ThmRejBadClocksMod")   : 0;
   TSnPlotAdcDists* tpamod = new TSnPlotAdcDists("ThmAdcPlotMod",
      ntbins, tmin, tmax, "thermal trg","Thm");
   tpamod->SetMakeFpnSubPlots(makeFpnSubPlots);
   if (fbcmod!=0) {
      thmmod->Add(tbcmod);
      tbcmod->Add(tpamod);
   } else {
      thmmod->Add(tpamod);
   }
   thmmod->SetAllVerbosity(verbosity);
   sel->AddInput(thmmod);
   
   Printf("Making plots...");
   dt->Process(sel);
   Printf("Done.");
   
   // save output
   TList* out = sel->GetModOutput();
   if (out!=0) {
      TFile* outf = TFile::Open(outfn,"recreate");
      if ( (outf!=0) && (outf->IsZombie()==kFALSE) ) {
         if (flattenOutput) {
            out->Write();
         } else {
            out->Write(out->GetName(), TObject::kSingleKey);
         }
         Printf("Wrote output to [%s].", outf->GetName());
      } else {
         Printf("Could not open output file.");
      }
      
      delete outf;
      
   } else {
      Printf("Could not get output list from selector.");
   }
   
   // cleanup
   delete sel; // this deletes the selector's modules and output list too!
   
   
}
