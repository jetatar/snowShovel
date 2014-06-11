#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TString.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#endif

static const Float_t kNvoltsAve = 500; // must match MBED

TChain* nt;
TFile* outf;
TGraphErrors* gVolt, * gCurr;

enum { kVolt, kCurr, kN };
const Int_t cols[kN] = { kMagenta+2, kOrange-3 };
const Int_t mrks[kN] = { 6, 6 };

void plotVolts(const Char_t* infn,
               const Char_t* outfn,
               const UInt_t  mintime=1351728000,
               const UInt_t  maxtime=1414800000,
               const Float_t vgain = 6.0649e-4,
               const Float_t cped  = 1.112,
               const Float_t cgain = -2.6828e-5) {
   
   nt = new TChain("volttree");
   nt->Add(infn);

   // the voltage reading tree
   Float_t av1, av2, rm1, rm2;
   UInt_t vtime;
   nt->SetBranchAddress("aveV1",&av1);
   nt->SetBranchAddress("aveV2",&av2);
   nt->SetBranchAddress("rmsV1",&rm1);
   nt->SetBranchAddress("rmsV2",&rm2);
   nt->SetBranchAddress("vtime",&vtime);
   
   const Long64_t nents = nt->GetEntries();
   
   outf = TFile::Open(outfn,"recreate");

   gVolt = new TGraphErrors;
   gVolt->SetName("gVolt");
   gVolt->SetTitle("Voltage vs Time; Date,time (UTC); V_in (V)");
   gCurr = new TGraphErrors;
   gCurr->SetName("gCurr");
   gCurr->SetTitle("Current vs Time; Date,time (UTC); I_in (A)");
   
   gVolt->SetMarkerStyle(mrks[kVolt]);
   gVolt->SetLineColor(cols[kVolt]);
   gVolt->SetMarkerColor(cols[kVolt]);

   gCurr->SetMarkerStyle(mrks[kCurr]);
   gCurr->SetLineColor(cols[kCurr]);
   gCurr->SetMarkerColor(cols[kCurr]);
   
   Int_t pv=0, pc=0;
   Float_t v, c, ev, ec;
   Bool_t ok;
   for (Long64_t i=0; i<nents; i++) {
      nt->GetEntry(i);
      
      ok = (vtime>mintime) && (vtime<maxtime);
      
      if (ok) {
         v  = av1 * vgain;
         ev = rm1 * vgain / TMath::Sqrt(kNvoltsAve);
         
         c  = cped + ( (av2-av1)*cgain );
         ec = TMath::Sqrt((cgain*cgain)*((rm2*rm2)+(rm1*rm1))/kNvoltsAve);
         
         gVolt->SetPoint(pv, vtime, v);
         gVolt->SetPointError(pv, 0, ev);
         ++pv;
         
         gCurr->SetPoint(pc, vtime, c);
         gCurr->SetPointError(pc, 0, ec);
         ++pc;
      }
   }
   
   outf->cd();
   gVolt->Write("gVolt");
   gCurr->Write("gCurr");
   
}
