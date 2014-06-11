#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TH2.h>
#include <TH1.h>
#include <TGraphErrors.h>
#include <TObjArray.h>
#include <TError.h>
#include <TString.h>
#include <TDirectory.h>
#include <TMath.h>
#endif

// 2011 station
static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;

TFile* inf, * outf;
TH2* hWvFreqVsEvt[kNchans];
TGraphErrors* gTotAveFreq[kNchans];
TObjArray* gAveFreqs[kNchans];

void frqStdy1d(const Char_t* infn,
               const Char_t* outfn,
               const Int_t   nevtsave=500) {
   // use output of frqStdy to make 1d "average" freq profiles
   
   
   inf = TFile::Open(infn);
   if (inf==0 || inf->IsZombie()) {
      Error("frqStdy1d","Could not open input file [%s]",infn);
      return;
   }
   
   outf = TFile::Open(outfn, "recreate");
   if (outf==0 || outf->IsZombie()) {
      Error("frqStdy1d","Could not open output file [%s]",outfn);
      return;
   }
   
   TGraphErrors* gAveFreq;
   
   for (Int_t i=0; i<kNchans; i++) {
      gAveFreqs[i] = new TObjArray;
      gAveFreqs[i]->SetName(Form("gTotAveFreqs_ch%d",i));
      hWvFreqVsEvt[i] = 
         dynamic_cast<TH2*>(inf->Get(Form("hWvFreqVsEvt_ch%d",i)));
      if (hWvFreqVsEvt[i] != 0) {
         
         gTotAveFreq[i] = new TGraphErrors;
         gTotAveFreq[i]->SetName(Form("gTotAveFreq_ch%d",i));
         gTotAveFreq[i]->SetTitle(Form(";%s;%s",
            hWvFreqVsEvt[i]->GetYaxis()->GetTitle(),
            hWvFreqVsEvt[i]->GetZaxis()->GetTitle()));
      
         const Int_t nybins = hWvFreqVsEvt[i]->GetNbinsY();
         const Int_t nxbins = hWvFreqVsEvt[i]->GetNbinsX();
         Double_t* const a  = new Double_t[nybins],
                 * ai       = a,
                 * const r  = new Double_t[nybins],
                 * ri       = r,
                 * const n  = new Double_t[nybins],
                 * ni       = n,
                 * const ta = new Double_t[nybins],
                 * tai      = ta,
                 * const tr = new Double_t[nybins],
                 * tri      = tr,
                 * const tn = new Double_t[nybins],
                 * tni      = tn;
         Double_t  c;
         Int_t evts=1;
         memset(a, 0, nybins*sizeof(Double_t));
         memset(r, 0, nybins*sizeof(Double_t));
         memset(n, 0, nybins*sizeof(Double_t));
         memset(ta, 0, nybins*sizeof(Double_t));
         memset(tr, 0, nybins*sizeof(Double_t));
         memset(tn, 0, nybins*sizeof(Double_t));
         Int_t y;
         for (Int_t x=0; x<nxbins; x++, evts++) {
            const Bool_t doAve = (evts>nevtsave) || (x==(nxbins-1));
            for (y=0, ai=a, ri=r, ni=n, tai=ta, tri=tr, tni=tn; y<nybins; y++, 
                 ai++, ri++, ni++, tai++, tri++, tni++) {
               
               c = hWvFreqVsEvt[i]->GetBinContent(x+1, y+1);
               
               *ai += c;
               *ri += c*c;
               *ni += 1.0;
               
               *tai += c;
               *tri += c*c;
               *tni += 1.0;
               
               if (doAve) {
                  evts=0;
                  if (y==0) {
                     gAveFreq = new TGraphErrors;
                     gAveFreq->SetName(Form("gAveFreq_ch%d_e%d",i,x));
                     gAveFreq->SetTitle(Form(";%s;%s",
                        hWvFreqVsEvt[i]->GetYaxis()->GetTitle(),
                        hWvFreqVsEvt[i]->GetZaxis()->GetTitle()));
                     gAveFreqs[i]->AddLast(gAveFreq);
                  }
                  
                  if (*ni > 1) {
                     *ai /= *ni;
                     *ri -= (*ai)*(*ai);
                     *ri /= *ni;
                     *ri  = TMath::Abs(*ri);
                  
                     gAveFreq->SetPoint(y, 
                        hWvFreqVsEvt[i]->GetYaxis()->GetBinCenter(y+1),
                        *ai);
                     gAveFreq->SetPointError(y,
                        hWvFreqVsEvt[i]->GetYaxis()->GetBinWidth(y+1) / 2.0,
                        (TMath::Sqrt(*ri)/(*ni))/2.0);
                  }
                  
                  *ai = *ri = *ni = 0;
               }
            }
         }
         
         
         for (y=0, tai=ta, tri=tr, tni=tn; 
              y<nybins; y++, tai++, tri++, tni++) {
            
            if (*tni > 1) {
               *tai /= *tni;
               *tri -= (*tai)*(*tai);
               *tri /= *tni;
               *tri  = TMath::Abs(*tri);
               
               gTotAveFreq[i]->SetPoint(y,
                  hWvFreqVsEvt[i]->GetYaxis()->GetBinCenter(y+1),
                  *tai);
               gTotAveFreq[i]->SetPointError(y,
                  hWvFreqVsEvt[i]->GetYaxis()->GetBinWidth(y+1) / 2.0,
                  (TMath::Sqrt(*tri)/(*tni))/2.0);
            }
            
            *tai = *tri = *tni = 0;
            
         }
         
         delete[] a;
         delete[] r;
         delete[] n;
         delete[] ta;
         delete[] tr;
         delete[] tn;
         
      }
      
      outf->cd();
      gAveFreqs[i]->Write(gAveFreqs[i]->GetName(), TObject::kSingleKey);
      gTotAveFreq[i]->Write(gTotAveFreq[i]->GetName(), TObject::kSingleKey);
      
   }
   
   
}
