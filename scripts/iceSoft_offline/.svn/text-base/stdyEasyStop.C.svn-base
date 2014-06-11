#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TGraphErrors.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TLatex.h>

#include <vector>

#endif

//#define DEBUG

enum EChBits_t {
   kCh0 = BIT(0),
   kCh1 = BIT(1),
   kCh2 = BIT(2),
   kCh3 = BIT(3)
};

// 2011 station
static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;
static const Int_t   kNextra    = 57;
static const Int_t   kTrgBitIdx = 10;
static const Float_t kSmpRate   = 1.92; // GHz

const Int_t  col[4] = {kOrange+7, kAzure-2, kViolet-1, kGreen+2};


const Int_t nwidbins = 21,
            nnstbins = 21,
            noffbins = 257,
            nofmbins = 129,
            npsabins = 121;
const Float_t widmin=-0.5, widmax=20.5,
              nstmin=-0.5, nstmax=20.5,
              offmin=-128.5, offmax=128.5,
              ofmmin=-64.5, ofmmax=64.5,
              psamin=-60.5, psamax=60.5;


TChain* nt;
TFile* outf, * pedf;

TGraphErrors* gPed[kNchans], * gNoise[kNchans];

TCanvas* cdbg, * c1, * c2, * c3, * c4, * c5, * c6;

TH2* hPedSubWvVsEvt[kNchans], * hStopAdcVsSamp[kNchans];

TH1* hStopWidth[kNchans], * hStopBegin[kNchans],
   * hNumStops[kNchans],  * hStopAdc[kNchans],
   * hAveStop[kNchans],   * hCntStop[kNchans],
   * hStopDistM[kNchans][kNchans];


void setStyle(TH1* h, const Int_t typ) {
   if (h!=0) {
      h->SetLineColor(col[typ]);
      h->SetMarkerColor(col[typ]);
   }
}

Bool_t isActive(const Int_t ch, const Int_t activeChs) {
   return ( BIT(ch) & activeChs )!=0;
}


void stdyEasyStop(const Char_t* infn,
                  const Char_t* outfn,
                  const Int_t   activeChs=(kCh0 | kCh2),
                  const Int_t   trg=-1,
                  const Bool_t  verifyChksum=kTRUE,
                  const Char_t* pedfn=
                     "/home/cjreed/work/AriannaData/icicle/run481/"
                     "stompedPeds.root") {
   
   
   // trg==-1 => all triggers
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   //gStyle->SetOptStat(0);
   
   // open the tree
   nt = new TChain("nt");
   nt->Add(infn);
   
   const Long64_t nents = nt->GetEntries();
   Printf("Entries in tree = %lld",nents);
   
   // open the pedestal file
   pedf = TFile::Open(pedfn);
   if (pedf!=0 && pedf->IsZombie()==kFALSE) {
      for (Int_t ch=0; ch<kNchans; ch++) {
         gPed[ch]   = dynamic_cast<TGraphErrors*>(
            pedf->Get(Form("gExlPed_ch%d",ch)));
         gNoise[ch] = dynamic_cast<TGraphErrors*>(
            pedf->Get(Form("gExlRms_ch%d",ch)));
         if ( (gPed[ch]==0) || (gNoise[ch]==0) ) {
            Printf("Couldn't get ped/noise graphs from [%s]",pedfn);
            return;
         }
      }
   } else {
      Printf("Could not open ped file [%s]",pedfn);
      return;
   }
   
   // open the output file (to store histograms)
   outf = TFile::Open(outfn,"recreate");
   
   // read from tree into...
   UShort_t samples[kNchans][kNsamps];
   UShort_t extra[kNextra];
   UInt_t   evnum, utime, utimeus, mbchksum;
   
   // setup tree, make hists
   TString hn;
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("EvtNum",&evnum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("extra",&(extra[0]));
   for (Int_t ch=0; ch<kNchans; ch++) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
      
      hn = Form("hPedSubWvVsEvt_ch%d",ch);
      hPedSubWvVsEvt[ch] = new TH2F(hn.Data(),
         Form("ped. sub. waveforms (chan %d);event;sample;ADC-ped",ch),
         nents, -0.5, nents-0.5,
         kNsamps, -0.5, kNsamps-0.5);
      
      
      hn = Form("hStopWidth_ch%d",ch);
      hStopWidth[ch] = new TH1F(hn.Data(),
         Form("Width of stop candidates (chan %d);width (ADCs)",ch),
         nwidbins, widmin, widmax);
      setStyle(hStopWidth[ch],ch);
      
      hn = Form("hStopBegin_ch%d",ch);
      hStopBegin[ch] = new TH1F(hn.Data(),
         Form("Start of stop candidates (chan %d);sample",ch),
         kNsamps, -0.5, kNsamps-0.5);
      hStopBegin[ch]->Sumw2();
      setStyle(hStopBegin[ch],ch);
      
      hn = Form("hNumStops_ch%d",ch);
      hNumStops[ch] = new TH1F(hn.Data(),
         Form("Number of stop candidates per event (chan %d)"
               ";num stop candidates;fraction events",ch),
         nnstbins, nstmin, nstmax);
      setStyle(hNumStops[ch],ch);
      
      hn = Form("hStopAdc_ch%d",ch);
      hStopAdc[ch] = new TH1F(hn.Data(),
         Form("Ped. sub. ADC distribution of stop samples (chan %d)"
               ";stop ADC - ped.",ch),
         npsabins, psamin, psamax);
      hStopAdc[ch]->SetBit(TH1::kCanRebin);
      setStyle(hStopAdc[ch],ch);
      
      hn = Form("hStopAdcVsSamp_ch%d",ch);
      hStopAdcVsSamp[ch] = new TH2F(hn.Data(),
         Form("Ped. sub. ADC distribution of stop samples (chan %d)"
               ";stop begin pos. (sample);stop ADC - ped.",ch),
         kNsamps, -0.5, kNsamps-0.5,
         npsabins, psamin, psamax);
      hStopAdcVsSamp[ch]->SetBit(TH1::kCanRebin);
      setStyle(hStopAdcVsSamp[ch],ch);
      
      hn = Form("hAveStop_ch%d",ch);
      hAveStop[ch] = new TH1F(hn.Data(),
         Form("Average ADC - ped. of samples in stops (chan %d)"
               ";stop ADC - ped.",ch),
         kNsamps, -0.5, kNsamps-0.5);
      hAveStop[ch]->Sumw2();
      hAveStop[ch]->SetMarkerStyle(7);
      hAveStop[ch]->SetLineWidth(1);
      setStyle(hAveStop[ch],ch);
      
      hn = Form("hCntStop_ch%d",ch);
      hCntStop[ch] = new TH1F(hn.Data(),
         Form("Average ADC - ped. of samples in stops (chan %d)"
               ";stop ADC - ped.",ch),
         kNsamps, -0.5, kNsamps-0.5);
      //hCntStop[ch]->Sumw2();
      setStyle(hCntStop[ch],ch);
            
      for (Int_t nc=0; nc<ch; nc++) {
         /*
         hn = Form("hStopDist_ch%d_nc%d",ch,nc);
         hStopDist[ch][nc] = new TH1F(hn.Data(),
            Form("Offset of stop candidates per event (chan %d - chan %d)"
                  ";ch%d - ch%d;num events",ch,nc,ch,nc),
            noffbins, offmin, offmax);
         //hStopDist[ch][nc]->SetBit(TH1::kCanRebin);
         setStyle(hStopDist[ch][nc],ch);
         */
         hn = Form("hStopDistM_ch%d_nc%d",ch,nc);
         hStopDistM[ch][nc] = new TH1F(hn.Data(),
            Form("Offset of stop candidates per event (chan %d - chan %d)"
                  ";ch%d - ch%d (modulo 64);num events",ch,nc,ch,nc),
            nofmbins, ofmmin, ofmmax);
         //hStopDistM[ch][nc]->SetBit(TH1::kCanRebin);
         setStyle(hStopDistM[ch][nc],ch);
      }
      
   }
   
   Bool_t* gevts = new Bool_t[nents];
   Double_t selevts=0;
   
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e);
      
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
         selevts+=1.0;
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               const Double_t* const peds = gPed[ch]->GetY();
               const Double_t*       p    = peds;
               for (Int_t sm=0; sm<kNsamps; sm++, p++) {
                  const Double_t dev = samples[ch][sm] - (*p);
                  hPedSubWvVsEvt[ch]->Fill(e, sm, dev);
               }
            }
         }
         
         gevts[e] = kTRUE;
      } else {
         gevts[e] = kFALSE;
      }
   }
   
#ifdef DEBUG
   cdbg = new TCanvas;
#endif
   
   Printf("Found %g good events",selevts);
   
   for (Int_t i=0; i<nents; i++) {
      if (gevts[i]) {
         
         const Int_t ebin = i+1;
         std::vector< std::vector<Int_t> > stopLocs,
                                           goodLocs; // [ch][stop#]
         stopLocs.resize(4);
         goodLocs.resize(4);
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               TH1* hp = hPedSubWvVsEvt[ch]->ProjectionY("_py", ebin, ebin);
#ifdef DEBUG
               cdbg->Clear();
               cdbg->cd();
               hp->Draw();
               cdbg->Update();
#endif
               
               for (Int_t xb=1; xb<=hp->GetNbinsX(); xb++) {
                  const Float_t cstrt = hp->GetBinContent(xb);
                  const Float_t xbsmp = hp->GetBinCenter(xb);
                  if (cstrt>100.) {
                     Int_t swid=1;
                     for (Int_t j=swid; j<kNsamps; j++) {
                        Int_t nb = xb+j;
                        if (nb>hp->GetNbinsX()) {
                           nb -= hp->GetNbinsX();
                        }
                        const Float_t cn = hp->GetBinContent(nb);
                        if (cn>100.) {
                           swid++;
                        } else {
                           break;
                        }
                     }
                     if (swid>=9) {
                        stopLocs[ch].push_back(xb);
                        hStopBegin[ch]->Fill(xbsmp);
                        hStopWidth[ch]->Fill(swid);
                        for (Int_t bb=0; bb<swid; bb++) {
                           Int_t nb = xb+bb;
                           if (nb>hp->GetNbinsX()) {
                              nb -= hp->GetNbinsX();
                           }
                           const Float_t cc = hp->GetBinContent(nb);
                           const Float_t cb = hp->GetBinCenter(nb);
                           hStopAdc[ch]->Fill(cc);
                           hStopAdcVsSamp[ch]->Fill(xbsmp, cc);
                           hAveStop[ch]->Fill(cb, cc);
                           hCntStop[ch]->Fill(cb, 1.0);
                        }
                     }
                  }
               }

#ifdef DEBUG
               cdbg->WaitPrimitive();
#endif
               delete hp;
      
            }
         }
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               const UInt_t nsch = stopLocs[ch].size();
               hNumStops[ch]->Fill(nsch);
               for (Int_t nc=0; nc<ch; nc++) {
                  if (isActive(nc, activeChs)) {
                     const UInt_t nsnc = stopLocs[nc].size();
                     for (UInt_t i=0; i<nsch; i++) {
                        for (UInt_t j=0; j<nsnc; j++) {
                           Int_t sof = stopLocs[ch][i] - stopLocs[nc][j];
                           //hStopDist[ch][nc]->Fill( sof );
                           if (TMath::Abs(sof)>64) {
                              sof = (stopLocs[ch][i] < stopLocs[nc][j]) ?
                                    ((stopLocs[ch][i]+128) - stopLocs[nc][j]) :
                                    (stopLocs[ch][i] - (stopLocs[nc][j]+128));
                           }
                           hStopDistM[ch][nc]->Fill( sof );
                        }
                     }
                  }
               }
               
            }
         }
         
      }
   }
   
   for (Int_t ch=0; ch<kNchans; ch++) {
      if (isActive(ch, activeChs)) {
         hAveStop[ch]->Divide(hCntStop[ch]);
         hNumStops[ch]->Scale(1.0 / selevts);
      }
   }
   
   Printf("\nprocessed %g events",selevts);
   
   delete[] gevts;
   
   c1 = new TCanvas("c1","c1",800,500);
   c1->Divide(2,1);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c1->cd(1);
      hStopWidth[ch]->Draw((ch==0)?"":"same");
      c1->cd(2);
      hNumStops[ch]->SetMaximum(0.8);
      hNumStops[ch]->SetMinimum(0);
      hNumStops[ch]->Draw((ch==0)?"":"same");
   }
   c1->cd();
   c1->Update();
   
   c6 = new TCanvas("c6","c6",800,500);
   c6->cd();
   for (Int_t ch=0; ch<kNchans; ch++) {
      hStopBegin[ch]->Draw((ch==0)?"":"same");
   }
   c6->cd();
   c6->Update();
   
   
   c2 = new TCanvas("c2","c2",800,500);
   c2->cd();
   c2->SetLogy();
   //c2->Divide(2,3);
   //Int_t q=0;
   TLatex* lat = new TLatex;
   lat->SetTextColor(kRed+2);
   lat->SetNDC();
   for (Int_t ch=0; ch<kNchans; ch++) {
      for (Int_t nc=0; nc<ch; nc++) {
         if (ch==1 && nc==0) {
            //c2->cd(++q);
            hStopDistM[ch][nc]->GetXaxis()->SetNdivisions(515);
            hStopDistM[ch][nc]->Draw();
            //c2->GetPad(q)->SetLogy();
            const Double_t lo = hStopDistM[ch][nc]->Integral(
               hStopDistM[ch][nc]->FindBin(ofmmin+0.5),
               hStopDistM[ch][nc]->FindBin(0));
            const Double_t hi = hStopDistM[ch][nc]->Integral(
               hStopDistM[ch][nc]->FindBin(0),
               hStopDistM[ch][nc]->FindBin(ofmmax+0.5));
            const Double_t re = (hi/lo)*TMath::Sqrt(
               ( (TMath::Sqrt(lo)*TMath::Sqrt(lo)/(lo*lo)) +
                 (TMath::Sqrt(hi)*TMath::Sqrt(hi)/(hi*hi)) ) );
            lat->DrawLatex(0.3,0.2,
               Form("N(>0) / N(<0) = %2.2f #pm %1.1e",hi/lo,re));
         }
      }
   }
   c2->SetGridx();
   c2->cd();
   
   c3 = new TCanvas("c3","c3",750,900);
   c3->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c3->cd(ch+1);
      hStopAdc[ch]->Draw();
      c3->GetPad(ch+1)->SetLogy();
   }
   c3->cd();
   c3->Update();
   
   c4 = new TCanvas("c4","c4",750,900);
   c4->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c4->cd(ch+1);
      hStopAdcVsSamp[ch]->Draw("colz");
   }
   c4->cd();
   c4->Update();
   
   
   c5 = new TCanvas("c5","c5",750,900);
   c5->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c5->cd(ch+1);
      hAveStop[ch]->Draw();
   }
   c5->cd();
   c5->Update();
   
   
   outf->Write();
}
