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
#include "TSnBasicEvtSelMod.h"
#include "TSnRejectBadClocksMod.h"
#include "TSnFindTimeLimitsMod.h"
#include "TSnRawTreeMaker.h"

#endif

#include "loadtree.C"

TChain* nt(0);

void findTimeAxis(Int_t&         ntbins,
                  Double_t&      tmin,
                  Double_t&      tmax,
                  const Char_t*  infn,
                  const Bool_t   rejectBadClocks=kTRUE,
                  const Bool_t   useThermals=kTRUE,
                  const Bool_t   useForced=kFALSE,
                  const Bool_t   checkCRC=kTRUE,
                  const UInt_t   verbosity=0) {
   // ntbins, tmin, tmax = time axis parameters set by this function
   // infn = string to add file(s) to a TChain
   // rejectBadClocks = if true, reject events with unreasonable times
   //                   if false, do not apply an evt time event selection
   // useThermals = if true, include thermal triggers in the plots.
   // useForced   = if true, include forced triggers in the plots.
   // verbosity   = verbosity level to set modules on. setting 15 usually gets
   //               lots of output (i.e. see why events fail the selection)
   
   const Bool_t ok = tryLoadDataTree(infn, nt);
   if (ok==kFALSE) {
      Printf("Could not load data from [%s].",infn);
      return;
   }
   
   // basic event selection: triggers and raw waveform CRC
   TSnBasicEvtSelMod* besmod = new TSnBasicEvtSelMod("BasicEvtSelMod");
   if (useThermals) {
      besmod->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   }
   if (useForced) {
      besmod->GetTrgBits().EnableTrig(TSnTrgInfo::kForced);
   }
   besmod->SetCheckCRC(checkCRC);
   // reject events with crazy times
   TSnRejectBadClocksMod* bcmod = (rejectBadClocks) ?
      new TSnRejectBadClocksMod("RejBadClocksMod")  : 0;
   // find the min/max of the event times
   TSnFindTimeLimitsMod* tlmod = new TSnFindTimeLimitsMod("FindTimeLimsMod");
   
   // the selector for the first loop through events to find the time limits
   TAMSelector* selLims = new TAMSelector;
   
   // build the module hierarchy.
   // if a module skips its event, its sub-modules will too.
   // therefore, any event selection modules can only work if we add
   // modules to them:
   if (bcmod!=0) {
      //   + BasicEvtSelMod
      //   +-> RejectBadClocksMod
      //     +-> FindTimeLimitsMod
      besmod->Add(bcmod);
      bcmod->Add(tlmod);
   } else {
      //   + BasicEvtSelMod
      //   +-> FindTimeLimitsMod
      besmod->Add(tlmod);
   }

   // set the verbosity
   besmod->SetAllVerbosity(verbosity);
   
   // add them to the selector
   selLims->AddInput(besmod);

   // do the first loop
   Printf("Finding time limits...");
   nt->Process(selLims);
   Printf("Processing events finished.");
   
   // get the limits
   TAMOutput* outLims = selLims->GetModOutput();
   if (outLims!=0) {
      outLims->ls("C");
      // now get the output from tlmod, which we know will be a TParameterD
      const TParameter<Double_t>* mintime = 
         dynamic_cast< const TParameter<Double_t>* >(
            outLims->FindOutput(tlmod->GetName(), tlmod->GetMinTimeName()) );
      const TParameter<Double_t>* maxtime = 
         dynamic_cast< const TParameter<Double_t>* >(
            outLims->FindOutput(tlmod->GetName(), tlmod->GetMaxTimeName()) );
      if ((mintime!=0) && (maxtime!=0)) {
         // get time bins
         TSnMath::GetNiceDateAxisFor(mintime->GetVal(),
                                     maxtime->GetVal(),
                                     ntbins, tmin, tmax);
         if ((tmax-tmin) < (3600.*24.)) {
            // if less than a day, use 5 min bins
            ntbins = TMath::Max(100.0, (tmax-tmin)/(300.0));
         } else if ((tmax-tmin) < (3600.*24.*5.)) {
            // less than 5 days, use hourly bins
            ntbins = TMath::Max(100.0, (tmax-tmin)/3600.0);
         }
         
         Printf("Found time binning: %d bins over (%12.12g, %12.12g)",
                ntbins, tmin, tmax);
      } else {
         Printf("Could not get min/max time from output list.");
      }
   } else {
      Printf("Could not get the output list from the limits selector.");
   }

   // cleanup
   delete selLims; // this deletes the selector's modules and output list too!

}
