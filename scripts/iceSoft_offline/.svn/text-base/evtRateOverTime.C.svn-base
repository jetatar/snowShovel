#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TDatime.h>
#include <TString.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#endif

static const Int_t kNextra=57;
static const Int_t kTrgLoc=10;

TCanvas* c1, * c2;
TChain* nt;
TGraphErrors* gERvsT, * gERvsEnum;
TGraph* gEnumVsT;
TFile* outf;

Double_t getTime(const UInt_t ut, const UInt_t utus) {
   return ( static_cast<Double_t>(ut) + 
            (static_cast<Double_t>(utus)*1.0e-6) );
}

void evtRateOverTime(const Char_t*  infn,
                     const Char_t*  outfn,
                     const Int_t    trg=0,
                     const Long64_t firstEvt=0,
                     const Double_t twin=120,
                     const Bool_t   verifyChksum=kTRUE) {
   // trg == -1 => all events (no selection)
   // verifyChksum should work for arianna0 or header-only trees,
   // since there both the mbed checksum and the extra array will both be 0
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   
   nt = new TChain("nt");
   nt->Add(infn);
   
   UShort_t extra[kNextra];
   UInt_t utime, utimeus, evtnum, mbchksum;
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("HdEvtNum",&evtnum);
   nt->SetBranchAddress("extra",&(extra[0]));
   
   const Long64_t nents = nt->GetEntries() - firstEvt;
   
   outf = TFile::Open(outfn,"recreate");
   
   gERvsT = new TGraphErrors;
   gERvsT->SetName("gERvsT");
   gERvsT->SetTitle("Event Rate vs Time;Date, time (UTC);Event Rate (Hz)");
   gEnumVsT = new TGraph;
   gEnumVsT->SetName("gEnumVsT");
   gEnumVsT->SetTitle("Event Num vs Time;Date, time (UTC);Event Num");
   gERvsEnum = new TGraphErrors;
   gERvsEnum->SetName("gERvsEnum");
   gERvsEnum->SetTitle("Event Rate vs Event Num;Event Num;Event Rate (Hz)");
   
   // first sort the times
   Printf("Found %lld entries. Reading times...",nents);
   Long64_t* const sidx = new Long64_t[nents]; // the full sorted entry numbers
   Double_t* const tims = new Double_t[nents]; // the full list of times
   UInt_t*   const evns = new UInt_t[nents];   // the full list of event nums
   Long64_t* si=sidx;
   Double_t* ti=tims;
   UInt_t*   ei=evns;
   Long64_t  nsel=0;
   Bool_t ok=kFALSE;
   for (Long64_t i=0; i<nents; i++) {
      nt->GetEntry(i+firstEvt);
      ok = (trg==-1) || (extra[kTrgLoc]==trg);
      if (ok) {
         ok &= utime!=0; // skip if no header info
      }
      if (ok && verifyChksum) {
         ok &= (mbchksum == (extra[21]<<16)+extra[20]);
      }
      if (ok) {
         
         //*si = i;
         *si = 0;
         *ti = getTime(utime, utimeus);
         *ei = evtnum;
         nsel++;
         si++;
         ti++;
         ei++;
      }
   }
   
   if (nsel<2) {
      Printf("Not enough events.");
      return;
   }
   
   Printf("Sorting...");
   
   // sidx = indicies of tims that correspond to the sorted ordering of tims
   TMath::Sort(nsel, tims, sidx, kFALSE);
   
   Printf("Finding rates...");
   
   Double_t start = tims[sidx[0]];
   Double_t evts  = 1;
   Double_t dt    = 0;
   Double_t ct    = start;
   Double_t ot    = start;
   Int_t    ertpt = 0, enmpt=0;
   UInt_t   frste = evns[sidx[0]], cen=evns[sidx[0]];
   Long64_t sti   = 0;
   
   si = sidx;
   for (Long64_t i=1; i<nsel; i++, si++) {
      ct = tims[*si];
      
      if (ct < ot) {
         Printf("Out of order! ct=%16.16f, ot=%16.16f, i=%lld",ct,ot,i);
      }
      
      gEnumVsT->SetPoint(enmpt, ct, evns[*si]);
      enmpt++;
      
      if ( (ct - start) > twin ) {
         // find rate
         if (dt>0) {
            const Double_t rate = evts / dt;
            const Double_t hrms = TMath::Sqrt(evts) / (2.0*dt);
            const Double_t hdt  = dt / 2.0;
            const Double_t x    = start + hdt;
            gERvsT->SetPoint(ertpt, x, rate);
            gERvsT->SetPointError(ertpt, hdt, hrms);
            const Double_t xen  = (cen - frste) / 2.0;
            gERvsEnum->SetPoint(ertpt, xen+frste, rate);
            gERvsEnum->SetPointError(ertpt, xen, hrms);
            ertpt++;

            if (rate>5.) {
               Printf("HI RATE: rate=%g, evts=%g, start=%16.16f, dt=%g, "
                  "ct=%16.16f, i=%lld, sti=%lld",
                  rate, evts, start, dt, ct, i, sti);
            }
            
            if (ertpt<10) {
               Printf("i=%lld, rate=%g, evts=%g, start=%16.16f, dt=%16.16f, "
                  "x=%16.16f, ertpt=%d",
                  i, rate, evts, start, dt, x, ertpt);
            }
            
            if ( (ct - start) > 1000) {
               Printf("LW: i=%lld, rate=%g, evts=%g, start=%16.16f, dt=%16.16f, "
                  "ct=%16.16f, ot=%16.16f",
                  i, rate, evts, start, dt, ct, ot);
            }
            
         }
         
         // reset
         start = ct;
         evts  = 1;
         dt    = 0;
         sti   = i;
         frste = evns[*si];
         continue;
      } else {
         
         evts += 1;
         dt    = ct - start;
         cen   = evns[*si];
         
      }
      
      ot = ct;
   }
   
   c1 = new TCanvas("c1","c1 - rate vs time",700,800);
   c1->Divide(1,2);
   c1->cd(1);
   gERvsT->Draw("ap");
   c1->GetPad(1)->SetLogy();
   c1->Update();
   //gERvsT->GetHistogram()->GetXaxis()->SetTimeOffset(0,"gmt");
   //gERvsT->GetHistogram()->GetXaxis()->SetTimeFormat("%m/%d %H:%M");
   gERvsT->GetHistogram()->GetXaxis()->SetTimeDisplay(kTRUE);
   c1->cd(2);
   gEnumVsT->Draw("ap");
   gEnumVsT->GetHistogram()->GetXaxis()->SetTimeDisplay(kTRUE);
   c1->cd();
   c1->Update();
   
   c2 = new TCanvas("c2","c2 - rate vs enum",700,450);
   c2->cd();
   gERvsEnum->Draw("ap");
   c2->SetLogy();
   c2->cd();
   c2->Update();
   
   outf->cd();
   gERvsT->Write("gERvsT");
   gEnumVsT->Write("gEnumVsT");
   gERvsEnum->Write("gERvsEnum");
   
}
