#include "TSnLoadFromFileMod.h"

#include <TFile.h>
#include <TDirectory.h>

ClassImp(TSnLoadFromFileMod);


void TSnLoadFromFileMod::Begin() {
   TDirectory* od = gDirectory;
   TFile* inf = TFile::Open(fFilen.Data());
   if ( (inf!=0) && (inf->IsZombie()==kFALSE) ) {
      TObject* o = inf->Get(fObjnm.Data());
      if (o!=0) {
         PublishObj(o);
      } else {
         SendError(kAbortAnalysis, "Begin",
                   "Could not get object [%s] from file [%s]",
                   fObjnm.Data(), inf->GetName());
      }
   } else {
      SendError(kAbortAnalysis, "Begin",
                "Could not open file [%s]",
                fFilen.Data());
   }
   gDirectory = od;
}

void TSnLoadFromFileMod::Terminate() {
   TObject* o = RetractObj(fObjnm.Data());
   delete o;
}
