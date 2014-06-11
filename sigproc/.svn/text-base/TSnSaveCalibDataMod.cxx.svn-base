#include "TSnSaveCalibDataMod.h"

#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>
#include <TClass.h>
#include <TKey.h>

#include "TSnRawTreeMaker.h"
#include "TSnCalWvData.h"

ClassImp(TSnSaveCalibDataMod);

const Char_t* TSnSaveCalibDataMod::kFTFprf        = "FPNSubTree.";
const Char_t* TSnSaveCalibDataMod::kFpnEvtTreeNm  = "FpnSubTree";
const Char_t* TSnSaveCalibDataMod::kCTFprf        = "CalTree.";
const Char_t* TSnSaveCalibDataMod::kCalEvtTreeNm  = "CalibTree";
const Char_t* TSnSaveCalibDataMod::kFPNSubDatBrNm = "FPNSubData.";
const Char_t* TSnSaveCalibDataMod::kAmpOutDatBrNm = "AmpOutData.";

TSnSaveCalibDataMod::TSnSaveCalibDataMod(const Char_t* name,
                                         const Char_t* outdir,
                                         const Char_t* outprf,
                                         const Char_t* fpnsubnm,
                                         const Char_t* ampoutnm,
                                         const Int_t   bufsz,
                                         const Int_t   split)
   : TAModule(name, "save calibrated data"),
     fFPNSubDatNm(fpnsubnm), fSaveFPNSub(kTRUE),
     fAmpOutDatNm(ampoutnm), fSaveAmpOut(kTRUE),
     fOutdir(outdir), fOutprf(outprf), fBufsz(bufsz), fSplit(split),
     fOutTreeTitle("(semi-)calibrated waveform events"),
     fFPNSubDat(new TSnCalWvData(fpnsubnm,"FPN subtracted data")),
     fAmpOutDat(new TSnCalWvData(ampoutnm,"digitizer calib data")),
     fOt(0), fOutf(0), fFPNSubBr(0), fAmpOutBr(0) {
}

TSnSaveCalibDataMod::~TSnSaveCalibDataMod() {
   delete fFPNSubDat;
   delete fAmpOutDat;
}

TString TSnSaveCalibDataMod::GetTreeFilename(const TString infn) const {
   TString ifn = infn.Data() + infn.Last('/') + 1;
   TString o(Form("%s/%s%s",
                  fOutdir.Data(),fOutprf.Data(),ifn.Data()));
   o.Remove(o.Last('.'));
   o += ".root";
   return o;
}

void TSnSaveCalibDataMod::CopyAllKeys(const TFile* inf) {
   // copy all the keys of the input file into the output file no
   // checks are made here on inf or fOutf (should be done before
   // calling this function)
   //
   // this function changes gDirectory
   
   TList* keys = inf->GetListOfKeys();
   TIter nk(keys);
   TKey* k;
   TObject* o;
   TClass* c;
   while ( (k=dynamic_cast<TKey*>(nk()))!=0 ) {
      o = k->ReadObj();
      if (o!=0) {
         c = o->IsA();
         if (c!=0) {
            if (c->InheritsFrom("TTree")) {
               TTree* t  = dynamic_cast<TTree*>(o);
               TTree* ct = t->CloneTree(-1, "fast");
               ct->SetDirectory(fOutf);
               TString tn(ct->GetName());
               if (tn.CompareTo(TSnRawTreeMaker::kETname)==0) {
                  fOt = ct; // this is our output tree
                  fOt->SetTitle(fOutTreeTitle.Data());
               } else if ( (fSaveFPNSub==kFALSE) &&
                           (tn.CompareTo(kFpnEvtTreeNm)==0) ) {
                  fOt = ct; // this is our output tree
                  fOt->SetTitle(fOutTreeTitle.Data());
               } else {
                  fOutf->cd();
                  ct->Write();
                  delete ct;
               }
            } else {
               fOutf->cd();
               o->Write();
            }
         } else {
            SendError(kAbortAnalysis, "CopyAllKeys",
                      "Could not get class for object [%s].", o->GetName());
         }
      } else {
         SendError(kAbortAnalysis, "CopyAllKeys",
                   "Could not read object of class [%s] from key [%s].",
                   k->GetClassName(), k->GetName());
      }
      
      delete o;

   } // end loop over keys
}

Bool_t TSnSaveCalibDataMod::Notify() {
   // first copy everything in the file
   // then make a new event tree based on the old one, but with extra branches

   Bool_t ok = kTRUE;

   TFile* cf = GetCurrentFile();
   if (cf!=0) {
      TDirectory* dir = gDirectory;
      delete fOutf;
      TString ofn( GetTreeFilename(cf->GetName()) );
      fOutf = TFile::Open(ofn.Data(),"RECREATE");
      if ( (fOutf!=0) && (fOutf->IsZombie()==kFALSE) ) {
         fOutf->SetCompressionLevel( cf->GetCompressionLevel() );
         
         if (GetVerbosity()>5) {
            Info("Notify","Created output file [%s]",fOutf->GetName());
         }
         
         CopyAllKeys(cf);
         if (fOt!=0) { // made inside CopyAllKeys (hopefully)
            if (fSaveFPNSub) {
               fFPNSubBr = fOt->Branch(kFPNSubDatBrNm, &fFPNSubDat,
                                       fBufsz, fSplit);
               fOt->SetName(kFpnEvtTreeNm);
            }
            if (fSaveAmpOut) {
               fAmpOutBr = fOt->Branch(kAmpOutDatBrNm, &fAmpOutDat,
                                       fBufsz, fSplit);
               fOt->SetName(kCalEvtTreeNm);
            }
         } else {
            SendError(kAbortAnalysis, "Notify",
                      "Could not make output tree.");
            ok = kFALSE;
         }
      } else {
         SendError(kAbortAnalysis, "Notify",
                   "Could not open output file [%s]",
                   ofn.Data());
         ok = kFALSE;
      }
      
      // put the directory back
      gDirectory = dir;
      
   } else {
      // can't call SendError here as Notify always seems to get
      // called once before there's actually a file loaded
      ok = kFALSE;
   }
   
   return ok;
}

void TSnSaveCalibDataMod::Process() {

   if (fSaveFPNSub) {
      const TSnCalWvData* psdat =
         dynamic_cast<const TSnCalWvData*>(
            FindObjThisEvt(fFPNSubDatNm.Data()));
      if (psdat!=0) {
         fFPNSubDat->CopyDataFrom(*psdat);
         fFPNSubBr->Fill();
      } else {
         // end all further processing. we can't miss one event or the
         // trees will get out of sync.
         SendError(kStopModule, "Process",
                   "Could not get FPN subracted data obj named [%s]",
                   fFPNSubDatNm.Data());
      }
   }

   if (fSaveAmpOut) {
      const TSnCalWvData* aodat =
         dynamic_cast<const TSnCalWvData*>(
            FindObjThisEvt(fAmpOutDatNm.Data()));
      if (aodat!=0) {
         fAmpOutDat->CopyDataFrom(*aodat);
         fAmpOutBr->Fill();
      } else {
         // end all further processing. we can't miss one event or the
         // trees will get out of sync.
         SendError(kStopModule, "Process",
                   "Could not get digitizer calib data obj named [%s]",
                   fAmpOutDatNm.Data());
      }
   }

}

void TSnSaveCalibDataMod::SlaveTerminate() {
   // write out tree
   if (fOutf!=0) {
      fOutf->cd();
      fOt->Write();
      delete fOutf;
   } else {
      SendError(kStopModule, "SlaveTerminate",
                "No output file.");
   }
}
