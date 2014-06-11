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
#include <TFitResult.h>

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
static const Float_t kSmpRate   = 1.92e9; // Hz


const Int_t nwidbins = 21,
            nnstbins = 21,
            noffbins = 257,
            nofmbins = 129,
            npsabins = 401,
            nresbins = 100,
            nadcbins = 500;
const Float_t widmin=-0.5, widmax=20.5,
              nstmin=-0.5, nstmax=20.5,
              offmin=-128.5, offmax=128.5,
              ofmmin=-64.5, ofmmax=64.5,
              psamin=-350.5, psamax=-50.5,
              resmin=-200., resmax=200.,
              adcmin=-1000., adcmax=1000.;

const Float_t loFitMin = 10, loFitMax = 50,
              hiFitMin = 75, hiFitMax = 115;


TGraphErrors* gPed[kNchans], * gNoise[kNchans];

TH2* hPedSubWvVsEvt[kNchans], * hInARowVsEvt[kNchans],
   * hStopAdcVsSamp[kNchans], * hStopAdcVsEvt[kNchans],
   * hResUp[kNchans], * hResDn[kNchans],
   * hAdcVsEvt[kNchans];

TH1* hStopWidth[kNchans], * hStopBegin[kNchans],
   * hNumStops[kNchans],  * hNumStopsS[kNchans][2],
   * hStopDist[kNchans][kNchans], * hStopDistM[kNchans][kNchans],
   * hStopAdc[kNchans],   * hStopAdcS[kNchans][2];

THStack* sStopAdcS[kNchans];

const Int_t  col[4] = {kOrange+7, kAzure-2, kViolet-1, kGreen+2};

TChain* nt;
TFile* outf, * pedf;
TCanvas* c1, * c2, * c3, * c4, * c5, * c6, * c7, * c8, * c9, * c10,
       * c11, * c12, * c13, * c14, * cdbg;


void setStyle(TH1* h, const Int_t typ) {
   if (h!=0) {
      h->SetLineColor(col[typ]);
   }
}

Bool_t isActive(const Int_t ch, const Int_t activeChs) {
   return ( BIT(ch) & activeChs )!=0;
}

void sineWvStop(const Char_t* infn,
                const Char_t* outfn,
                const Int_t   stpNumChn=5,  // min num of stop samples in a row
                const Float_t wvFreq=20e6,  // sine wave frequency (Hz)
                const Float_t stpRes=50.,   // min residual to be in stop
                const Float_t stpLoc=-240., // usual location of stop in ADCs
                const Float_t stpWin=50.,   // window around stop location
                const Int_t   activeChs=(kCh0 | kCh1),
                const Int_t   trg=0,
                const Bool_t  verifyChksum=kTRUE,
                const Char_t* pedfn="/home/cjreed/work/AriannaData/replica/"
                                  "pulserStdy2Hz/peds.pulserStdy2Hz.all.root") {
   // trg==-1 => all triggers
   
   const Float_t angfreq = TMath::TwoPi() / (kSmpRate/wvFreq);
   
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
      
      hn = Form("hInARowVsEvt_ch%d",ch);
      hInARowVsEvt[ch] = new TH2F(hn.Data(),
         Form("start of %d in a row (chan %d);event;sample",stpNumChn,ch),
         nents, -0.5, nents-0.5,
         kNsamps, -0.5, kNsamps-0.5);
      
      hn = Form("hStopWidth_ch%d",ch);
      hStopWidth[ch] = new TH1F(hn.Data(),
         Form("Width of stop candidates (chan %d);width (ADCs)",ch),
         nwidbins, widmin, widmax);
      hStopWidth[ch]->SetBit(TH1::kCanRebin);
      setStyle(hStopWidth[ch],ch);
      
      hn = Form("hStopBegin_ch%d",ch);
      hStopBegin[ch] = new TH1F(hn.Data(),
         Form("Start of stop candidates (chan %d);sample",ch),
         kNsamps, -0.5, kNsamps-0.5);
      setStyle(hStopBegin[ch],ch);
      
      hn = Form("hNumStops_ch%d",ch);
      hNumStops[ch] = new TH1F(hn.Data(),
         Form("Number of stop candidates per event (chan %d)"
               ";num stop candidates;fraction events",ch),
         nnstbins, nstmin, nstmax);
      hNumStops[ch]->SetBit(TH1::kCanRebin);
      setStyle(hNumStops[ch],ch);
      
      for (Int_t i=0; i<2; i++) {
         hn = Form("hNumStopsS_ch%d_g%d",ch,i);
         hNumStopsS[ch][i] = new TH1F(hn.Data(),
            Form("Number of stop cand. in '%s' slots per event (chan %d)"
                  ";num stop candidates;fraction events",
                  (i==0)?"good":"bad",ch),
            nnstbins, nstmin, nstmax);
         hNumStopsS[ch][i]->SetBit(TH1::kCanRebin);
         setStyle(hNumStopsS[ch][i],ch);
      }
      
      for (Int_t nc=0; nc<ch; nc++) {
         hn = Form("hStopDist_ch%d_nc%d",ch,nc);
         hStopDist[ch][nc] = new TH1F(hn.Data(),
            Form("Offset of stop candidates per event (chan %d - chan %d)"
                  ";ch%d - ch%d;num events",ch,nc,ch,nc),
            noffbins, offmin, offmax);
         //hStopDist[ch][nc]->SetBit(TH1::kCanRebin);
         setStyle(hStopDist[ch][nc],ch);
         
         hn = Form("hStopDistM_ch%d_nc%d",ch,nc);
         hStopDistM[ch][nc] = new TH1F(hn.Data(),
            Form("Offset of stop candidates per event (chan %d - chan %d)"
                  ";ch%d - ch%d (modulo 64);num events",ch,nc,ch,nc),
            nofmbins, ofmmin, ofmmax);
         //hStopDistM[ch][nc]->SetBit(TH1::kCanRebin);
         setStyle(hStopDistM[ch][nc],ch);
      }
      
      hn = Form("hStopAdc_ch%d",ch);
      hStopAdc[ch] = new TH1F(hn.Data(),
         Form("Ped. sub. ADC distribution of stop samples (chan %d)"
               ";stop ADC - ped.",ch),
         npsabins, psamin, psamax);
      hStopAdc[ch]->SetBit(TH1::kCanRebin);
      setStyle(hStopAdc[ch],ch);
      
      for (Int_t i=0; i<2; i++) {
         hn = Form("hStopAdcS_ch%d_g%d",ch,i);
         hStopAdcS[ch][i] = new TH1F(hn.Data(),
            Form("Ped. sub. stop samples in '%s' slots (chan %d)"
                  ";stop ADC - ped.",(i==0)?"good":"bad",ch),
            npsabins, psamin, psamax);
         hStopAdcS[ch][i]->SetBit(TH1::kCanRebin);
         setStyle(hStopAdcS[ch][i],ch);
         /*
         if (i==1) {
            hStopAdcS[ch][i]->SetLineStyle(7);
         }
         */
      }
      
      hn = Form("hStopAdcVsSamp_ch%d",ch);
      hStopAdcVsSamp[ch] = new TH2F(hn.Data(),
         Form("Ped. sub. ADC distribution of stop samples (chan %d)"
               ";stop begin pos. (sample);stop ADC - ped.",ch),
         kNsamps, -0.5, kNsamps-0.5,
         npsabins, psamin, psamax);
      //hStopAdcVsSamp[ch]->SetBit(TH1::kCanRebin);
      setStyle(hStopAdcVsSamp[ch],ch);
      
      
      hn = Form("hResUp_ch%d",ch);
      hResUp[ch] = new TH2F(hn.Data(),
         Form("Residuals during upgoing search (chan %d);event;ADC-fit",ch),
         nents, -0.5, nents-0.5,
         nresbins, resmin, resmax);
      
      hn = Form("hResDn_ch%d",ch);
      hResDn[ch] = new TH2F(hn.Data(),
         Form("Residuals during downgoing search (chan %d);event;ADC-fit",ch),
         nents, -0.5, nents-0.5,
         nresbins, resmin, resmax);
      
      hn = Form("hStopAdcVsEvt_ch%d",ch);
      hStopAdcVsEvt[ch] = new TH2F(hn.Data(),
         Form("ADC-pedestal of samples in the stop (chan %d);event;ADC-ped",ch),
         nents, -0.5, nents-0.5,
         npsabins, psamin, psamax);
      
      hn = Form("hAdcVsEvt_ch%d",ch);
      hAdcVsEvt[ch] = new TH2F(hn.Data(),
         Form("ADC-pedestal of all samples (chan %d);event;ADC-ped",ch),
         nents, -0.5, nents-0.5,
         nadcbins, adcmin, adcmax);
      
   }
   
   Bool_t* gevts = new Bool_t[nents];
   
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e);
      
      if ( (e%500)==0 ) {
         fprintf(stderr,"Reading %lld / %lld  (%02.2f%%)            \r",
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
                  hAdcVsEvt[ch]->Fill(e, samples[ch][sm] - (*p));
               }
            }
         }
         
         gevts[e] = kTRUE;
      } else {
         gevts[e] = kFALSE;
      }
   }
   /*
   c1 = new TCanvas("c1","c1",800,800);
   c1->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c1->cd(ch+1);
      hPedSubWvVsEvt[ch]->Draw("colz");
   }
   */
#ifdef DEBUG
   cdbg = new TCanvas;
#endif
   
   Printf("\nFinding stops...");
   
   Double_t selevts=0;
   for (Int_t i=0; i<nents; i++) {
      if ( (i%50)==0 ) {
         fprintf(stderr,"Processing %d / %lld  (%02.2f%%)            \r",
            i, nents,
            100.*static_cast<Float_t>(i)/static_cast<Float_t>(nents));
      }
      
      if (gevts[i]) {
         selevts+=1.0;
         
         const Int_t ebin = i+1;
         std::vector< std::vector<Int_t> > stopLocs,
                                           goodLocs; // [ch][stop#]
         stopLocs.resize(4);
         goodLocs.resize(4);
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               TH1* hp = hPedSubWvVsEvt[ch]->ProjectionY("_py", ebin, ebin);
               
               TF1 ft1("ft1","([0]*sin(([1]*x)+[2]))");
               ft1.FixParameter(1,angfreq);
               ft1.SetParameter(0,500);
               ft1.SetRange(loFitMin, loFitMax);
               ft1.SetLineColor(kGreen+2);
               
               TF1 ft2("ft2","([0]*sin(([1]*x)+[2]))");
               ft2.FixParameter(1,angfreq);
               ft2.SetParameter(0,500);
               ft2.SetRange(hiFitMin, hiFitMax);
               ft2.SetLineColor(kAzure-6);
               
               TFitResultPtr fr1 = hp->Fit(&ft1,"QRS+");
               TFitResultPtr fr2 = hp->Fit(&ft2,"QRS+");
               const Double_t pr1 = fr1->Chi2() / fr1->Ndf(),
                              pr2 = fr2->Chi2() / fr2->Ndf();
#ifdef DEBUG
               Printf("prob1=%g, prob2=%g", pr1, pr2);
#endif
               
               TF1* fit = (pr1<pr2) ? hp->GetFunction("ft1")
                                    : hp->GetFunction("ft2");
               
               // assume the stop is not inside the well fit region
               const Int_t upBin = hp->FindBin(fit->GetXmax()),
                           dnBin = hp->FindBin(fit->GetXmin());
               
               fit->SetRange(-3*kNsamps, 3*kNsamps);
               
               Int_t stpBeg=-1, stpEnd=-1;
               
               // first search up for beginning
               for (Int_t j=0; j<kNsamps; j++) {
                  const Int_t xb = upBin+j; // non-wrapping
                  Int_t xw=xb; // wrapping loc
                  if (xb>kNsamps) { // xb==kNsamps is ok (last bin)
                     xw -= kNsamps; // min is 1
                  }
                  const Float_t ct = hp->GetBinContent(xw);
                  //const Float_t xs = xw-1; // bin 1 is sample 0
                  const Float_t xf = xb-1; // extend past histogram
                  const Double_t res = ct - fit->Eval(xf);
                  hResUp[ch]->Fill(i, res);
                  if (TMath::Abs(res) > stpRes) {
                     if ( TMath::Abs(ct - stpLoc) < stpWin ) {
                        // found stop edge
                        stpBeg = xw;
                        break;
                     }
                  }
               }
               
               // now search down for end
               for (Int_t j=0; j<kNsamps; j++) {
                  const Int_t xb = dnBin-j;
                  Int_t xw=xb;
                  if (xb<1) { // min is 1
                     xw += kNsamps; // max is 128
                  }
                  const Float_t ct = hp->GetBinContent(xw);
                  //const Float_t xs = xw-1; // bin 1 is sample 0
                  const Float_t xf = xb-1; // extend past histogram
                  const Double_t res = ct - fit->Eval(xf);
                  hResDn[ch]->Fill(i, res);
                  if (TMath::Abs(res) > stpRes) {
                     if ( TMath::Abs(ct - stpLoc) < stpWin ) {
                        // found stop edge
                        stpEnd = xw;
                        break;
                     }
                  }
               }

#ifdef DEBUG
               Printf("fit=%s, stpBeg=%d, stpEnd=%d",
                  fit->GetName(), stpBeg, stpEnd);
#endif
               
               Int_t swid=0;
               if ( (stpBeg!=-1) && (stpEnd!=-1) ) {
                  //swid = stpEnd - stpBeg + 1;
                  
                  // now find the flat region
                  // assume either stpBeg or stpEnd is really at the edge
                  
                  // first look above stpBeg
                  Int_t wup=0;
                  for (Int_t b=0; b<kNsamps; b++) {
                     Int_t xb = stpBeg+b;
                     if (xb>kNsamps) {
                        xb -= kNsamps;
                     }
                     const Float_t ct = hp->GetBinContent(xb);
                     if ( TMath::Abs(ct - stpLoc) < stpWin ) {
                        wup++;
                     } else {
                        break;
                     }
                  }
                  if (wup>=stpNumChn) {
                     swid   = wup;
                     stpEnd = stpBeg+swid-1;
                  } else {
                     // then try below stpEnd
                     Int_t wdn=0;
                     for (Int_t b=0; b<kNsamps; b++) {
                        Int_t xb = stpEnd-b;
                        if (xb<1) {
                           xb += kNsamps;
                        }
                        const Float_t ct = hp->GetBinContent(xb);
                        if ( TMath::Abs(ct - stpLoc) < stpWin ) {
                           wdn++;
                        } else {
                           break;
                        }
                     }
                     if (wdn>=stpNumChn) {
                        swid   = wdn;
                        stpBeg = stpEnd-swid+1;
                     }
                  }
               }
#ifdef DEBUG
               Printf("flat: stpBeg=%d, stpEnd=%d, swid=%d",
                  stpBeg, stpEnd, swid);
               Bool_t dowait = (swid>12);
#endif
               if (swid>=stpNumChn) {
                  hInARowVsEvt[ch]->SetBinContent(ebin, stpBeg, 1);
                  hStopWidth[ch]->Fill(swid);
                  hStopBegin[ch]->Fill(stpBeg-1);
                  
                  stopLocs[ch].push_back(stpBeg-1);
                  const Bool_t isGood = ( ((stpBeg> 0)&&(stpBeg<16)) ||
                                          ((stpBeg>30)&&(stpBeg<46)) ||
                                          ((stpBeg>60)&&(stpBeg<76)) ||
                                          ((stpBeg>95)&&(stpBeg<110)) );
                  if (isGood) {
                     goodLocs[ch].push_back(stpBeg-1);
                  }

                  for (Int_t bb=0; bb<swid; bb++) {
                     Int_t nb = bb+stpBeg;
                     if (nb>hp->GetNbinsX()) {
                        nb -= hp->GetNbinsX();
                     }
                     const Float_t cc = hp->GetBinContent(nb);
                     hStopAdc[ch]->Fill(cc);
                     hStopAdcVsSamp[ch]->Fill(nb-1, cc);
                     hStopAdcVsEvt[ch]->Fill(i, cc);
#ifdef DEBUG
                     if (cc<-300) {
                        Printf("bb=%d, nb=%d, cc=%g",bb,nb,cc);
                        dowait=kTRUE;
                     }
#endif
                     if ( isGood ) {
                        hStopAdcS[ch][0]->Fill(cc);
                     } else {
                        hStopAdcS[ch][1]->Fill(cc);
                     }
                  }
               }
#ifdef DEBUG
               if (dowait) {
                  cdbg->Clear();
                  cdbg->cd();
                  hp->Draw();
                  cdbg->Update();
                  cdbg->WaitPrimitive();
               }
#endif
               delete hp;
            }
         }
         for (Int_t ch=0; ch<kNchans; ch++) {
            if (isActive(ch, activeChs)) {
               const UInt_t nsch = stopLocs[ch].size();
               const UInt_t ngch = goodLocs[ch].size();
               hNumStops[ch]->Fill(nsch);
               hNumStopsS[ch][0]->Fill(ngch);
               hNumStopsS[ch][1]->Fill(nsch-ngch);
               for (Int_t nc=0; nc<ch; nc++) {
                  if (isActive(nc, activeChs)) {
                     const UInt_t nsnc = stopLocs[nc].size();
                     for (UInt_t k=0; k<nsch; k++) {
                        for (UInt_t j=0; j<nsnc; j++) {
                           Int_t sof = stopLocs[ch][k] - stopLocs[nc][j];
                           hStopDist[ch][nc]->Fill( sof );
                           if (TMath::Abs(sof)>64) {
                              sof = (stopLocs[ch][k] < stopLocs[nc][j]) ?
                                    ((stopLocs[ch][k]+128) - stopLocs[nc][j]) :
                                    (stopLocs[ch][k] - (stopLocs[nc][j]+128));
                           }
                           hStopDistM[ch][nc]->Fill( sof );
                           
                           if (TMath::Abs(sof)<2) {
                              Printf("event i=%d, sof=%d",i,sof);
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   
   if (selevts>1.0) {
      for (Int_t ch=0; ch<kNchans; ch++) {
         hNumStops[ch]->Scale(1.0 / selevts);
         for (Int_t i=0; i<2; i++) {
            hNumStopsS[ch][i]->Scale(1.0 / selevts);
         }
      }
   }
   
   Printf("\nprocessed %g events",selevts);
   
   delete[] gevts;
   
   c2 = new TCanvas("c2","c2",800,800);
   c2->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c2->cd(ch+1);
      hInARowVsEvt[ch]->Draw("colz");
   }
   c2->cd();
   c2->Update();
   
   c3 = new TCanvas("c3","c3",800,800);
   c3->Divide(2,2);
   for (Int_t ch=1; ch>=0; ch--) {
      c3->cd(1);
      hStopWidth[ch]->Draw((ch==1)?"":"same");
      c3->cd(2);
      hStopBegin[ch]->Draw((ch==1)?"":"same");
      c3->cd(3);
      hNumStops[ch]->SetMaximum(1);
      hNumStops[ch]->SetMinimum(0);
      hNumStops[ch]->Draw((ch==1)?"":"same");
   }
   c3->cd();
   c3->Update();
   
   c10 = new TCanvas("c10","c10",800,400);
   c10->Divide(2,1);
   for (Int_t i=0; i<2; i++) {
      c10->cd(i+1);
      for (Int_t ch=0; ch<kNchans; ch++) {
         hNumStopsS[ch][i]->SetMaximum(0.8);
         hNumStopsS[ch][i]->SetMinimum(0);
         hNumStopsS[ch][i]->Draw((ch==0)?"":"same");
      }
   }
   c10->cd();
   c10->Update();
   
   c4 = new TCanvas("c4","c4",750,900);
   c4->Divide(2,3);
   Int_t p=0;
   for (Int_t ch=0; ch<kNchans; ch++) {
      for (Int_t nc=0; nc<ch; nc++) {
         c4->cd(++p);
         hStopDist[ch][nc]->Draw();
         c4->GetPad(p)->SetLogy();
      }
   }
   c4->cd();
   c4->Update();
   
   c9 = new TCanvas("c9","c9",800,500);
   /*
   c9->Divide(2,3);
   Int_t q=0;
   TLatex* lat = new TLatex;
   lat->SetTextColor(kRed+2);
   lat->SetNDC();
   for (Int_t ch=1; ch<2; ch++) {
      for (Int_t nc=0; nc<ch; nc++) {
         c9->cd(++q);
   */
         hStopDistM[1][0]->Draw();
         c9->SetLogy();
         c9->SetGridx();
   /*
         const Double_t lo = hStopDistM[ch][nc]->Integral(
            hStopDistM[ch][nc]->FindBin(ofmmin+0.5),
            hStopDistM[ch][nc]->FindBin(0));
         const Double_t hi = hStopDistM[ch][nc]->Integral(
            hStopDistM[ch][nc]->FindBin(0),
            hStopDistM[ch][nc]->FindBin(ofmmax+0.5));
         const Double_t re = (hi/lo)*TMath::Sqrt(
            ( (TMath::Sqrt(lo)*TMath::Sqrt(lo)/(lo*lo)) +
              (TMath::Sqrt(hi)*TMath::Sqrt(hi)/(hi*hi)) ) );
         lat->DrawLatex(0.23,0.2,
            Form("N(>0) / N(<0) = %2.2f #pm %1.1e",hi/lo,re));
      }
   }
   */
   c9->cd();
   c9->Update();
   
   c5 = new TCanvas("c5","c5",750,900);
   c5->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c5->cd(ch+1);
      hStopAdc[ch]->Draw();
      c5->GetPad(ch+1)->SetLogy();
   }
   c5->cd();
   c5->Update();
   
   
   c6 = new TCanvas("c6","c6",750,900);
   c6->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c6->cd(ch+1);
      hStopAdcVsSamp[ch]->Draw("colz");
   }
   c6->cd();
   c6->Update();
   
   c7 = new TCanvas("c7","c7",750,900);
   c7->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c7->cd(ch+1);
      hStopAdcS[ch][0]->Draw();
      c7->GetPad(ch+1)->SetLogy();
   }
   c7->cd();
   c7->Update();
   
   c8 = new TCanvas("c8","c8",750,900);
   c8->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c8->cd(ch+1);
      hStopAdcS[ch][1]->Draw();
      c8->GetPad(ch+1)->SetLogy();
   }
   c8->cd();
   c8->Update();
   
   c11 = new TCanvas("c11","c11",800,500);
   c11->Divide(2,1);
   for (Int_t ch=0; ch<2; ch++) {
      c11->cd(ch+1);
      hResUp[ch]->Draw("colz");
   }
   c11->cd();
   c11->Update();
   
   c12 = new TCanvas("c12","c12",800,500);
   c12->Divide(2,1);
   for (Int_t ch=0; ch<2; ch++) {
      c12->cd(ch+1);
      hResDn[ch]->Draw("colz");
   }
   c12->cd();
   c12->Update();
   
   c13 = new TCanvas("c13","c13",800,500);
   c13->Divide(2,1);
   for (Int_t ch=0; ch<2; ch++) {
      c13->cd(ch+1);
      hStopAdcVsEvt[ch]->Draw("colz");
   }
   c13->cd();
   c13->Update();
   
   c14 = new TCanvas("c14","c14",800,500);
   c14->Divide(2,1);
   for (Int_t ch=0; ch<2; ch++) {
      c14->cd(ch+1);
      hAdcVsEvt[ch]->Draw("colz");
   }
   c14->cd();
   c14->Update();
   
   
}
