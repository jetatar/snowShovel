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
#include <TF1.h>
#include <TLatex.h>
#endif

// 2011 station
static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;
static const Int_t   kNextra    = 57;
static const Int_t   kTrgLoc    = 10;

const Int_t nadcbins=1024;//2048;//4096;
const Float_t minadc=-0.5,maxadc=4095.5;

enum ETrgType { kAll, kThm, kFrc, kHrt, kNtrgTypes};

const Int_t col[kNtrgTypes] = {kBlack, kAzure-6, kOrange+7, kGreen+2};
const Int_t ftl[kNtrgTypes] = {kGray,  kAzure-9, kOrange,   kSpring-4};

TChain* nt;
TFile* inf, * outf;
TH1* hAllTrg[kNchans], * hThmTrg[kNchans], * hFrcTrg[kNchans],
   * hHrtBts[kNchans];
TCanvas* c1;

const Char_t* getTrgTypStr(const Int_t tt) {
   switch (tt) {
      case kAll: return "All Trigs";
      case kThm: return "Therm Trigs";
      case kFrc: return "Forced Trigs";
      case kHrt: return "Heartbeats";
      default: return "Unknown";
   };
}

void drawSig(TH1* h, const Int_t type,
             const Float_t xndc, const Float_t yndc) {
   if (h!=0 && h->GetEntries()>0) {
      h->Fit("gaus","Q","same");
      TF1* fit = h->GetFunction("gaus");
      fit->SetLineColor(ftl[type]);
      fit->SetLineStyle(7);
      fit->SetLineWidth(2);
      const Double_t sig = fit->GetParameter(2);
      const Double_t err = fit->GetParError(2);
      TLatex* l = new TLatex;
      l->SetLineColor(col[type]);
      l->SetTextColor(col[type]);
      l->SetNDC();
      l->DrawLatex(xndc,yndc,
         Form("#splitline{%s}{#sigma=%2.1f#pm%2.1f}",
         getTrgTypStr(type),sig,err));
   }
}

void drawChanRms(const Char_t* infn,
                 const Char_t* outfn,
                 const Bool_t  hasHeartbeats=kFALSE,
                 const Bool_t  verifyChksum=kTRUE) {
   
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
   
   outf = TFile::Open(outfn,"recreate");
   
   TString hn;
   UShort_t samples[kNchans][kNsamps];
   for (Int_t ch=0; ch<kNchans; ch++) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
      
      hn = Form("hAllTrg%02d",ch);
      hAllTrg[ch] = new TH1F(hn.Data(), 
         Form("Chan %d adc distribution, all triggers;ADC",ch),
         nadcbins, minadc, maxadc);
      hAllTrg[ch]->SetDirectory(outf);
      hAllTrg[ch]->SetLineColor(col[kAll]);
      hAllTrg[ch]->GetXaxis()->SetNdivisions(505);
      
      hn = Form("hThmTrg%02d",ch);
      hThmTrg[ch] = new TH1F(hn.Data(), 
         Form("Chan %d adc distribution, thermal triggers;ADC",ch),
         nadcbins, minadc, maxadc);
      hThmTrg[ch]->SetDirectory(outf);
      hThmTrg[ch]->GetXaxis()->SetNdivisions(505);
      hThmTrg[ch]->SetLineColor(col[kThm]);
      
      hn = Form("hFrcTrg%02d",ch);
      hFrcTrg[ch] = new TH1F(hn.Data(), 
         Form("Chan %d adc distribution, forced triggers;ADC",ch),
         nadcbins, minadc, maxadc);
      hFrcTrg[ch]->SetDirectory(outf);
      hFrcTrg[ch]->GetXaxis()->SetNdivisions(505);
      hFrcTrg[ch]->SetLineColor(col[kFrc]);
      
      if (hasHeartbeats) {
         hn = Form("hHrtBts%02d",ch);
         hHrtBts[ch] = new TH1F(hn.Data(), 
            Form("Chan %d adc distribution, heartbeats;ADC",ch),
            nadcbins, minadc, maxadc);
         hHrtBts[ch]->SetDirectory(outf);
         hHrtBts[ch]->GetXaxis()->SetNdivisions(505);
         hHrtBts[ch]->SetLineColor(col[kHrt]);
      }
   }
   
   const Long64_t nents = nt->GetEntries();
   
   nt->GetEntry(0);
   UInt_t oldtime = utime, oldhrt = 0;
   
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e);
      
      if ( (e%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            e, nents,
            100.*static_cast<Float_t>(e)/static_cast<Float_t>(nents));
      }
      
      Bool_t ok=kTRUE;
      if (verifyChksum) {
         ok = (mbchksum == (extra[21]<<16)+extra[20]);
      }
      
      const Bool_t isThm = (extra[kTrgLoc]==0);
      
      Bool_t isHrt=kFALSE;
      if (isThm) {
         if (oldhrt==0) {
            isHrt = (utime - oldtime)==1;
            oldtime = utime;
         } else {
            isHrt = (utime - oldhrt)==1;
         }
         if (isHrt) {
            oldhrt = utime;
         }
      }
      
      if (ok) {
         for (Int_t ch=0; ch<kNchans; ch++) {
            for (Int_t sm=0; sm<kNsamps; sm++) {
               hAllTrg[ch]->Fill(samples[ch][sm]);
               if (isThm) {
                  hThmTrg[ch]->Fill(samples[ch][sm]);
                  if (isHrt && hasHeartbeats) {
                     hHrtBts[ch]->Fill(samples[ch][sm]);
                  }
               } else if (extra[kTrgLoc]==1) {
                  hFrcTrg[ch]->Fill(samples[ch][sm]);
               }
            }
         }
      }
   }
   
   c1 = new TCanvas("c1","c1",800,800);
   c1->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c1->cd(ch+1);
      c1->GetPad(ch+1)->SetLogy();
      hThmTrg[ch]->Draw();
      if (hasHeartbeats) {
         hHrtBts[ch]->Draw("same");
      }
      hFrcTrg[ch]->Draw("same");
      drawSig(hThmTrg[ch],kThm,.58,.82);
      drawSig(hFrcTrg[ch],kFrc,.58,.69);
      if (hasHeartbeats) {
         drawSig(hHrtBts[ch],kHrt,.58,.56);
      }
   }
   c1->cd();
   c1->Update();
   
   outf->Write();
   
}
