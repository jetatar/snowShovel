#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TObjArray.h>
#include <TGraphErrors.h>
#include <TVirtualFFT.h>
#include <TTimeStamp.h>
#include <TParameter.h>
#include <TCanvas.h>
#endif

#include "lombtest.C"

//#define DEBUG


// 2011 station
static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;
static const Int_t   kNextra    = 57;
static const Int_t   kTrgBitIdx = 10;
static const Float_t kSmpRate   = 1.92; // GHz


TChain* nt;
TFile* outf;

TH1* hf, * hw;
TH2* hWvFreqVsEvt[kNchans], * hWvFreqVsGdEvt[kNchans];

TCanvas* cc, * cb;

void frqStdy(const Char_t*  infn,
             const Char_t*  outfn,
             const Int_t    trg=0,
             const Float_t  maxPower=5e4,
             const Float_t  minPower=1e1,
             const Long64_t firstEvt=0,
             const Bool_t   verifyChksum=kTRUE) {
   
   
   // open the tree
   nt = new TChain("nt");
   nt->Add(infn);
   
   const Long64_t nents = nt->GetEntries() - firstEvt;
   Printf("Entries in tree = %lld",nents);
   
   // open the output file (to store histograms)
   outf = TFile::Open(outfn,"recreate");
   
   // read from tree into...
   UShort_t samples[kNchans][kNsamps];
   UShort_t extra[kNextra];
   UInt_t   evnum, utime, utimeus, mbchksum;
   
   // setup tree, make hists
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("EvtNum",&evnum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("extra",&(extra[0]));
   
   
   static const Float_t delta   = 1.0 / kSmpRate;
   static const Int_t halfsamps = kNsamps/2;
   static const Float_t fincr   = 1.0 / (delta*kNsamps);
   
   
   TString hn;
   for (Int_t ch=0; ch<kNchans; ch++) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
      
      hn = Form("hWvFreqVsEvt_ch%d",ch);
      hWvFreqVsEvt[ch] = new TH2F(hn.Data(),
         Form("frequencies vs event (chan %d);event;freqency",ch),
         nents, -0.5, nents-0.5,
         halfsamps+1, -fincr/2.0, halfsamps*fincr);
      hWvFreqVsEvt[ch]->SetDirectory(outf);
      
   }

#ifdef DEBUG
   cb = new TCanvas("cb","cb - debug",900,650);
#endif   
   
   Long64_t selevts=0;
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e+firstEvt);
      
      if ( (e%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            e, nents,
            100.*static_cast<Float_t>(e)/static_cast<Float_t>(nents));
      }
      
      Bool_t ok = (trg==-1 || trg==extra[kTrgBitIdx]);
      if (ok && verifyChksum) {
         ok &= (mbchksum == (extra[21]<<16)+extra[20]);
      }
      
      if (ok) {
         
         selevts++;
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            
#ifdef DEBUG
            hw = new TH1F("hw","wave",kNsamps,-0.5,kNsamps-0.5);
            hf = new TH1F("hf","freq",halfsamps+1, -fincr/2.0, halfsamps*fincr);
            hw->SetBit(TH1::kCanDelete);
            hf->SetBit(TH1::kCanDelete);
#endif

            // copy the data to the FFT input/output array
            Float_t* const wy  = new Float_t[kNsamps],
                   *       dwy = wy;
            UShort_t*      dd  = samples[ch];
            for (Int_t i=0; i<kNsamps; i++, dwy++, dd++) {
               *dwy = static_cast<Float_t>(*dd);
#ifdef DEBUG
               hw->Fill(i,*dwy);
               //Printf("ch%d,i%d: samples=%hu, dwy=%g",
               //   ch,i,samples[ch][i],*dwy);
#endif
            }
            // do the FFT
            realft(wy, kNsamps, 1);
            // copy the result to the histogram
            Int_t j=0;
            dwy = wy+2;
            Float_t* dw1 = dwy+1;
            Float_t  pw;
            hWvFreqVsEvt[ch]->SetCellContent(e+1, ++j, wy[1]);
#ifdef DEBUG
            hf->SetBinContent(j, wy[1]);
#endif
            for (Int_t i=2; i<kNsamps; i+=2, dwy+=2, dw1+=2) {
               pw = TMath::Sqrt((*dwy)*(*dwy)+(*dw1)*(*dw1));
               hWvFreqVsEvt[ch]->SetCellContent(e+1, ++j, pw);
#ifdef DEBUG
               hf->SetBinContent(j, pw);
#endif
            }
            hWvFreqVsEvt[ch]->SetCellContent(e+1, ++j, wy[1]);
#ifdef DEBUG
            hf->SetBinContent(j, wy[1]);
#endif
            delete[] wy;
            
#ifdef DEBUG
            cb->Divide(2,1);
            cb->cd(1);
            hf->Draw();
            cb->cd(2);
            hw->Draw();
            cb->cd();
            cb->WaitPrimitive();
#endif
         }
      }
      
   }
   
   cc = new TCanvas("cc","cc - freq vs evt",800,749);
   cc->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      cc->cd(ch+1);
      cc->GetPad(ch+1)->SetLogz();
      hWvFreqVsEvt[ch]->SetMinimum(minPower);
      hWvFreqVsEvt[ch]->SetMaximum(maxPower);
      hWvFreqVsEvt[ch]->Draw("colz");
   }
   cc->cd();
   cc->Update();
   
   TString ofn = outfn;
   TString oon = ofn(ofn.Last('/')+1, ofn.Index(".root")-ofn.Last('/')-1);
   TString odn = ofn(0, ofn.Last('/'));
   cc->Print(Form("%s/canv.%s.root",odn.Data(),oon.Data()));
   cc->Print(Form("%s/canv.%s.gif",odn.Data(),oon.Data()));
   
   c2 = new TCanvas("c2","c2 - freq vs evt . collapsed",800,749);
   c2->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c2->cd(ch+1);
      c2->GetPad(ch+1)->SetLogz();
      
      hn = Form("hWvFreqVsGdEvt_ch%d",ch);
      hWvFreqVsGdEvt[ch] = new TH2F(hn.Data(),
         Form("frequencies vs good event (chan %d)"
              ";good event (arb. num.);freq",ch),
         selevts, -0.5, selevts-0.5,
         halfsamps+1, -fincr/2.0, halfsamps*fincr);
      hWvFreqVsGdEvt[ch]->SetDirectory(outf);
      
      const Int_t nybins = hWvFreqVsEvt[ch]->GetNbinsY();
      Int_t sb=0;
      for (Int_t xb=hWvFreqVsEvt[ch]->GetNbinsX(); xb>0; xb--) {
         if (hWvFreqVsEvt[ch]->Integral(xb,xb, 1, nybins) > 0) {
            sb++;
            for (Int_t yb=nybins; yb>0; yb--) {
               const Double_t cont = hWvFreqVsEvt[ch]->GetBinContent(xb, yb);
               hWvFreqVsGdEvt[ch]->SetBinContent(sb, yb, cont);
            }
         }
      }
      
      hWvFreqVsGdEvt[ch]->SetMinimum(minPower);
      hWvFreqVsGdEvt[ch]->SetMaximum(maxPower);
      hWvFreqVsGdEvt[ch]->Draw("colz");
   }
   c2->cd();
   c2->Update();
   
   
   outf->Write();
   
  
}
