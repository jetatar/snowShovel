#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TNtuple.h>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TVector2.h>
#include <TMath.h>

#include <NSnConstants.h>

#endif

#include "wavePropagate.C"

TFile* outf;
TTree* pt;
TChain* ta, * tc;

void zipDtAngTraj(const Char_t* tcfn,
                  const Char_t* tafn,
                  const Char_t* outfn) {
   ta = new TChain("tAngleFit");
   ta->Add(tafn);
   tc = new TChain("tChanDelays");
   tc->Add(tcfn);
   
   Float_t aRefEvtNum, aBncEvtNum, cRefEvtNum, cBncEvtNum,
      dt[NSnConstants::kNchans], ll[NSnConstants::kNchans],
      chDelEntry, rtheta, rphi, chi2, rho, theta, phi;
   ta->SetBranchAddress("RefEvtNum",&aRefEvtNum);
   ta->SetBranchAddress("BncEvtNum",&aBncEvtNum);
   ta->SetBranchAddress("ChDelEntry",&chDelEntry);
   ta->SetBranchAddress("theta",&rtheta);
   ta->SetBranchAddress("phi",&rphi);
   ta->SetBranchAddress("chi2",&chi2);
   
   tc->SetBranchAddress("RefEvtNum",&cRefEvtNum);
   tc->SetBranchAddress("BncEvtNum",&cBncEvtNum);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      tc->SetBranchAddress(Form("dt%d",ch),&(dt[ch]));
      tc->SetBranchAddress(Form("ll%d",ch),&(ll[ch]));
   }
   
   outf = TFile::Open(outfn,"recreate");
   pt = new TTree("pt","chan delay, angle and prop pos fits");
   pt->Branch("RefEvtNum",&aRefEvtNum);
   pt->Branch("BncEvtNum",&aBncEvtNum);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      pt->Branch(Form("dt%d",ch),&(dt[ch]));
      pt->Branch(Form("ll%d",ch),&(ll[ch]));
   }
   pt->Branch("rtheta",&rtheta);
   pt->Branch("rphi",&rphi);
   pt->Branch("theta",&theta);
   pt->Branch("phi",&phi);
   pt->Branch("chi2",&chi2);
   pt->Branch("rho",&rho);
   pt->SetDirectory(outf);
   
   const Long64_t nents = ta->GetEntries();
   for (Long64_t i=0; i<nents; ++i) {
      if ( (i%250)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
                 i, nents,
                 100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }

      ta->GetEntry(i);
      tc->GetEntry(TMath::Nint(chDelEntry));
      
      if ( (aRefEvtNum!=cRefEvtNum) || (aBncEvtNum!=cBncEvtNum) ) {
         Fatal("zipDtAngTraj","event mismatch at entry %lld: "
               "chDelEntry=%d (%g), ta(%g,%g), tc(%g, %g)",
               i, TMath::Nint(chDelEntry),chDelEntry,
               aRefEvtNum, aBncEvtNum,
               cRefEvtNum, cBncEvtNum);
      } else {

         theta = TVector2::Phi_mpi_pi(rtheta);
         phi   = rphi;
         if (theta<0) {
            theta *= -1.0;
            phi   += TMath::Pi();
         }
         phi   = TVector2::Phi_0_2pi(phi);
         
         rho = trajectory(theta, kFALSE);
         
         theta *= TMath::RadToDeg();
         phi   *= TMath::RadToDeg();

         pt->Fill();
      }
   }
   
   outf->Write();
}
