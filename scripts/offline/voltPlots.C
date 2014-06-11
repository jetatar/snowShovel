#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TList.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRejectBadVoltTimesMod.h"
#include "TSnPlotPowerReadingsMod.h"
#include "TSnRawTreeMaker.h"
#endif

#include "loadtree.C"

TChain* rt(0);

void voltPlots(const Char_t*  infn,
               const Char_t*  outfn,
               const Bool_t   rejectBadClocks=kTRUE,
               const UInt_t   verbosity=0,
               const Bool_t   flattenOutput=kTRUE) {
   // infn = string to add file(s) to a TChain
   // outfn = pull path to output file, storing hists
   // rejectBadClocks = if true, reject events with unreasonable times
   //                   if false, do not apply an evt time event selection
   // verbosity   = verbosity level to set modules on. setting 15 usually gets
   //               lots of output (i.e. see why events fail the selection)
   // flattenOutput = if true, save all output objects directly to the file;
   //                 if false, save them in folders corresponding to the module
   //                 hierarchy (easy browsing in TBrowser after
   //                 libtam is loaded)

   const Bool_t ok = tryLoadTree(TSnRawTreeMaker::kVTname, infn, rt);
   if (ok==kFALSE) {
      Printf("Could not load [%s] from [%s].",TSnRawTreeMaker::kVTname,infn);
      return;
   }
   
   // loop through the chain using TAM
   TAMSelector* sel = new TAMSelector;

   TSnRejectBadVoltTimesMod* bcmod = (rejectBadClocks) ?
      new TSnRejectBadVoltTimesMod("RejBadVoltTimeMod")   : 0;
   
   TSnPlotPowerReadingsMod* pprmod = new TSnPlotPowerReadingsMod("PlotPwrMod");
   
   // build the module hierarchy
   TAModule* topmod = (rejectBadClocks) ? static_cast<TAModule*>(bcmod) 
      : static_cast<TAModule*>(pprmod);
   if (rejectBadClocks) {
      bcmod->Add(pprmod);
   }

   // set the verbosity
   topmod->SetAllVerbosity(verbosity);
   sel->SetVerbosity(verbosity);
   
   // add the whole thing to the selector
   sel->AddInput(topmod);
   
   // loop over the events!
   Printf("Plotting power readings...");
   rt->Process(sel);
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
