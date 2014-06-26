#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1.h>
#include <TVector3.h>
#include <TString.h>

#include "TSnRecoDirection.h"
#endif

const Int_t angresbins=200;
const Float_t angresmin=-2, angresmax=2;

TChain* mct, * ftt;
TFile* outf;

TH1* hAngRes;

void makeAngResPlot(const Char_t* mcfn,
                    const Char_t* fitfn,
                    const Char_t* outfn,
                    const Char_t* fitbrnm="AngleFromChanCorl.",
                    const Char_t* fittreenm="FitAngle",
                    const Char_t* mctreenm="SimTemplEvts") {
   
   mct = new TChain(mctreenm);
   mct->Add(mcfn);
   Float_t theta(0), phi(0);
   mct->SetBranchAddress("theta",&theta);
   mct->SetBranchAddress("phi",&phi);
   
   
   ftt = new TChain(fittreenm);
   ftt->Add(fitfn);
   TSnRecoDirection* dir(0);
   ftt->SetBranchAddress(fitbrnm,&dir);
   
   
   const Long64_t nents = mct->GetEntries();
   if (nents==ftt->GetEntries()) {
      
      outf = TFile::Open(outfn,"recreate");
      
      // unequal bins for log
      const Double_t arbw = (angresmax-angresmin)/
         static_cast<Float_t>(angresbins);
      Double_t* arbins = new Double_t[angresbins+1],
              * arb    = arbins,
                abw    = 0;
      for (Int_t i=0; i<=angresbins; ++i, ++arb, abw+=arbw) {
         *arb = TMath::Power(10.0, angresmin + abw);
      }
      hAngRes = new TH1F("hAngRes","angular error;"
                            "#alpha (deg)",
                            angresbins, arbins);
      hAngRes->SetDirectory(outf);
      delete[] arbins;
      
      TVector3 mc, reco;
      for (Long64_t i=0; i<nents; ++i) {
         mct->GetEntry(i);
         ftt->GetEntry(i);
         
         mc.SetMagThetaPhi(1.0,theta,phi);
         reco.SetMagThetaPhi(1.0,dir->GetTheta(),dir->GetPhi());
         
         hAngRes->Fill(TMath::RadToDeg() * (reco.Angle(mc)));
         
      }
      
      outf->Write();
      delete outf; outf=0;
      
   } else {
      Printf("ERROR: different number of entries in fit and mc trees.");
   }
   
}
