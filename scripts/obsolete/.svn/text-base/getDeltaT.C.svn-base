#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TChain.h>
#include <TDatime.h>
#include <TString.h>
#include <TGraph.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TMath.h>
#include <TFile.h>
#include <TEventList.h>

#include <algorithm>

#endif

static const Int_t kNextra=57;
static const Int_t kTrgLoc=10;

TCanvas* c1, * c2, * c3;
TChain* nt;
TH1* h, * hc;
TH2* hDTvsEN, * hDTvsT;
TGraph* gEvsT, * gDTvsEN, * gDTvsT;
TFile* outf;

Double_t* t_p;
UInt_t* en_p;
Long64_t* sidx_p;

void getDeltaT(const Char_t*  infn,
               const Char_t*  outfn,
               const Int_t    trg=-1,
               const Bool_t   reject5sec=kFALSE,
               const UInt_t   enmin=0,
               const UInt_t   enmax=kMaxUInt,
               const Double_t tresComb=0.1,
               const Double_t tresCnsc=0.5,
               const Double_t twin=100.0,
               //const Bool_t   verifyChksum=kTRUE,
               const Bool_t   verifyChksum=kFALSE,
                     Double_t dtmin=1e-2,
                     Double_t dtmax=1e2,
               const Bool_t   dtlogbins=kTRUE,
               //const Double_t mindate=1324339200, // Dec 20, 2011 00:00:00
               const Double_t mindate=0, // Dec 20, 2011 00:00:00
               //const Double_t maxdate=1349049600) { // Oct 1, 2011
               const Double_t maxdate=9999999999) { // Oct 1, 2011
   // trg == -1 => all events (no selection)
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   
   const Int_t dtnbins = static_cast<Int_t>( (dtmax-dtmin) / tresCnsc );
   if (dtlogbins) {
      dtmin = TMath::Log10(dtmin);
      dtmax = TMath::Log10(dtmax);
   }
   
   Printf("Chaining...");
   
   //nt = new TChain("nt");
   nt = new TChain("runtree");
   nt->Add(infn);
   
   UShort_t extra[kNextra];
   UInt_t utime, utimeus, evtnum, mbchksum;
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("EvtNum",&evtnum);
   nt->SetBranchAddress("extra",&(extra[0]));
   
   nt->GetEntry(0);
   
   const Long64_t nents = nt->GetEntries();
   
   Double_t* const t = new Double_t[nents];
                 t_p = t;
   Double_t*       u = t,
           *       v = t,
           *       s = t;
   
   UInt_t* const en = new UInt_t[nents];
               en_p = en;
   UInt_t*       ni = en;
   
   outf = TFile::Open(outfn,"recreate");
   if (outf==0 || outf->IsZombie()) {
      Printf("Could not open output file [%s].",outfn);
      return;
   }
   
   h = new TH1F("h","#Deltat consecutive;t_{i} - t_{i-1} (s)",
      /* nents, -0.5, nents-0.5, */
      twin/tresCnsc, 0, twin);
   h->Sumw2();
   h->SetDirectory(outf);
   /*
   h2 = new TH2F("h2","#Deltat consecutive;evtNum;t_{i} - t_{i-1} (s)",
      nents, evtnum-0.5, evtnum+nents-0.5,
      twin/tresCnsc, 0, twin);
   h2->Sumw2();
   */
   
   gDTvsEN = new TGraph;
   gDTvsEN->SetName("gDTvsEN");
   gDTvsEN->SetTitle("#Deltat consecutive;evtNum;t_{i} - t_{i-1} (s)");
   gDTvsEN->SetMarkerColor(kRed);
   
   hc = new TH1F("hc","All #Deltat combinations;t_{i} - t_{j} (s), j<i",
      /* nents, -0.5, nents-0.5, */
      twin/tresComb, 0, twin);
   hc->Sumw2();
   hc->SetDirectory(outf);
   
   gEvsT = new TGraph;
   gEvsT->SetName("gEvsT");
   gEvsT->SetTitle("Event Number vs Time;Time (UTC);Event Num");
   gEvsT->SetMarkerColor(kRed);
   
   gDTvsT = new TGraph;
   gDTvsT->SetName("gDTvsT");
   gDTvsT->SetTitle("#Deltat consecutive;Time (UTC);t_{i} - t_{i-1} (s)");
   gDTvsT->SetMarkerColor(kRed);
   
   Printf("Looping over %lld entries", nents);
   
   Long64_t selevts = 0;
   for (Long64_t i=0; i<nents; i++) {
      nt->GetEntry(i);
      
      if ( (i%5000)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            i, nents,
            100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      
      Bool_t ok = ( (trg==-1) || (extra[kTrgLoc]==trg) );
      if (ok && verifyChksum) {
         ok &= (mbchksum == (extra[21]<<16)+extra[20]);
      }
      if (ok) {
         ok &= (evtnum<=enmax) && (evtnum>=enmin);
      }
      if (ok) {
         ok &= (utime>mindate) && (utime<maxdate);
      }
      if ( ok ) {

         *u = static_cast<Double_t>(utime)
               + (static_cast<Double_t>(utimeus)*1.0e-6);
         *ni = evtnum;

         gEvsT->SetPoint(selevts, *u, *ni);
         
         selevts++;
         u++;
         ni++;
      }
   }
   
   if (selevts==0) {
      Printf("No events selected.");
      return;
   }
   
   // sidx = indicies of tims that correspond to the sorted ordering of tims
   Long64_t* const sidx = new Long64_t[selevts];
                 sidx_p = sidx;
   Long64_t*          x = sidx,
           *         sx = sidx,
           *         vx = sidx,
           * const   ex = sidx+selevts;
   
   Printf("Sorting by time...");
   
   TMath::Sort(selevts, t, x, kFALSE);
   
   Double_t delt;
   if (reject5sec) {
      Printf("Removing 5 second correlations...");
      TEventList* el5sec = new TEventList("EvtNums5secRjct",
         "List of event numbers rejected by 5 second correlation filter");
      UInt_t nrejects=0;
      for (x = sx = sidx; x!=ex; x++) {
         u = t  + (*x);
         if (x!=sx) {
            delt = *u - *(t + (*(x-1)));
            if ( (delt>4.8) && (delt<6.2) ) {
               nrejects++;
               el5sec->Enter( *(en + (*x)) );
               if (nrejects<5) {
                  Printf("en=%u, time=%16.16f, delt=%f",
                     *(en + (*x)), *u, delt);
               }
               // already sorted, so all we have to do is
               // shift the rest of the array left by 1
               for (vx=x; vx!=(ex-1); vx++) {
                  *vx = *(vx+1);
               }
            }
         }
      }
      if (el5sec->GetN()>0) {
         outf->cd();
         el5sec->Write();
      }
      selevts -= nrejects;
   }
   
   Double_t frstT = TMath::Max(mindate, t[*sidx]);
   Double_t lastT = TMath::Min(maxdate, t[*(ex-1)]);
   const Int_t nbinsT = TMath::Min(2000, static_cast<Int_t>(lastT-frstT+1));
   const Int_t nbinsE = TMath::Min(2000, static_cast<Int_t>(selevts));
   Float_t* const dtbins = new Float_t[dtnbins+1],
          *       dtb    = dtbins,
          * const tbins  = new Float_t[nbinsT+1],
          *       tb     = tbins,
          * const ebins  = new Float_t[nbinsE+1],
          *       eb     = ebins;
   const Double_t dtlinbin = (dtmax - dtmin) / static_cast<Float_t>(dtnbins);
   for (Int_t i=0; i<=dtnbins; i++, dtb++) {
      *dtb = (i*dtlinbin) + dtmin;
      if (dtlogbins) {
         *dtb = TMath::Power(10., *dtb);
      }
   }
   const Double_t tbinw = (lastT-frstT) / static_cast<Float_t>(nbinsT);
   for (Int_t i=0; i<=nbinsT; i++, tb++) {
      *tb = (i*tbinw) + frstT;
   }
   const Double_t ebinw = selevts / static_cast<Float_t>(nbinsE);
   for (Int_t i=0; i<=nbinsE; i++, eb++) {
      *eb = (i*ebinw) + en[*sidx];
   }
   
   hDTvsEN = new TH2F("hDTvsEN",
      "#Deltat consecutive;evtNum;t_{i} - t_{i-1} (s)",
      nbinsE, ebins,
      dtnbins, dtbins);
   hDTvsEN->GetXaxis()->SetNdivisions(505);
   hDTvsEN->SetDirectory(outf);
   
   hDTvsT = new TH2F("hDTvsT",
      "#Deltat consecutive;Time (UTC);t_{i} - t_{i-1} (s)",
      nbinsT, tbins,
      dtnbins, dtbins);
   hDTvsT->GetXaxis()->SetTimeDisplay(1);
   hDTvsT->GetXaxis()->SetNdivisions(505);
   hDTvsT->SetDirectory(outf);
   
   delete[] dtbins;
   delete[] tbins;
   delete[] ebins;
   
   Printf("Calculating time differences...");
   
   Int_t nwins=0;
   
   v = s = t + sidx[0];
   for (x = vx = sx = sidx; x!=ex; x++) {
      u = t + (*x);
      if ( (*u - *s) > twin ) {
         // reset
         s  = u;
         sx = x;
         nwins++;
      } else {
         for (vx=sx; vx!=x; vx++) {
            v = t + (*vx);
            hc->Fill(*u - *v);
         }
      }
   }
   
   UInt_t* n = en;
   UInt_t pt=0;
   for (x = sx = sidx; x!=ex; x++) {
      u = t  + (*x);
      n = en + (*x);
      if (x!=sx) {
         delt = *u - *(t + (*(x-1)));
         h->Fill(delt);
         gDTvsEN->SetPoint(pt, *n, delt);
         gDTvsT->SetPoint(pt, *u, delt);
         hDTvsEN->Fill(*n, delt);
         hDTvsT->Fill(*u, delt);
         pt++;
      }
   }
   
   outf->cd();
   outf->Write();
   gEvsT->Write(gEvsT->GetName());
   gDTvsEN->Write(gDTvsEN->GetName());
   gDTvsT->Write(gDTvsT->GetName());
   
   Printf("Done! Drawing...");
   
   c1 = new TCanvas("c1","c1 - deltas",800,450);
   c1->Divide(2,1);
   c1->cd(1);
   hc->Draw();
   gPad->SetLogy();
   c1->cd(2);
   gPad->SetLogy();
   h->Draw();
   /*
   c1->cd(3);
   gDTvsT->Draw("ap");
   c1->GetPad(3)->SetLogy();
   c1->cd(4);
   c1->GetPad(4)->SetLogy();
   gDTvsEN->Draw("ap");
   */
   c1->cd();
   c1->Update();
   gSystem->ProcessEvents();
   TH1* hg = gDTvsT->GetHistogram();
   if (hg!=0) {
      //hg->GetXaxis()->SetTimeFormat("%H:%M %F1970-01-01 00:00:00");
      hg->GetXaxis()->SetTimeDisplay(1);
      hg->GetXaxis()->SetNdivisions(505);
   }
   hg = gDTvsEN->GetHistogram();
   if (hg!=0) {
      hg->GetXaxis()->SetNdivisions(505);
   }
   c1->cd();
   c1->Update();
   
   
   c2 = new TCanvas("c2","c2 - rate",450,450);
   c2->cd();
   gEvsT->Draw("ap");
   c2->Update();
   c2->cd();
   c2->Update();
   gSystem->ProcessEvents();
   hg = gEvsT->GetHistogram();
   if (hg!=0) {
      //hg->GetXaxis()->SetTimeFormat("%H:%M %F1970-01-01 00:00:00");
      hg->GetXaxis()->SetTimeDisplay(1);
      hg->GetXaxis()->SetNdivisions(505);
   }
   c2->cd();
   c2->Update();
   
   c3 = new TCanvas("c3","c3 - 2d hist deltas",800,450);
   c3->Divide(2,1);
   c3->cd(1);
   hDTvsT->Draw("colz");
   if (dtlogbins) {
      c3->GetPad(1)->SetLogy();
   }
   c3->GetPad(1)->SetLogz();
   c3->cd(2);
   hDTvsEN->Draw("colz");
   if (dtlogbins) {
      c3->GetPad(2)->SetLogy();
   }
   c3->GetPad(2)->SetLogz();
   c3->cd();
   c3->Update();
   
   
   Printf("found [%d] windows",nwins);
   
   
}
