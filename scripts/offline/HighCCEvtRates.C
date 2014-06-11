#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TList.h>
#include <TParameter.h>
#include <TMath.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnMath.h"
#include "TSnConfigTreeLoader.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnRejectBadClocksMod.h"
#include "TSnFindTimeLimitsMod.h"
#include "TSnCalcAbsTimeMod.h"
#include "TSnPlotEvtRateMod.h"
#include "TSnRawTreeMaker.h"

#endif

#include "findTimeAxis.C"

TChain* rt(0);

void eventRates(const Char_t*  infn,
                const Char_t*  outfn,
                const Int_t    ntbins,
                const Double_t tmin,
                const Double_t tmax,
                const Double_t twin=-1,
                const Bool_t   rejectBadClocks=kTRUE,
                const Bool_t   useThermals=kTRUE,
                const Bool_t   useForced=kFALSE,
                const Bool_t   checkCRC=kTRUE,
                const UInt_t   verbosity=0,
                const Bool_t   flattenOutput=kTRUE);

void eventRates(const Char_t*  infn,
                const Char_t*  outfn,
                const Double_t twin=-1,
                const Bool_t   rejectBadClocks=kTRUE,
                const Bool_t   useThermals=kTRUE,
                const Bool_t   useForced=kFALSE,
                const Bool_t   checkCRC=kTRUE,
                const UInt_t   verbosity=0,
                const Bool_t   flattenOutput=kTRUE) {
   // use the macro findTimeAxis.C to find an appropriate time axis
   // before finding the event rates. this loops through the chain twice.
   //
   // infn = string to add file(s) to a TChain
   // outfn = pull path to output file, storing hists
   // twin = time window (see TSnPlotEvtRateMod). if <=0, use module's default
   // rejectBadClocks = if true, reject events with unreasonable times
   //                   if false, do not apply an evt time event selection
   // useThermals = if true, include thermal triggers in the plots.
   // useForced   = if true, include forced triggers in the plots.
   // checkCRC    = if true, skip events in which the station and local CRC
   //               of the raw data waveforms don't match
   // verbosity   = verbosity level to set modules on. setting 15 usually gets
   //               lots of output (i.e. see why events fail the selection)
   // flattenOutput = if true, save all output objects directly to the file;
   //                 if false, save them in folders corresponding to the module
   //                 hierarchy (easy browsing in TBrowser after
   //                 libtam is loaded)
   
   Int_t ntbins(0);
   Double_t tmin(0), tmax(0);
   findTimeAxis(ntbins, tmin, tmax, infn,
                rejectBadClocks, useThermals, useForced,
                checkCRC, verbosity);
   
   eventRates(infn, outfn, ntbins, tmin, tmax, twin,
              rejectBadClocks, useThermals, useForced,
              checkCRC, verbosity, flattenOutput);

}

void eventRates(const Char_t*  infn,
                const Char_t*  outfn,
                const Int_t    ntbins,
                const Double_t tmin,
                const Double_t tmax,
                const Double_t twin,
                const Bool_t   rejectBadClocks,
                const Bool_t   useThermals,
                const Bool_t   useForced,
                const Bool_t   checkCRC,
                const UInt_t   verbosity,
                const Bool_t   flattenOutput) {
   // infn = string to add file(s) to a TChain
   // outfn = pull path to output file, storing hists
   // ntbins, tmin, tmax = binning for time axis of plots
   // twin = time window (see TSnPlotEvtRateMod). if <=0, use module's default
   // rejectBadClocks = if true, reject events with unreasonable times
   //                   if false, do not apply an evt time event selection
   // useThermals = if true, include thermal triggers in the plots.
   // useForced   = if true, include forced triggers in the plots.
   // checkCRC    = if true, skip events in which the station and local CRC
   //               of the raw data waveforms don't match
   // verbosity   = verbosity level to set modules on. setting 15 usually gets
   //               lots of output (i.e. see why events fail the selection)
   // flattenOutput = if true, save all output objects directly to the file;
   //                 if false, save them in folders corresponding to the module
   //                 hierarchy (easy browsing in TBrowser after
   //                 libtam is loaded)

   const Bool_t ok = tryLoadDataTree(infn, rt);
   if (ok==kFALSE) {
      Printf("Could not load data from [%s].",infn);
      return;
   }
   
   Printf("Using time binning: %d bins over (%12.12g, %12.12g)",
          ntbins, tmin, tmax);
   
   // loop through the chain using TAM
   TAMSelector* selRates = new TAMSelector;
   
   // need a special loader to get the trigger clock start/stop from
   // the config tree. it will get used automagically when the
   // module(s) use ReqBranch/LoadBranch for branches in the config
   // tree. the loader will check that we get the right config info
   // for the event we're currently processing
   TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
   selRates->AddLoader(confLoader);

   // trying to calculate better than 1 second resolution on the event time
   // this must module NEVER be included in any event selection, as it
   // adds up delta-t's between all events in the file!
   TSnCalcAbsTimeMod* catmod = new TSnCalcAbsTimeMod("AbsTimeMod");

   // the event selection modules
   TSnBasicEvtSelMod* rbesmod = new TSnBasicEvtSelMod("BasicEvtSelMod");
   if (useThermals) {
      rbesmod->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   }
   if (useForced) {
      rbesmod->GetTrgBits().EnableTrig(TSnTrgInfo::kForced);
   }
   rbesmod->SetCheckCRC(checkCRC);
   TSnRejectBadClocksMod* rbcmod = (rejectBadClocks) ?
      new TSnRejectBadClocksMod("RejBadClocksMod")   : 0;
   // for plotting the rates and time differences
   TSnPlotEvtRateMod* ermod = new TSnPlotEvtRateMod("EvtRateMod");
   if (twin>0) {
      ermod->SetTwin(twin);
   }
   ermod->SetNtimeBins(ntbins);
   ermod->SetTimeMin(tmin);
   ermod->SetTimeMax(tmax);
   ermod->SetAbsEvtTimeName(catmod->GetAbsEvtTimeName());
   // leave the rest of the options at their default
   
   // build the module hierarchy
   catmod->Add(rbesmod);
   if (rbcmod!=0) {
      rbesmod->Add(rbcmod);
      rbcmod->Add(ermod);
   } else {
      rbesmod->Add(ermod);
   }
   
   // set the verbosity
   rbesmod->SetAllVerbosity(verbosity);
   selRates->SetVerbosity(verbosity);
   
   // add the whole thing to the selector
   selRates->AddInput(catmod);

   // loop over the events!
   Printf("Finding event rates...");
   rt->Process(selRates);
   
   Printf("Done.");
   
   // save output
   TList* outRates = selRates->GetModOutput();
   if (outRates!=0) {
      TFile* outf = TFile::Open(outfn,"recreate");
      if ( (outf!=0) && (outf->IsZombie()==kFALSE) ) {
         if (flattenOutput) {
            outRates->Write();
         } else {
            outRates->Write(outRates->GetName(),
                            TObject::kSingleKey);
         }
         Printf("Wrote output to [%s].", outf->GetName());
      } else {
         Printf("Could not open output file.");
      }
      
      delete outf;
      
      
   } else {
      Printf("Could not get output list from rates selector.");
   }
   
   // cleanup
   delete confLoader;
   delete selRates; // this deletes the selector's modules and output
                    // list too!

}

