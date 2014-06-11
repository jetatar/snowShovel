#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TSystem.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include <TPython.h>
#include <TVector3.h>
#include <TSpline.h>
#include <TRandom3.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <NSnConstants.h>
#include <TSnSpectral.h>

#include <cmath>
#endif

#include "noAveBounceStdy.C"

//#define DEBUG_ANGLE

TNtuple* tAngleFit;

TChain* tChanDelays;

#ifdef DEBUG_ANGLE
TGraph* dbgth=0, * dbgphi=0;
#endif

Double_t getAngleChi2(const Double_t* thetaPhi) {
   Double_t chi2(0), c(0);
   
   TVector3 norm; norm.SetMagThetaPhi(1.0, thetaPhi[0], thetaPhi[1]);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         const TVector3& posCh = getStnPos(ch);
         const TVector3& posXc = getStnPos(xc);
         const Double_t  disCh = -(posCh.Dot(norm));
         const Double_t  disXc = -(posXc.Dot(norm));
         // !!! check sign of delta(distance) and dtcor!
         const Double_t  dt = (disCh-disXc) * kNgTopFern / kC_m_ns;  // from m to ns
         const Double_t bdt = thetaPhi[2+ch] - thetaPhi[2+xc];

         c = (dt-bdt);
         chi2 += c*c;
         /*
         Printf("(%g,%g) d[%d]=%g, d[%d]=%g, bdt=%g, sch=%g, sxc=%g, dt=%g, c=%g",
                thetaPhi[0]*TMath::RadToDeg(), thetaPhi[1]*TMath::RadToDeg(),
                ch, thetaPhi[2+ch], xc, thetaPhi[2+xc], bdt, disCh, disXc, dt, c);
         */
      }
   }
   
#ifdef DEBUG_ANGLE
   if (dbgth!=0) {
      dbgth->SetPoint(dbgth->GetN(), thetaPhi[0]*TMath::RadToDeg(),
                      chi2);
      dbgphi->SetPoint(dbgphi->GetN(), thetaPhi[1]*TMath::RadToDeg(),
                      chi2);
   }
#endif
   
   return chi2;
   
}


void fitThetaPhiFromDelays(const Char_t* tcdfn,
                           const Char_t* outfn,
                           const Float_t llcut=1e20,
                           const Char_t* minner="Minuit2",
                           const Char_t* algo="Migrad") {


   if (mini==0) {
      mini = ROOT::Math::Factory::CreateMinimizer(minner, algo);
      mini->SetMaxFunctionCalls(1000000);
      mini->SetMaxIterations(10000);
      mini->SetTolerance(0.001);
      mini->SetPrintLevel(0);
   }

   tChanDelays = new TChain("tChanDelays");
   tChanDelays->Add(tcdfn);
   const Long64_t nents = tChanDelays->GetEntries();
   if (nents==0) {
      Error("fitThetaPhiFromDelays","No events in [%s].",tcdfn);
      return;
   }
   
   Float_t revnum, bevnum,
           dt[NSnConstants::kNchans],
           ll[NSnConstants::kNchans];
   tChanDelays->SetBranchAddress("RefEvtNum",&revnum);
   tChanDelays->SetBranchAddress("BncEvtNum",&bevnum);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      tChanDelays->SetBranchAddress(Form("dt%d",ch),&(dt[ch]));
      tChanDelays->SetBranchAddress(Form("ll%d",ch),&(ll[ch]));
   }

   
   outf = TFile::Open(outfn,"recreate");
   tAngleFit = new TNtuple("tAngleFit","theta phi fit from chan delays",
                           "RefEvtNum:BncEvtNum:ChDelEntry:theta:phi:chi2");
   tAngleFit->SetDirectory(outf);
   
   for (Long64_t ev=0; ev<nents; ++ev) {
      if ( (ev%250)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
                 ev, nents,
                 100.*static_cast<Float_t>(ev)/static_cast<Float_t>(nents));
      }

      tChanDelays->GetEntry(ev);
      
#ifdef DEBUG_ANGLE
      dbgth = new TGraph;
      dbgphi = new TGraph;
#endif
      
      mini->Clear();
      ROOT::Math::Functor f(&getAngleChi2, 2+NSnConstants::kNchans);
      mini->SetFunction(f);
      if (strcmp(minner,"Genetic")==0) {
         mini->SetLimitedVariable(0, "theta", 3, 0.025, 
                                  TMath::Pi()/2, TMath::Pi());
         mini->SetLimitedVariable(1,   "phi", 5, 0.025, 
                                  0, TMath::TwoPi());
      } else {
         mini->SetVariable(0, "theta", 3, 0.01);
         mini->SetVariable(1,   "phi", 5, 0.01);
      }
      Bool_t ok=kTRUE;
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         mini->SetFixedVariable(2+ch, Form("dt%d",ch), -dt[ch]);
         ok &= (ll[ch]<llcut);
      }
      if (ok) {
         mini->Minimize();
         const Double_t* result = mini->X();
         // regularize the angles
         Double_t theta = TVector2::Phi_mpi_pi(result[0]);
         Double_t phi   = result[1];
         if (theta<0) {
            theta *= -1.0;
            phi   += TMath::Pi();
         }
         phi   = TVector2::Phi_0_2pi(phi);
         /*
         if (ev==2) {
            Printf("sh=(%g, %g, %g, %g), dt=(%g, %g, %g), res=(%g, %g), "
                   "ang=(%g, %g)",
                   -dt[0], -dt[1], -dt[2], -dt[3],
                   -dt[1]+dt[0], -dt[2]+dt[1], -dt[3]+dt[2],
                   result[0]*TMath::RadToDeg(), result[1]*TMath::RadToDeg(),
                   theta*TMath::RadToDeg(), phi*TMath::RadToDeg());
         }
         */
         tAngleFit->Fill(revnum, bevnum, ev, 
                         //theta*TMath::RadToDeg(), phi*TMath::RadToDeg(),
                         theta, phi,
                         mini->MinValue());
         
#ifdef DEBUG_ANGLE
         TCanvas* cdbg(0);
         Printf("ev=%lld: (%g, %g), min=%g, dt=(%g, %g, %g, %g)",
                ev, theta*TMath::RadToDeg(), phi*TMath::RadToDeg(),
                mini->MinValue(), dt[0], dt[1], dt[2], dt[3]);
            
         cdbg = new TCanvas("cdbg","cdbg",800,600);
         cdbg->Divide(1,2);
         cdbg->cd(1);
         dbgth->Draw("ap");
         cdbg->cd(2);
         dbgphi->Draw("ap");
         cdbg->cd();
         cdbg->Update();
         cdbg->WaitPrimitive();
         delete dbgth; delete dbgphi; delete cdbg;
#endif
      }
   }
   
   outf->Write();
   
}
