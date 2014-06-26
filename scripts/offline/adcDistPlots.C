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
#include "TSnRejectEarlyEvtsMod.h"
#include "TSnConfigTreeLoader.h"
#endif

#include "findTimeAxis.C"

TChain* dt(0);

void adcDistPlots(const Char_t* infn,
                  const Char_t* outfn,
                  const Int_t    ntbins,
                  const Double_t tmin,
                  const Double_t tmax,
                  const Bool_t  makeFftPlots=kTRUE,
                  const Bool_t  rejectEarlyEvts=kTRUE,
                  const Bool_t  rejectBadClocks=kTRUE,
                  const Bool_t  checkCRC=kTRUE,
                  const UInt_t  verbosity=0,
                  const Bool_t  flattenOutput=kTRUE);

void adcDistPlots(const Char_t* infn,
                  const Char_t* outfn,
                  const Bool_t  makeFftPlots=kTRUE,
                  const Bool_t  rejectEarlyEvts=kTRUE,
                  const Bool_t  rejectBadClocks=kTRUE,
                  const Bool_t  checkCRC=kTRUE,
                  const UInt_t  verbosity=0,
                  const Bool_t  flattenOutput=kTRUE) {
   
   Int_t ntbins(0);
   Double_t tmin(0), tmax(0);
   findTimeAxis(ntbins, tmin, tmax, infn,
                rejectBadClocks, kTRUE, kTRUE,
                checkCRC, verbosity);
   
   adcDistPlots(infn, outfn, ntbins, tmin, tmax, makeFftPlots,
                rejectEarlyEvts, rejectBadClocks, checkCRC, verbosity,
                flattenOutput);
   
}

void adcDistPlots(const Char_t* infn,
                  const Char_t* outfn,
                  const Int_t    ntbins,
                  const Double_t tmin,
                  const Double_t tmax,
                  const Bool_t  makeFftPlots,
                  const Bool_t  rejectEarlyEvts,
                  const Bool_t  rejectBadClocks,
                  const Bool_t  checkCRC,
                  const UInt_t  verbosity,
                  const Bool_t  flattenOutput) {
   
   TString datbrnm;
   const Bool_t ok = tryLoadDataTree(infn, dt, &datbrnm);
   if (ok==kFALSE) {
      Printf("Could not load data from [%s].",infn);
      return;
   }
   Printf("Using time binning: %d bins over (%12.12g, %12.12g)",
          ntbins, tmin, tmax);
   
   // binning to use in case we're not making raw plots
   const Int_t adcbins = 1001;
   const Float_t adcmin = -1001;
   const Float_t adcmax = 1001;
   
   // loop through the chain using TAM
   TAMSelector* sel = new TAMSelector;
   sel->SetVerbosity(verbosity);
   
   // add a config loader in case we want to use trig start times
   TSnConfigTreeLoader* confload = new TSnConfigTreeLoader;
   sel->AddLoader(confload);
   
   // two ADC plot modules will be used, each inside a different
   // event selection: one for forced triggers and one for thermal triggers.
   
   // forced triggers
   TSnBasicEvtSelMod* frcmod = new TSnBasicEvtSelMod("FrcTrgSelMod");
   frcmod->GetTrgBits().EnableTrig(TSnTrgInfo::kForced);
   frcmod->SetCheckCRC(checkCRC);
   TSnRejectBadClocksMod* fbcmod = (rejectBadClocks) ?
      new TSnRejectBadClocksMod("FrcRejBadClocksMod")   : 0;
   TSnRejectEarlyEvtsMod* fremod = (rejectEarlyEvts) ?
      new TSnRejectEarlyEvtsMod("FrcRejEarlyEvtsMod")   : 0;
   TSnPlotAdcDists* fpamod = new TSnPlotAdcDists("FrcAdcPlotMod",
      ntbins, tmin, tmax, "forced trg","Frc", datbrnm.Data());
   fpamod->SetMakeFFTPlots(makeFftPlots);
   if (fpamod->IsDataBrRaw()==kFALSE) {
      fpamod->SetNAdcBins(adcbins);
      fpamod->SetAdcMin(adcmin);
      fpamod->SetAdcMax(adcmax);
   }
   TAModule* mod = frcmod;
   if (fbcmod!=0) {
      mod->Add(fbcmod);
      mod = fbcmod;
   }
   if (fremod!=0) {
      mod->Add(fremod);
      mod = fremod;
   }
   mod->Add(fpamod);
   frcmod->SetAllVerbosity(verbosity);
   sel->AddInput(frcmod);
   
   // thermal triggers
   TSnBasicEvtSelMod* thmmod = new TSnBasicEvtSelMod("ThmTrgSelMod");
   thmmod->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   thmmod->SetCheckCRC(checkCRC);
   TSnRejectBadClocksMod* tbcmod = (rejectBadClocks) ?
      new TSnRejectBadClocksMod("ThmRejBadClocksMod")   : 0;
   TSnRejectEarlyEvtsMod* tremod = (rejectEarlyEvts) ?
      new TSnRejectEarlyEvtsMod("ThmRejEarlyEvtsMod")   : 0;
   TSnPlotAdcDists* tpamod = new TSnPlotAdcDists("ThmAdcPlotMod",
      ntbins, tmin, tmax, "thermal trg","Thm", datbrnm.Data());
   tpamod->SetMakeFFTPlots(makeFftPlots);
   if (tpamod->IsDataBrRaw()==kFALSE) {
      tpamod->SetNAdcBins(adcbins);
      tpamod->SetAdcMin(adcmin);
      tpamod->SetAdcMax(adcmax);
   }
   mod = thmmod;
   if (tbcmod!=0) {
      mod->Add(tbcmod);
      mod = tbcmod;
   }
   if (tremod!=0) {
      mod->Add(tremod);
      mod = tremod;
   }
   mod->Add(tpamod);
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
