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

TGraphErrors* gPed[kNchans], * gNoise[kNchans];

TH1* hPlsWidth[kNchans], * hNumPls[kNchans], * hNumFulPls[kNchans],
   * hPlsDelBeg[kNchans], * hPlsDelEnd[kNchans],
   * hOffBeg[kNchans][kNchans], * hOffEnd[kNchans][kNchans],
   * hPlsBeg[kNchans], * hPlsEnd[kNchans];

TH2* hPedSubWvVsEvt[kNchans],
   * hPlsAdcVsSamp[kNchans];

TCanvas* c1, * c2, * c3, * c4, * c5, * c6,
       * cdbg;

const Int_t nwidbins = 21,
            nnstbins = 21,
            noffbins = 257,
            ndelbins = 129,
            npsabins = 1001;
const Float_t widmin=-0.5,   widmax=20.5,
              nstmin=-0.5,   nstmax=20.5,
              offmin=-128.5, offmax=128.5,
              delmin=-0.5,   delmax=128.5,
              psamin=-100.5, psamax=900.5;


const Int_t  col[4] = {kOrange+7, kAzure-2, kViolet-1, kGreen+2};

TChain* nt;
TFile* outf, * pedf;


void setStyle(TH1* h, const Int_t typ) {
   if (h!=0) {
      h->SetLineColor(col[typ]);
   }
}

Bool_t isActive(const Int_t ch, const Int_t activeChs) {
   return ( BIT(ch) & activeChs )!=0;
}

void twoPulseStdy(const Char_t* infn,
                  const Char_t* outfn,
                  const Float_t plsCut=50.,
                  const Int_t   fulPlsWid=8,
                  const Int_t   activeChs=(kCh0 | kCh1),
                  const Int_t   trg=0,
                  const Bool_t  verifyChksum=kTRUE,
                  const Char_t* pedfn="/home/cjreed/work/AriannaData/replica/"
                                  "pulserStdy2Hz/peds.pulserStdy2Hz.all.root") {
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
      
      hn = Form("hPlsWidth_ch%d",ch);
      hPlsWidth[ch] = new TH1F(hn.Data(),
         Form("pulse width (chan %d);pulse width",ch),
         nwidbins, widmin, widmax);
      setStyle(hPlsWidth[ch],ch);
      
      hn = Form("hNumPls_ch%d",ch);
      hNumPls[ch] = new TH1F(hn.Data(),
         Form("number of pulses per event (chan %d);num. pulses",ch),
         nnstbins, nstmin, nstmax);
      setStyle(hNumPls[ch],ch);
      
      hn = Form("hNumFulPls_ch%d",ch);
      hNumFulPls[ch] = new TH1F(hn.Data(),
         Form("num of full pulses per event (chan %d)"
              ";num. pulses w/ width >= %d", ch, fulPlsWid),
         nnstbins, nstmin, nstmax);
      setStyle(hNumFulPls[ch],ch);
      
      hn = Form("hPlsDelBeg_ch%d",ch);
      hPlsDelBeg[ch] = new TH1F(hn.Data(),
         Form("distance between pulse begins in an event (chan %d)"
              ";pulse begin #Delta (ch%d)",ch,ch),
         ndelbins, delmin, delmax);
      setStyle(hPlsDelBeg[ch], ch);
      
      hn = Form("hPlsDelEnd_ch%d",ch);
      hPlsDelEnd[ch] = new TH1F(hn.Data(),
         Form("distance between pulse ends in an event (chan %d)"
              ";pulse end #Delta (ch%d)",ch,ch),
         ndelbins, delmin, delmax);
      setStyle(hPlsDelEnd[ch], ch);
      
      for (Int_t nc=0; nc<kNchans; nc++) {
         hn = Form("hOffBeg_ch%d_nc%d",ch,nc);
         hOffBeg[ch][nc] = new TH1F(hn.Data(),
            Form("distance between pulse begins in an event (ch%d-nc%d)"
                 ";pulse begins offset (single chan)",ch,nc),
            noffbins, offmin, offmax);
         setStyle(hOffBeg[ch][nc], ch);
         
         hn = Form("hOffEnd_ch%d_nc%d",ch,nc);
         hOffEnd[ch][nc] = new TH1F(hn.Data(),
            Form("distance between pulse ends in an event (ch%d-nc%d)"
                 ";pulse ends offset (single chan)",ch,nc),
            noffbins, offmin, offmax);
         setStyle(hOffEnd[ch][nc], ch);
      }
      
      hn = Form("hPlsBeg_ch%d",ch);
      hPlsBeg[ch] = new TH1F(hn.Data(),
         Form("pulse begin location (chan %d);pulse begin",ch),
         kNsamps, -0.5, kNsamps-0.5);
      setStyle(hPlsBeg[ch], ch);
      
      hn = Form("hPlsEnd_ch%d",ch);
      hPlsEnd[ch] = new TH1F(hn.Data(),
         Form("pulse end location (chan %d);pulse end",ch),
         kNsamps, -0.5, kNsamps-0.5);
      setStyle(hPlsEnd[ch], ch);
      
      hn = Form("hPedSubWvVsEvt_ch%d",ch);
      hPedSubWvVsEvt[ch] = new TH2F(hn.Data(),
         Form("ped. sub. waveforms (chan %d);event;sample;ADC-ped",ch),
         nents, -0.5, nents-0.5,
         kNsamps, -0.5, kNsamps-0.5);
      
      hn = Form("hPlsAdcVsSamp_ch%d",ch);
      hPlsAdcVsSamp[ch] = new TH2F(hn.Data(),
         Form("Ped. sub. ADC distribution of pulse samples (chan %d)"
               ";sample;stop ADC - ped.",ch),
         kNsamps, -0.5, kNsamps-0.5,
         npsabins, psamin, psamax);
      hPlsAdcVsSamp[ch]->SetBit(TH1::kCanRebin);
      setStyle(hPlsAdcVsSamp[ch],ch);
      
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
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               const Double_t* const peds = gPed[ch]->GetY();
               const Double_t*       p    = peds;
               for (Int_t sm=0; sm<kNsamps; sm++, p++) {
                  hPedSubWvVsEvt[ch]->Fill(e, sm, samples[ch][sm] - (*p));
               }
            }
         }
         
         gevts[e] = kTRUE;
         selevts+=1.0;
      } else {
         gevts[e] = kFALSE;
      }
   }
   
   Printf("\nFound %g good events.",selevts);
   
   for (Int_t i=0; i<nents; i++) {
      if (gevts[i]) {
         
         const Int_t ebin = i+1;
         std::vector< std::vector< std::pair<Int_t, Int_t> > > plsLocWid;
         plsLocWid.resize(4);
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               TH1* hp = hPedSubWvVsEvt[ch]->ProjectionY("_py", ebin, ebin);
               for (Int_t xb=1; xb<=hp->GetNbinsX(); xb++) {
                  const Float_t cstrt = hp->GetBinContent(xb);
                  const Float_t xbsmp = hp->GetBinCenter(xb);
                  
                  if (cstrt > plsCut) {
                     Int_t swid=0;
                     for (Int_t j=swid; j<kNsamps; j++) {
                        Int_t nb = xb+j;
                        if (nb>hp->GetNbinsX()) {
                           nb -= hp->GetNbinsX();
                        }
                        const Float_t cn = hp->GetBinContent(nb);
                        if (cn > plsCut) {
                           hPlsAdcVsSamp[ch]->Fill(xbsmp, cn);
                           swid++;
                        } else {
                           break; // no longer in the pulse
                        }
                     }
                     plsLocWid.at(ch).push_back(
                        std::pair<Int_t,Int_t>(xbsmp,swid) );
                     xb += swid; // move up past the pulse
                  }
               }
            }
         }
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               
               const UInt_t npulses = plsLocWid.at(ch).size();
               
               hNumPls[ch]->Fill(npulses);
               
               Int_t nfulpls = 0;
               for (Int_t i=0; i<npulses; i++) {
                  std::pair<Int_t, Int_t>& plw = plsLocWid.at(ch).at(i);
                  const Int_t pbeg = plw.first;
                  const Int_t pwid = plw.second;
                  const Int_t pend = pbeg+pwid-1;
                  
                  hPlsWidth[ch]->Fill(pwid);
                  
                  const Bool_t pful = pwid>=fulPlsWid;
                  
                  if (pful) {
                     nfulpls++;
                     hPlsBeg[ch]->Fill(pbeg);
                     hPlsEnd[ch]->Fill(pend);
                     
                     // other pulses in this channel
                     for (Int_t j=0; j<i; j++) {
                        std::pair<Int_t, Int_t>& qlw = plsLocWid.at(ch).at(j);
                        const Int_t qbeg = qlw.first;
                        const Int_t qwid = qlw.second;
                        const Int_t qend = qbeg+qwid-1;

                        if (qwid>=fulPlsWid) {
                           hPlsDelBeg[ch]->Fill(pbeg - qbeg);
                           hPlsDelEnd[ch]->Fill(pend - qend);
                        }
                     }
                  
                     // other channels
                     for (Int_t nc=0; nc<ch; nc++) {
                        if (isActive(nc, activeChs)) {
                           const UInt_t ncnp = plsLocWid.at(nc).size();

                           for (Int_t k=0; k<ncnp; k++) {
                              std::pair<Int_t, Int_t>& klw = 
                                 plsLocWid.at(nc).at(k);
                              const Int_t kbeg = klw.first;
                              const Int_t kwid = klw.second;
                              const Int_t kend = kbeg+kwid-1;
                              
                              if (kwid>=fulPlsWid) {
                                 hOffBeg[ch][nc]->Fill(pbeg - kbeg);
                                 hOffEnd[ch][nc]->Fill(pend - kend);
                              }
                           }

                        }
                     }
                  }
                  
               }
               
               hNumFulPls[ch]->Fill(nfulpls);
            }
         }
         
      }
   }
   
   c1 = new TCanvas("c1","c1 - num, wid",800,800);
   c1->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c1->cd(1);
      hNumPls[ch]->Draw((ch==0)?"":"same");
      c1->cd(2);
      hNumFulPls[ch]->Draw((ch==0)?"":"same");
      c1->cd(3);
      hPlsWidth[ch]->Draw((ch==0)?"":"same");
   }
   c1->cd();
   c1->Update();
   
   c2 =  new TCanvas("c2","c2 - deltas",800,500);
   //c2->Divide(2,1);
   for (Int_t ch=0; ch<kNchans; ch++) {
      //c2->cd(1);
      //hPlsDelBeg[ch]->Draw((ch==0)?"":"same");
      //c2->cd(2);
      hPlsDelEnd[ch]->Draw((ch==0)?"":"same");
   }
   c2->cd();
   c2->Update();
   /*
   c3 = new TCanvas("c3","c3 - begin offsets",750,900);
   c3->Divide(2,3);
   Int_t q=0;
   TLatex* lat = new TLatex;
   lat->SetTextColor(kRed+2);
   lat->SetNDC();
   for (Int_t ch=0; ch<kNchans; ch++) {
      for (Int_t nc=0; nc<ch; nc++) {
         c3->cd(++q);
         hOffBeg[ch][nc]->Draw();
         c3->GetPad(q)->SetLogy();
         const Double_t lo = hOffBeg[ch][nc]->Integral(1,
            hOffBeg[ch][nc]->FindBin(0));
         const Double_t hi = hOffBeg[ch][nc]->Integral(
            hOffBeg[ch][nc]->FindBin(0),
            hOffBeg[ch][nc]->GetNbinsX());
         const Double_t re = (hi/lo)*TMath::Sqrt((1.0/lo) + (1.0/hi));
         lat->DrawLatex(0.23,0.2,
            Form("N(>0) / N(<0) = %2.2f #pm %1.1e",hi/lo,re));
      }
   }
   c3->cd();
   c3->Update();
   */
   c4 = new TCanvas("c4","c4 - end offsets",800,500);
   /*
   c4->Divide(2,3);
   q=0;
   for (Int_t ch=0; ch<kNchans; ch++) {
      for (Int_t nc=0; nc<ch; nc++) {
         c4->cd(++q);
         hOffEnd[ch][nc]->Draw();
         c4->GetPad(q)->SetLogy();
         const Double_t lo = hOffEnd[ch][nc]->Integral(1,
            hOffEnd[ch][nc]->FindBin(0));
         const Double_t hi = hOffEnd[ch][nc]->Integral(
            hOffEnd[ch][nc]->FindBin(0),
            hOffEnd[ch][nc]->GetNbinsX());
         const Double_t re = (hi/lo)*TMath::Sqrt((1.0/lo) + (1.0/hi));
         lat->DrawLatex(0.23,0.2,
            Form("N(>0) / N(<0) = %2.2f #pm %1.1e",hi/lo,re));
      }
   }
   */
   hOffEnd[1][0]->Draw();
   c4->cd();
   c4->Update();
   
   c5 = new TCanvas("c5","c5 - locs",800,500);
   c5->Divide(2,1);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c5->cd(1);
      hPlsBeg[ch]->Draw((ch==0)?"":"same");
      c5->cd(2);
      hPlsEnd[ch]->Draw((ch==0)?"":"same");
   }
   c5->cd();
   c5->Update();
   
   
   c6 = new TCanvas("c6","c6 - adc vs samp",800,800);
   c6->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c6->cd(ch+1);
      hPlsAdcVsSamp[ch]->Draw("colz");
   }
   c6->cd();
   c6->Update();
   
   
   
   
   outf->Write();
}
