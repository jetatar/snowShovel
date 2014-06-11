#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TList.h>
#include <TParameter.h>
#include <TMath.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnFindEvtNumLimitsMod.h"
#include "TSnRawTreeMaker.h"
#endif

TChain* nt(0);

void findEventNumAxis(Int_t&         enbins,
                      Double_t&      enmin,
                      Double_t&      enmax,
                      const Char_t*  infn,
                      const Char_t*  treename=TSnRawTreeMaker::kETname,
                      const UInt_t   verbosity=0) {
   // enbins, enmin, enmax = event number axis parameters
   // infn = string to add file(s) to a TChain
   
   delete nt;
   nt = new TChain(treename);
   nt->Add(infn);
   
   TSnFindEvtNumLimitsMod* elmod = 
      new TSnFindEvtNumLimitsMod("FindEvtNumLimsMod");
   
   // the selector for the first loop through events to find the time limits
   TAMSelector* selLims = new TAMSelector;
   
   // set the verbosity
   elmod->SetAllVerbosity(verbosity);
   
   // add them to the selector
   selLims->AddInput(elmod);
   
   // do the first loop
   Printf("Finding evt num limits...");
   nt->Process(selLims);
   Printf("Processing events finished.");
   
   // get the limits
   TAMOutput* outLims = selLims->GetModOutput();
   if (outLims!=0) {
      outLims->ls("C");
      // now get the output from tlmod, which we know will be a TParameterD
      const TParameter<UInt_t>* minen = 
         dynamic_cast< const TParameter<UInt_t>* >(
            outLims->FindOutput(elmod->GetName(), elmod->GetMinEvtNumName()) );
      const TParameter<UInt_t>* maxen = 
         dynamic_cast< const TParameter<UInt_t>* >(
            outLims->FindOutput(elmod->GetName(), elmod->GetMaxEvtNumName()) );
      if ((minen!=0) && (maxen!=0)) {
         
         enmin = minen->GetVal() - 0.5;
         enmax = maxen->GetVal() + 0.5;
         enbins = TMath::Ceil(enmax - enmin);
         
         Printf("Found evt num binning: %d bins over (%12.12g, %12.12g)",
                enbins, enmin, enmax);
      } else {
         Printf("Could not get min/max evt num from output list.");
      }
   } else {
      Printf("Could not get the output list from the limits selector.");
   }

   // cleanup
   delete selLims; // this deletes the selector's modules and output list too!

}
