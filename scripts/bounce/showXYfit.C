#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TVector2.h>
#include <TMath.h>
#include <TChain.h>

#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"
#include "TSnRecoDirection.h"
#include "TSnStopPosnSet.h"

#include "wavePropagate.C"

#endif


TChain* nt, * dt;
TFile* outf(0);
TH1F* hdist;
TH2F* hxy, * htp, * hfs, * hfm, * htc;
TCanvas* cxy, * ctp;

const Int_t rdbins=200,
   phibins=360,
   thetabins=180,
   //xbins=500, ybins=500,
   xbins=200, ybins=200,
   fqbins=100;

const Float_t rdmin=0, rdmax=800,
   phimin=0, phimax=360,
   thetamin=0, thetamax=180,
   //xmin=-750, xmax=750,
   //ymin=-750, ymax=750,
   xmin=-200, xmax=200,
   ymin=-200, ymax=200,
   //fqmin=0, fqmax=200;
   fqmin=-5, fqmax=5;

const Int_t fsthbins=200, fsphbins=720;
const Float_t fsthmin = 160.0, fsthmax = 180.0,
              fsphmin =   0.0, fsphmax = 360.0;


void showXYfit(const Char_t* bfn,
               const Char_t* cfn=0,
               const Char_t* outfn=0,
               const Char_t* fitbn="AngleFromSingleChanOffsets.",
               const Char_t* dtreen="FitOffsets",
               const Char_t* btreen="FitAngle",
               const Char_t* stpbnm="StopPositions.",
               const Char_t* altsbn="AltRunStopPositions.",
               const Float_t chi2min=-100,
               const Float_t chi2max=2,
               const Float_t minStopQual=0,
               const Float_t maxStopQual=5,
               const Int_t sampleEvent=10) {
   nt = new TChain(btreen);
   nt->Add(bfn);
   TSnEventMetadata* meta = new TSnEventMetadata;
   TSnEventHeader*   hdr  = new TSnEventHeader;
   TSnRecoDirection* dir  = new TSnRecoDirection;
   nt->SetBranchAddress("EventMetadata.", &meta);
   nt->SetBranchAddress("EventHeader.", &hdr);
   nt->SetBranchAddress(fitbn, &dir);
   
   TSnStopPosnSet* stops(0), * altstops(0);
   if (cfn!=0) {
      dt = new TChain(dtreen);
      dt->Add(cfn);
      stops = new TSnStopPosnSet;
      dt->SetBranchAddress(stpbnm, &stops);
      if ((dt->GetBranch(altsbn))!=0) {
         altstops = new TSnStopPosnSet;
         dt->SetBranchAddress(altsbn, &altstops);
      }
   }
   
   gROOT->LoadMacro(gSystem->ExpandPathName(
                       "$SNS/scripts/bounce/wavePropagate.C+"));

   if (outfn!=0) {
      outf = TFile::Open(outfn,"recreate");
   }
   
   hdist = new TH1F("hdist","reco'd radial distance;distance to source (m)",
                    rdbins, rdmin, rdmax);
   hdist->SetDirectory(outf);
   
   htp = new TH2F("htp","reco'd #theta,#phi to source;#theta;#phi",
                  thetabins, thetamin, thetamax,
                  phibins, phimin, phimax);
   htp->SetStats(0);
   htp->SetDirectory(outf);
   
   hxy = new TH2F("hxy","reco'd x,y position of source;x (m);y (m)",
                  xbins, xmin, xmax,
                  ybins, ymin, ymax);
   hxy->SetDirectory(outf);
   
   htc = new TH2F("htc","reco'd #theta vs fit quality;fit quality;#theta",
                  fqbins, fqmin, fqmax,
                  thetabins, thetamin, thetamax);
   htc->SetDirectory(outf);
   
   Float_t theta, phi;
   const Long64_t nents = nt->GetEntries();
   for (Long64_t i=0; i<nents; ++i) {
      if ( (i%100)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            i, nents,
            100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(i);
      if ( (stops!=0) || (altstops!=0) ) {
         dt->GetEntry(i);
      }
      
      Bool_t ok = kTRUE;
      const Double_t chi2 = TMath::Log10(dir->GetFitQual());
      if (chi2<chi2min) {
         ok = kFALSE;
      }
      if (chi2>chi2max) {
         ok = kFALSE;
      }
      if (stops!=0) {
         ok &= (stops->GetStopQuality() < maxStopQual);
         ok &= (stops->GetStopQuality() > minStopQual);
      }
      if (altstops!=0) {
         ok &= (altstops->GetStopQuality() < maxStopQual);
         ok &= (altstops->GetStopQuality() > minStopQual);
      }
      
      if (ok) {
         // regularize the angles
         phi   = dir->GetPhi();
         theta = TVector2::Phi_mpi_pi(dir->GetTheta());
         if (theta<0) {
            theta *= -1.0;
            phi   += TMath::Pi();
         }
         phi   = TVector2::Phi_0_2pi(phi);
      
         htp->Fill(theta*TMath::RadToDeg(),
                   phi*TMath::RadToDeg());
         htc->Fill(chi2, theta*TMath::RadToDeg());
         const Double_t dist = trajectory(theta, kFALSE);
         hdist->Fill(dist);
         hxy->Fill(dist*TMath::Cos(phi),
                   dist*TMath::Sin(phi));
      }
   }
   
   cxy = new TCanvas("cxy","cxy",1400,1000);
   cxy->Divide(2,2);
   cxy->cd(1);
   htp->Draw("colz");
   cxy->cd(2);
   hxy->Draw("colz");
   cxy->GetPad(2)->SetGridx();
   cxy->GetPad(2)->SetGridy();
   cxy->cd(3);
   htc->Draw("colz");
   cxy->cd(4);
   hdist->Draw();
   cxy->cd();
   cxy->Update();
   
   if (outf!=0) {
      outf->Write();
   }

}
