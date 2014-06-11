#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnBasicEvtSelMod.h"
#include "TSnPlotWaveformsMod.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnTrgInfo.h"
#endif

#include "findEventNumAxis.C"

TChain* ct(0);
TFile* outf(0);

void waveformPlots(const Char_t* infn,
                   const Char_t* outdir,
                   const Int_t   nebins,
                   const Double_t emin,
                   const Double_t emax,
                   const Char_t* treename=TSnSaveCalibDataMod::kCalEvtTreeNm,
                   const Bool_t  doAdcPlots=kTRUE,
                   const Bool_t  doFpnSubPlots=kTRUE,
                   const Bool_t  doAmpOutPlots=kTRUE,
                   const Bool_t  useThermals=kTRUE,
                   const Bool_t  useForced=kTRUE,
                   const Bool_t  rejectAdcToMB=kFALSE,
                   const Bool_t  checkCRC=kFALSE,
                   const UInt_t  verbosity=11,
                   const Char_t* outtag="WvPlots.");

void waveformPlots(const Char_t* infn,
                   const Char_t* outdir,
                   const Char_t* treename=TSnSaveCalibDataMod::kCalEvtTreeNm,
                   const Bool_t  doAdcPlots=kTRUE,
                   const Bool_t  doFpnSubPlots=kTRUE,
                   const Bool_t  doAmpOutPlots=kTRUE,
                   const Bool_t  useThermals=kTRUE,
                   const Bool_t  useForced=kTRUE,
                   const Bool_t  rejectAdcToMB=kFALSE,
                   const Bool_t  checkCRC=kFALSE,
                   const UInt_t  verbosity=11,
                   const Char_t* outtag="WvPlots.") {
   // infn  = string to add file(s) to a TChain
   //         NOTE: do NOT chain multiple runs together!
   //               this breaks the assumption that event numbers are unique
   // outdir = directory in which to store output file
   // doAdcPlots = if true, make raw data waveform plots
   // doFpnSubPlots = if true, make FPN subtracted waveform plots
   // doAmpOutPlots = if true, make gain calibrated waveform plots
   // useThermals = if true, include thermal triggers in the plots.
   // useForced   = if true, include forced triggers in the plots.
   // checkCRC    = if true, skip events in which the station and local CRC
   //               of the raw data waveforms don't match
   
   Int_t nebins(0);
   Double_t emin(0), emax(0);
   findEventNumAxis(nebins, emin, emax, infn, treename, verbosity);
   waveformPlots(infn, outdir, nebins, emin, emax, treename,
                 doAdcPlots, doFpnSubPlots, doAmpOutPlots,
                 useThermals, useForced, rejectAdcToMB, checkCRC, 
                 verbosity, outtag);
}

void waveformPlots(const Char_t* infn,
                   const Char_t* outdir,
                   const Int_t   nebins,
                   const Double_t emin,
                   const Double_t emax,
                   const Char_t* treename,
                   const Bool_t  doAdcPlots,
                   const Bool_t  doFpnSubPlots,
                   const Bool_t  doAmpOutPlots,
                   const Bool_t  useThermals,
                   const Bool_t  useForced,
                   const Bool_t  rejectAdcToMB,
                   const Bool_t  checkCRC,
                   const UInt_t  verbosity,
                   const Char_t* outtag) {
   // infn  = string to add file(s) to a TChain
   //         NOTE: do NOT chain multiple runs together!
   //               this breaks the assumption that event numbers are unique
   // outdir = directory in which to store output file
   // nebins, emin, emax = parameters for the event number axis
   // doAdcPlots = if true, make raw data waveform plots
   // doFpnSubPlots = if true, make FPN subtracted waveform plots
   // doAmpOutPlots = if true, make gain calibrated waveform plots
   
   delete ct;
   ct = new TChain(treename);
   ct->Add(infn);
   
   Printf("Using event num binning: %d bins over (%12.12g, %12.12g)",
          nebins, emin, emax);
   
   // loop through the chain using TAM
   TAMSelector* sel = new TAMSelector;
   
   // the event selection module(s)
   TSnBasicEvtSelMod* besmod = new TSnBasicEvtSelMod("BasicEvtSelMod");
   if (useThermals) {
      besmod->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   }
   if (useForced) {
      besmod->GetTrgBits().EnableTrig(TSnTrgInfo::kForced);
   }
   if (rejectAdcToMB) {
      besmod->GetRejectTrgBits().EnableTrig(TSnTrgInfo::kAdcToMBflag);
   } else {
      besmod->GetRejectTrgBits().DisableTrig(TSnTrgInfo::kAdcToMBflag);
   }
   besmod->SetCheckCRC(checkCRC);
   
   // to make the plots
   TSnPlotWaveformsMod* pwmod = 
      new TSnPlotWaveformsMod("PlotWaveformsMod", nebins, emin, emax);
   pwmod->SetMakeAdcPlots(doAdcPlots);
   pwmod->SetMakeFpnSubPlots(doFpnSubPlots);
   pwmod->SetMakeAmpOutPlots(doAmpOutPlots);
   
   // build the module hierarchy
   besmod->Add(pwmod);
   
   // set the verbosity
   besmod->SetAllVerbosity(verbosity);
   sel->SetVerbosity(verbosity);
   
   // add the whole thing to the selector
   sel->AddInput(besmod);
   
   // loop over the events!
   Printf("Making waveform plots...");
   ct->Process(sel);

   Printf("Saving plots...");
   
   // save output
   TList* out = sel->GetModOutput();
   if (out!=0) {
      TString sinfn(infn);
      TString ifn = sinfn.Data() + sinfn.Last('/') + 1;
      TString outfn(Form("%s/%s%s",outdir,outtag,ifn.Data()));
      outf = TFile::Open(outfn.Data(),"recreate");
      if ( (outf!=0) && (outf->IsZombie()==kFALSE) ) {
         out->Write();
         Printf("Wrote output to [%s].", outf->GetName());
      } else {
         Printf("Could not open output file.");
      }
      delete outf;
   } else {
      Printf("Could not get output list from rates selector.");
   }
   
   // cleanup
   delete sel; // this deletes the selector's modules and output lists too!
   
   Printf("Done!");
   
}
