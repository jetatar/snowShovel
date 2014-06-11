#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRawTreeMaker.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnFPNCalcMod.h"

#endif

TChain* nt(0);
TFile* outf;

void calcFPN(const Char_t* infn,
             const Char_t* outfn,
             const Bool_t  useGausFit=kFALSE,
             const Bool_t  useThermals=kFALSE,
             const Bool_t  useForced=kTRUE,
             const Char_t* fpnsetname="FPNSet",
             const Bool_t  checkCRC=kTRUE,
             const UInt_t  verbosity=0) {
   
   delete nt;
   nt = new TChain(TSnRawTreeMaker::kETname);
   nt->Add(infn);
   
   // loop through the chain using TAM
   TAMSelector* sel = new TAMSelector;
   
   // basic event selection: triggers and raw waveform CRC
   TSnBasicEvtSelMod* besmod = new TSnBasicEvtSelMod("BasicEvtSelMod");
   if (useThermals) {
      besmod->GetTrgBits().EnableTrig(TSnTrgInfo::kThermal);
   }
   if (useForced) {
      besmod->GetTrgBits().EnableTrig(TSnTrgInfo::kForced);
   }
   besmod->SetCheckCRC(checkCRC);
   
   // the module to calculate the FPN
   TSnFPNCalcMod* fpnmod = new TSnFPNCalcMod("FPNCalcMod", fpnsetname);
   fpnmod->SetGausFit(useGausFit);
   
   // put the FPN mod inside the event selection
   besmod->Add(fpnmod);
   
   // set the verbosity
   besmod->SetAllVerbosity(verbosity);
   
   // add the modules to the selector
   sel->AddInput(besmod);
   
   // process the events and find the FPN
   Printf("Finding FPN...");
   nt->Process(sel, "");
   Printf("Processing events finished.");
   
   // get the limits
   TAMOutput* outLims = sel->GetModOutput();
   if (outLims!=0) {
      outLims->ls("C");
      
      outf = TFile::Open(outfn, "RECREATE");
      outf->cd();
      outLims->Write();
      delete outf; outf=0;

   } else {
      Printf("Could not get the output list from the selector.");
   }
   
   // cleanup
   delete sel; // this deletes the modules and output list too!
   
}
