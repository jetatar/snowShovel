#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TMath.h>
#include <TChain.h>
#include <TVector3.h>

#include <TSnMath.h>
#include <NSnConstants.h>
#endif

TFile* outf;
TChain* nt;

TH1* hllAll[NSnConstants::kNchans], * hllSel[NSnConstants::kNchans],
   * hChi2All, * hChi2Sel,
   * hRhoAll, * hRhoSel,
   * hThetaAll, * hThetaSel, * hThetaResAll, * hThetaResSel,
   * hPhiAll, * hPhiSel, * hPhiResAll, * hPhiResSel,
   * hAngResAll, * hAngResSel;
TH2* hThetaPhiAll, * hThetaPhiSel,
   * hXYAll, * hXYSel;


const Int_t llbins = 100, c2bins=120,
   rhobins=200, 
   thetabins=180, thetaresbins=400, phibins=360, phiresbins=400, angresbins=200,
   xbins=300, ybins=300;

const Float_t llmin=0, llmax=1,
   c2min=0, c2max=40,
   rhomin=0, rhomax=400,
   thetamin=0, thetamax=180,
   thetaresmin=-10, thetaresmax=10,
   phimin=0, phimax=360,
   phiresmin=-100, phiresmax=100,
   angresmin=-2, angresmax=2,
   xmin=-150, xmax=150, ymin=-150, ymax=150;


void fillBouncePlots(const Char_t* zfn,
                     const Char_t* outfn,
                     const Float_t llcut,
                     const Float_t chi2min,
                     const Float_t chi2max) {
   nt = new TChain("pt");
   nt->Add(zfn);
   
   Float_t RefEvtNum, BncEvtNum,
      dt[NSnConstants::kNchans], ll[NSnConstants::kNchans],
      rtheta, rphi, chi2, rho, theta, phi;
   nt->SetBranchAddress("RefEvtNum",&RefEvtNum);
   nt->SetBranchAddress("BncEvtNum",&BncEvtNum);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      nt->SetBranchAddress(Form("dt%d",ch),&(dt[ch]));
      nt->SetBranchAddress(Form("ll%d",ch),&(ll[ch]));
   }
   nt->SetBranchAddress("rtheta",&rtheta);
   nt->SetBranchAddress("rphi",&rphi);
   nt->SetBranchAddress("theta",&theta);
   nt->SetBranchAddress("phi",&phi);
   nt->SetBranchAddress("chi2",&chi2);
   nt->SetBranchAddress("rho",&rho);

   TString hn;
   TString seln(Form("fq<%g, %g<#chi^{2}<%g",llcut,chi2min,chi2max));
   outf = TFile::Open(outfn,"recreate");
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      hn = Form("hllAll_%d",ch);
      hllAll[ch] = new TH1F(hn.Data(),
                            Form("fit quality of chan %d shifts (all)"
                                 ";fit quality",ch),
                            llbins, llmin, llmax);
      hllAll[ch]->SetDirectory(outf);
      hn = Form("hllSel_%d",ch);
      hllSel[ch] = new TH1F(hn.Data(),
                            Form("fit quality of chan %d shifts (%s)"
                                 ";fit quality",ch,seln.Data()),
                            llbins, llmin, llmax);
      hllSel[ch]->SetDirectory(outf);
   }
   hChi2All = new TH1F("hChi2All","#chi^{2} of (#theta,#phi) reco (all)"
                       ";#chi^{2}",
                       c2bins, c2min, c2max);
   hChi2All->SetDirectory(outf);
   hChi2Sel = new TH1F("hChi2Sel",Form("#chi^{2} of (#theta,#phi) reco (%s)"
                                       ";#chi^{2}",seln.Data()),
                       c2bins, c2min, c2max);
   hChi2Sel->SetDirectory(outf);
   hRhoAll = new TH1F("hRhoAll","reco distance (all);#rho (m)",
                      rhobins, rhomin, rhomax);
   hRhoAll->SetDirectory(outf);
   hRhoSel = new TH1F("hRhoSel",Form("reco distance (%s);#rho (m)",seln.Data()),
                      rhobins, rhomin, rhomax);
   hRhoSel->SetDirectory(outf);
   hThetaAll = new TH1F("hThetaAll","#theta (all);#theta (deg)",
                        thetabins, thetamin, thetamax);
   hThetaAll->SetDirectory(outf);
   hThetaSel = new TH1F("hThetaSel",Form("#theta (%s);#theta (deg)",seln.Data()),
                        thetabins, thetamin, thetamax);
   hThetaSel->SetDirectory(outf);
   hThetaResAll = new TH1F("hThetaResAll","#theta - <#theta> (all);"
                           "#theta - <#theta> (deg)",
                           thetaresbins, thetaresmin, thetaresmax);
   hThetaResAll->SetDirectory(outf);
   hThetaResSel = new TH1F("hThetaResSel",Form("#theta - <#theta> (%s);"
                           "#theta - <#theta> (deg)",seln.Data()),
                           thetaresbins, thetaresmin, thetaresmax);
   hThetaResSel->SetDirectory(outf);
   hPhiAll = new TH1F("hPhiAll","#phi (all);#phi (deg)",
                        phibins, phimin, phimax);
   hPhiAll->SetDirectory(outf);
   hPhiSel = new TH1F("hPhiSel",Form("#phi (%s);#phi (deg)",seln.Data()),
                        phibins, phimin, phimax);
   hPhiSel->SetDirectory(outf);
   hPhiResAll = new TH1F("hPhiResAll","#phi - <#phi> (all);"
                           "#phi - <#phi> (deg)",
                           phiresbins, phiresmin, phiresmax);
   hPhiResAll->SetDirectory(outf);
   hPhiResSel = new TH1F("hPhiResSel",Form("#phi - <#phi> (%s);"
                           "#phi - <#phi> (deg)",seln.Data()),
                           phiresbins, phiresmin, phiresmax);
   hPhiResSel->SetDirectory(outf);
   hThetaPhiAll = new TH2F("hThetaPhiAll","#phi vs #theta (all);"
                           "#theta (deg);#phi (deg)",
                           thetabins, thetamin, thetamax,
                           phibins, phimin, phimax);
   hThetaPhiAll->SetDirectory(outf);
   hThetaPhiSel = new TH2F("hThetaPhiSel",Form("#phi vs #theta (%s);"
                           "#theta (deg);#phi (deg)",seln.Data()),
                           thetabins, thetamin, thetamax,
                           phibins, phimin, phimax);
   hThetaPhiSel->SetDirectory(outf);
   // unequal bins for log
   const Double_t arbw = (angresmax-angresmin)/static_cast<Float_t>(angresbins);
   Double_t* arbins = new Double_t[angresbins+1],
           * arb    = arbins,
             abw    = 0;
   for (Int_t i=0; i<=angresbins; ++i, ++arb, abw+=arbw) {
      *arb = TMath::Power(10.0, angresmin + abw);
   }
   hAngResAll = new TH1F("hAngResAll","angular error (all);"
                           "#alpha (deg)",
                           angresbins, arbins);
   hAngResAll->SetDirectory(outf);
   hAngResSel = new TH1F("hAngResSel",Form("angular error (%s);"
                           "#alpha (deg)",seln.Data()),
                           angresbins, arbins);
   hAngResSel->SetDirectory(outf);
   hXYAll = new TH2F("hXYAll","X vs Y (all);X (m);Y (m)",
                     xbins, xmin, xmax,
                     ybins, ymin, ymax);
   hXYAll->SetDirectory(outf);
   hXYSel = new TH2F("hXYSel",Form("X vs Y (%s);X (m);Y (m)",seln.Data()),
                     xbins, xmin, xmax,
                     ybins, ymin, ymax);
   hXYSel->SetDirectory(outf);

   Printf("Filling hists, calculating averages...");
   
   Float_t x, y, 
      thetaXAveAll(0), thetaYAveAll(0),
      phiXAveAll(0), phiYAveAll(0),
      thetaXAveSel(0), thetaYAveSel(0),
      phiXAveSel(0), phiYAveSel(0),
      nAll(0), nSel(0);
   const Long64_t nents = nt->GetEntries();
   for (Long64_t i=0; i<nents; ++i) {
      if ( (i%1000)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
                 i, nents,
                 100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(i);
      Bool_t ok = kTRUE;
      ok &= (chi2>chi2min);
      ok &= (chi2<chi2max);
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         ok &= (ll[ch] < llcut);
         
         hllAll[ch]->Fill(ll[ch]);
      }
      x = rho*TMath::Cos(rphi);
      y = rho*TMath::Sin(rphi);
      thetaXAveAll += TMath::Cos(rtheta);
      thetaYAveAll += TMath::Sin(rtheta);
      phiXAveAll   += TMath::Cos(rphi);
      phiYAveAll   += TMath::Sin(rphi);
      nAll         += 1.0;
      hChi2All->Fill(chi2);
      hRhoAll->Fill(rho);
      hThetaAll->Fill(theta);
      hPhiAll->Fill(phi);
      hThetaPhiAll->Fill(theta,phi);
      hXYAll->Fill(x, y);
      if (ok) {
         thetaXAveSel += TMath::Cos(rtheta);
         thetaYAveSel += TMath::Sin(rtheta);
         phiXAveSel   += TMath::Cos(rphi);
         phiYAveSel   += TMath::Sin(rphi);
         nSel         += 1.0;
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            hllSel[ch]->Fill(ll[ch]);
         }
         hChi2Sel->Fill(chi2);
         hRhoSel->Fill(rho);
         hThetaSel->Fill(theta);
         hPhiSel->Fill(phi);
         hThetaPhiSel->Fill(theta,phi);
         hXYSel->Fill(x, y);
      }
   }

   Printf("\nFilling resolution hists...");
   
   thetaXAveAll /= nAll;
   thetaYAveAll /= nAll;
   phiXAveAll   /= nAll;
   phiYAveAll   /= nAll;
   thetaXAveSel /= nSel;
   thetaYAveSel /= nSel;
   phiXAveSel   /= nSel;
   phiYAveSel   /= nSel;
   
   const Float_t
      thetaAveAll = TMath::ATan2(thetaYAveAll, thetaXAveAll) * TMath::RadToDeg(),
      phiAveAll   = TMath::ATan2(phiYAveAll, phiXAveAll) * TMath::RadToDeg(),
      thetaAveSel = TMath::ATan2(thetaYAveSel, thetaXAveSel) * TMath::RadToDeg(),
      phiAveSel   = TMath::ATan2(phiYAveSel, phiXAveSel) * TMath::RadToDeg();
   
   TVector3 aveDirAll, aveDirSel;
   aveDirAll.SetMagThetaPhi(1.00, thetaAveAll*TMath::DegToRad(),
                            phiAveAll*TMath::DegToRad());
   aveDirSel.SetMagThetaPhi(1.00, thetaAveSel*TMath::DegToRad(),
                            phiAveSel*TMath::DegToRad());
   
   for (Long64_t i=0; i<nents; ++i) {
      if ( (i%1000)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
                 i, nents,
                 100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(i);
      Bool_t ok = kTRUE;
      ok &= (chi2>chi2min);
      ok &= (chi2<chi2max);
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         ok &= (ll[ch] < llcut);
      }

      TVector3 dir;
      dir.SetMagThetaPhi(1.00, rtheta, rphi);
      
      hThetaResAll->Fill(TSnMath::GetAngleDiffDeg(theta,thetaAveAll));
      hPhiResAll->Fill(TSnMath::GetAngleDiffDeg(phi, phiAveAll));
      hAngResAll->Fill(TMath::RadToDeg() * (dir.Angle(aveDirAll)));
      if (ok) {
         hThetaResSel->Fill(TSnMath::GetAngleDiffDeg(theta, thetaAveSel));
         hPhiResSel->Fill(TSnMath::GetAngleDiffDeg(phi, phiAveSel));
         hAngResSel->Fill(TMath::RadToDeg() * (dir.Angle(aveDirSel)));
      }
   }
   
   outf->Write();
}
