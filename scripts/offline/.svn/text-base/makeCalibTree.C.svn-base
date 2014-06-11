#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TChain.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnFPNSubMod.h"
#include "TSnDigitizerCalibMod.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnRawTreeMaker.h"
#include "TSnLoadFromFileMod.h"
#endif

TChain* nt(0);
TFile* cfile, * pfile;

Bool_t verifyObjInFile(const Char_t* filen, const Char_t* objn) {
   // verify the fpn file
   Bool_t ok = kFALSE;
   TObject* o(0);
   TFile* fpnf = TFile::Open(filen);
   if ((fpnf!=0) && (fpnf->IsZombie()==kFALSE)) {
      o = fpnf->Get(objn);
      if (o!=0) {
         ok = kTRUE;
      } else {
         Printf("Could not get obj [%s] from file [%s].",
                objn, filen);
      }
   } else {
      Printf("Could not open file [%s].", filen);
   }
   delete fpnf;
   delete o;
   return ok;
}

void makeCalibTree(const Char_t* infn,
                   const Char_t* fpnFilen,
                   const Char_t* calFilen,
                   const Char_t* outdir,
                   const Char_t* fpnSetNm="FPNSet",
                   const Char_t* calSetNm="DigitizerCalibSet") {
   // infn = input filename containing the RawEventTree
   //        Note: only 1 input file; not a chain with wildcards
   // fpnFilen = input file containing the FPN values
   // calFilen = input file containing the calibration values (0 => no calib)
   // outdir = directory in which to store the output file
   // fpnSetNm = name of the TSnFPNSet inside fpnFilen
   // calSetNm = name of the calibration set inside calFilen

   // verify the fpn file
   const Bool_t fpnok = (fpnFilen!=0) ? verifyObjInFile(fpnFilen, fpnSetNm)
                                      : kFALSE;

   // verify the calib file
   const Bool_t calok = (calFilen!=0) ? verifyObjInFile(calFilen, calSetNm)
      : kFALSE;
   
   if ((fpnok==kFALSE) && (calok==kFALSE)) {
      Printf("Must have either FPN or gain calibrations.");
      return;
   }
   
   delete nt;
   nt = new TChain((fpnok) ? TSnRawTreeMaker::kETname
                           : TSnSaveCalibDataMod::kFpnEvtTreeNm );
   nt->Add(infn);

   Printf("Calibrating from [%s].",nt->GetName());

   if (nt->GetEntries()<=0) {
      Printf("Could not load the raw tree.");
      return;
   }
   
   // check if there's more than one file in the chain, as the
   // module probably won't work properly in this case
   TObjArray* files = nt->GetListOfFiles();
   if (files!=0) {
      if (1 != files->GetEntries()) {
         Printf("Expect only one tree file. Found [%d].",
                files->GetEntries());
         return;
      }
   } else {
      Printf("Could not get list of files from chain.");
      return;
   }
   
   // to loop through events
   TAMSelector* sel = new TAMSelector;
   
   // we must NOT use any event selection, as we want the output tree
   // to contain all original data!
   
   TSnLoadFromFileMod* loadfpnmod(0);
   TSnFPNSubMod* fpnmod(0);
   if (fpnok) {
   
      // to load the fpn
      loadfpnmod = new TSnLoadFromFileMod("LoadFPN", fpnFilen, fpnSetNm);
   
      // to subtract FPN
      fpnmod = new TSnFPNSubMod("FPNSubMod", fpnSetNm);
      
   }

   TSnLoadFromFileMod* loadcalmod(0);
   TSnDigitizerCalibMod* calmod(0);
   if (calok) {
         
      // to load the gain calibrations
      loadcalmod = new TSnLoadFromFileMod("LoadDigiCalibs", calFilen, calSetNm);
      // to apply gain calibrations
      calmod = new TSnDigitizerCalibMod("DigiCalibMod", calSetNm);
   }
      
   // to make the output file and save all the data
   // (this module is probably not safe to run with Proof)
   TSnSaveCalibDataMod* savemod =
      new TSnSaveCalibDataMod("SaveCalibMod", outdir);
   savemod->SetVerbosity(10);
   if (fpnok) {
      savemod->SetOutputPrefix(TSnSaveCalibDataMod::kFTFprf);
      savemod->SetSaveFPNSubData(kTRUE);
      savemod->SetSaveAmpOutData(kFALSE);
   } else {
      // fpnok = false, calok = true .. no check on calok here because
      // that was done up top
      savemod->SetSaveFPNSubData(kFALSE);
      calmod->SetFpnSubFromTree(kTRUE);
      calmod->SetFPNSubDatNm(TSnSaveCalibDataMod::kFPNSubDatBrNm);
   }
   if (calok) {
      // this needs to be done if fpnok is true or false
      savemod->SetOutputPrefix(TSnSaveCalibDataMod::kCTFprf);
      savemod->SetSaveAmpOutData(kTRUE);
   }
      
      
   // module hierarchy
   // it doesn't really *have* to be this compilcated; we *could*
   // just do:
   //
   // if (fpnok) {
   //    sel->AddInput(loadfpnmod);
   //    sel->AddInput(fpnmod);
   // }
   // if (calok) {
   //    sel->AddInput(loadcalmod);
   //    sel->AddInput(calmod);
   // }
   // sel->AddInput(savemod);
   //
   // but then an "AbortModule" error in one module will not stop
   // subsequent modules (that may depend on its output)
   TAModule* topmod(0);
   if (fpnok) {
      topmod = loadfpnmod;
      loadfpnmod->Add(fpnmod);
      if (calok) {
         fpnmod->Add(loadcalmod);
         loadcalmod->Add(calmod);
         calmod->Add(savemod);
      } else {
         fpnmod->Add(savemod);
      }
   } else if (calok) {
      topmod = loadcalmod;
      loadcalmod->Add(calmod);
      calmod->Add(savemod);
   }

   // add to the selector
   sel->AddInput(topmod);
   
   // process the events and save the new tree(s)
   Printf("Processing data...");
   nt->Process(sel);
   Printf("Processing events finished.");
      
      
   // cleanup
   delete sel; // this deletes the modules and output list too!
      
}
