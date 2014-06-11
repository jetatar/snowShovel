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
#include <TProfile.h>
#include <TCanvas.h>
#endif


// 2011 station
static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;
static const Int_t   kNextra    = 57;
static const Int_t   kTrgBitIdx = 10;
static const Float_t kSmpRate   = 1.92; // GHz

const Int_t nadcbins=4096, ndevbins=801, maxtimebins=2000;
const Float_t adcmin=-0.5, adcmax=4095.5,
              devmin=-400.5, devmax=400.5,
              wvmin=1650, wvmax=1760;

const Char_t* timefmt="%m/%d %Hh %F1970-01-01 00:00:00";

const Int_t  col[4] = {kOrange+7, kAzure-2, kViolet-1, kGreen+2};

TChain* nt;
TFile* outf;

TH1* hAdcDist[kNchans][kNsamps], * hDevs[kNchans][kNsamps],
   * hEvtsVsT,
   * hWvAveVsT[kNchans], * hWvRmsVsT[kNchans], * hWvCountT[kNchans];
TGraphErrors* gAvePed[kNchans], * gAveRms[kNchans],
            * gExlPed[kNchans], * gExlRms[kNchans]/*,
            * gDevVsT[kNchans][kNsamps] */;
TH2* hDevVsSamp[kNchans],
   * hDevVsTvsSamp[kNchans],
   * hCountTvsSamp[kNchans],
   * hWvVsT[kNchans];

TProfile* pWvAveRmsVsT[kNchans];

TCanvas* c1, * c2, * c3, * c4, * c5=0, * c6, * c7, * c8, * c9;

void setStyle(TH1* h, const Int_t typ) {
   if (h!=0) {
      h->SetLineColor(col[typ]);
   }
}

void findMeanStepFromPeak(const TH1& h, const Float_t peakFrc,
                          Double_t& mean, Double_t& rms, Double_t& n) {
   const Int_t maxbin = h.GetMaximumBin();
   const Float_t cut  = h.GetBinContent(maxbin) * peakFrc;
   //Printf("maxc=%g, frc=%g, cut=%g", h.GetBinContent(maxbin), peakFrc, cut);
   Int_t hi=maxbin, lo=maxbin;
   for (Int_t i=maxbin+1; i<=h.GetNbinsX(); i++) {
      //Printf("hi: cont(%d)=%g",i,h.GetBinContent(i));
      if (h.GetBinContent(i) > cut) {
         hi=i;
      } else {
         break;
      }
   }
   for (Int_t i=maxbin-1; i>0; i--) {
      //Printf("lo: cont(%d)=%g",i,h.GetBinContent(i));
      if (h.GetBinContent(i) > cut) {
         lo=i;
      } else {
         break;
      }
   }
   h.GetXaxis()->SetRange(lo,hi);
   mean = h.GetMean();
   rms  = h.GetRMS();
   n    = h.Integral(lo,hi);
   h.GetXaxis()->SetRange();
   /*
   if (c5==0) c5 = new TCanvas;
   Printf("max=%d, lo=%d, hi=%d, mean=%g, rms=%g, n=%g",
      maxbin, lo, hi, mean, rms, n);
   c5->cd();
   h.Draw();
   c5->Update();
   c5->WaitPrimitive();
   */
}

void addPoint(const Double_t mean, const Double_t rms,
              const Double_t n,
              const Int_t sm,
              TGraphErrors& gm, TGraphErrors& gr) {
   const Double_t merr = (n>1) ? (rms/TMath::Sqrt(n))         : 1e6;
   const Double_t rerr = (n>1) ? (rms/TMath::Sqrt(2.0*(n-1))) : 1e6;
   gm.SetPoint(sm, sm, mean);
   gm.SetPointError(sm, 0, merr);
   gr.SetPoint(sm, sm, rms);
   gr.SetPointError(sm, 0, rerr);
}

Double_t getTime(const UInt_t utime, const UInt_t utimeus) {
   return (     static_cast<Double_t>(utime)
            + ( static_cast<Double_t>(utimeus)*1e-6 ) );
}

void pedTrack(const Char_t* infn,
              const Char_t* outfn,
              const Int_t   trg=1,
              const Bool_t  verifyChksum=kTRUE,
              const Float_t peakFrc=0.05) {
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   gStyle->SetOptStat(0);
   
   // open the tree
   nt = new TChain("nt");
   nt->Add(infn);
   
   const Long64_t nents = nt->GetEntries();
   Printf("Entries in tree = %lld",nents);
   
   // open the output file (to store histograms)
   outf = TFile::Open(outfn,"recreate");
   TDirectory/** dvtdir[kNchans],*/
             * adcdir[kNchans],
             * devdir[kNchans];
   
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
      
      /*dvtdir[ch] = outf->mkdir(Form("DevVsTch%d",ch));*/
      adcdir[ch] = outf->mkdir(Form("ADCch%d",ch));
      devdir[ch] = outf->mkdir(Form("Devch%d",ch));
      
      for (Int_t sm=0; sm<kNsamps; sm++) {
         hn = Form("hAdcDist_ch%d_sm%d",ch,sm);
         hAdcDist[ch][sm] = new TH1F(hn.Data(),
            Form("ADC distribution (chan %d, sample %d);ADC",ch,sm),
            nadcbins, adcmin, adcmax);
         hAdcDist[ch][sm]->GetXaxis()->SetNdivisions(505);
         hAdcDist[ch][sm]->SetDirectory(adcdir[ch]);
         
         hn = Form("hDevs_ch%d_sm%d",ch,sm);
         hDevs[ch][sm] = new TH1F(hn.Data(),
            Form("Deviation from pedestal (chan %d, sample %d)"
                 ";ADC - pedestal",ch,sm),
            ndevbins, devmin, devmax);
         hDevs[ch][sm]->GetXaxis()->SetNdivisions(505);
         hDevs[ch][sm]->SetBit(TH1::kCanRebin);
         hDevs[ch][sm]->SetDirectory(devdir[ch]);
         
         /*
         gDevVsT[ch][sm] = new TGraphErrors;
         gDevVsT[ch][sm]->SetName(Form("gDevVsT_ch%d_sm%d",ch,sm));
         gDevVsT[ch][sm]->SetTitle(Form("Deviation from pedestal "
            "(chan %d, samp %d);Date, Time (UTC);ADC - pedestal",ch,sm));
         */
      }
      
      gAvePed[ch] = new TGraphErrors;
      gAvePed[ch]->SetName(Form("gAvePed_ch%d",ch));
      gAvePed[ch]->SetTitle(Form("Overall pedestal (chan %d);"
         "sample;pedestal",ch));
      gAvePed[ch]->SetMarkerColor(kBlack);
      gAvePed[ch]->SetLineColor(kBlack);
      
      gAveRms[ch] = new TGraphErrors;
      gAveRms[ch]->SetName(Form("gAveRms_ch%d",ch));
      gAveRms[ch]->SetTitle(Form("Overall noise (chan %d);"
         "sample;noise",ch));
      gAveRms[ch]->SetMarkerColor(kBlack);
      gAveRms[ch]->SetLineColor(kBlack);
      
      gExlPed[ch] = new TGraphErrors;
      gExlPed[ch]->SetName(Form("gExlPed_ch%d",ch));
      gExlPed[ch]->SetTitle(Form("Overall pedestal excl. outliers (chan %d);"
         "sample;pedestal",ch));
      gExlPed[ch]->SetMarkerColor(kOrange+10);
      gExlPed[ch]->SetLineColor(kOrange+10);
      
      gExlRms[ch] = new TGraphErrors;
      gExlRms[ch]->SetName(Form("gExlRms_ch%d",ch));
      gExlRms[ch]->SetTitle(Form("Overall noise excl. outliers (chan %d);"
         "sample;noise",ch));
      gExlRms[ch]->SetMarkerColor(kOrange+10);
      gExlRms[ch]->SetLineColor(kOrange+10);
      
      hn = Form("hDevVsSamp_ch%d",ch);
      hDevVsSamp[ch] = new TH2F(hn.Data(),
         Form("Deviation from pedestal (chan %d) by sample"
              ";ADC - pedestal;sample;fraction of events",ch),
         ndevbins, devmin, devmax,
         kNsamps, -0.5, kNsamps-0.5);
      hDevVsSamp[ch]->SetDirectory(outf);
   }
   
   Printf("Building full ADC distributions...");
   
   nt->GetEntry(0);
   UInt_t mintime, maxtime;
   mintime = maxtime = getTime(utime, utimeus);
   
   Double_t selevts=0;
   Bool_t* gevts = new Bool_t[nents];
   
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e);
      
      if ( (e%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            e, nents,
            100.*static_cast<Float_t>(e)/static_cast<Float_t>(nents));
      }
      
      Bool_t ok = ((trg==-1) || (trg==extra[kTrgBitIdx]));
      if (ok && verifyChksum) {
         ok &= (mbchksum == (extra[21]<<16)+extra[20]);
      }
      
      if (ok) {
         
         const Double_t ctime = getTime(utime, utimeus);
         if (ctime<mintime) {
            mintime = ctime;
         }
         if (ctime>maxtime) {
            maxtime = ctime;
         }
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            for (Int_t sm=0; sm<kNsamps; sm++) {
               hAdcDist[ch][sm]->Fill(samples[ch][sm]);
            }
         }
         
         gevts[e] = kTRUE;
         selevts += 1.00;
      } else {
         gevts[e] = kFALSE;
      }
   }
   const Int_t ntimebins = TMath::Min(maxtime-mintime+1,
      static_cast<UInt_t>(maxtimebins));
   Printf("\nSelected %g events.",selevts);
   
   Printf("Removing outliers and finding pedestals...");
   
   hEvtsVsT = new TH1F("hEvtsVsT",
      "Number of events in each time bin;Date, Time (UTC);events",
      ntimebins, mintime, maxtime);
   hEvtsVsT->GetXaxis()->SetTimeDisplay(1);
   hEvtsVsT->SetDirectory(outf);
   
   TH1* h;
   Double_t mean=0, rms=0, n=0;
   for (Int_t ch=0; ch<kNchans; ch++) {
      
      hn = Form("hDevVsTvsSamp_ch%d",ch);
      hDevVsTvsSamp[ch] = new TH2F(hn.Data(),
         Form("Ave. deviation from pedestal (chan %d) vs time"
              ";Date, Time (UTC);sample;deviation",ch),
         ntimebins, mintime, maxtime,
         kNsamps, -0.5, kNsamps-0.5);
      hDevVsTvsSamp[ch]->GetXaxis()->SetTimeDisplay(1);
      hDevVsTvsSamp[ch]->SetDirectory(outf);
      
      hn = Form("hCountTvsSamp_ch%d",ch);
      hCountTvsSamp[ch] = new TH2F(hn.Data(),
         Form("Number of events in time, sample cell (chan %d)"
              ";Date, Time (UTC);sample;events",ch),
         ntimebins, mintime, maxtime,
         kNsamps, -0.5, kNsamps-0.5);
      hCountTvsSamp[ch]->SetDirectory(outf);
      
      hn = Form("hWvAveVsT_ch%d",ch);
      hWvAveVsT[ch] = new TH1F(hn.Data(),
         Form("waveform ave (chan %d) vs time;Date, Time (UTC);wv ave",ch),
         ntimebins, mintime, maxtime);
      setStyle(hWvAveVsT[ch], ch);
      
      hn = Form("hWvRmsVsT_ch%d",ch);
      hWvRmsVsT[ch] = new TH1F(hn.Data(),
         Form("waveform rms (chan %d) vs time;Date, Time (UTC);wv ave",ch),
         ntimebins, mintime, maxtime);
      setStyle(hWvRmsVsT[ch], ch);
      
      hn = Form("hWvCountT_ch%d",ch);
      hWvCountT[ch] = new TH1F(hn.Data(),
         Form("waveform count (chan %d) vs time;Date, Time (UTC);wv ave",ch),
         ntimebins, mintime, maxtime);
      setStyle(hWvCountT[ch], ch);
      
      hn = Form("pWvAveRmsVsT_ch%d",ch);
      pWvAveRmsVsT[ch] = new TProfile(hn.Data(),
         Form("waveform ave/rms (chan %d) vs time;Date, Time (UTC);ave, rms",
            ch),
         ntimebins, mintime, maxtime);
      setStyle(pWvAveRmsVsT[ch], ch);
      
      hn = Form("hWvVsT_ch%d",ch);
      hWvVsT[ch] = new TH2F(hn.Data(),
         "waveform adc values vs time (chan %d);Date, Time (UTC);ADC",
         ntimebins, mintime, maxtime,
         nadcbins, adcmin, adcmax);
      
      for (Int_t sm=0; sm<kNsamps; sm++) {
         h = hAdcDist[ch][sm];
         mean = h->GetMean();
         rms  = h->GetRMS();
         n    = h->GetEntries();
         addPoint(mean, rms, n, sm, *(gAvePed[ch]), *(gAveRms[ch]));
         
         // get mean excluding X-sigma outliers
         findMeanStepFromPeak(*h, peakFrc, mean, rms, n);
         addPoint(mean, rms, n, sm, *(gExlPed[ch]), *(gExlRms[ch]));
      }
   }
   
   TH1* htmp = new TH1F("htmp","temp",4096,-0.5,4095.5);
   
   Printf("Finding deviations from pedestal...");
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e);
      
      if ( (e%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            e, nents,
            100.*static_cast<Float_t>(e)/static_cast<Float_t>(nents));
      }
      
      if (gevts[e]) {
         const Double_t ctime = getTime(utime, utimeus);
         
         hEvtsVsT->Fill(ctime);
         
         for (Int_t ch=0; ch<kNchans; ch++) {
            const Double_t* const peds = gExlPed[ch]->GetY();
            const Double_t*       ped  = peds;
            const Double_t* const rmss = gExlRms[ch]->GetY();
            const Double_t*       rms  = rmss;
            
            htmp->Reset();
            
            for (Int_t sm=0; sm<kNsamps; sm++, ped++, rms++) {
               const UShort_t sam = samples[ch][sm];
               const Double_t dev = sam - (*ped);
               
               /*gDevVsT[ch][sm]->SetPoint( e, ctime, dev );*/
               hDevs[ch][sm]->Fill( dev );
               hDevVsSamp[ch]->Fill(dev, sm);
               hDevVsTvsSamp[ch]->Fill(ctime, sm, dev);
               hCountTvsSamp[ch]->Fill(ctime, sm, 1.0);
               
               htmp->Fill(samples[ch][sm]);
               
               pWvAveRmsVsT[ch]->Fill(ctime, sam);
               hWvVsT[ch]->Fill(ctime, sam);
            }
            
            hWvAveVsT[ch]->Fill(ctime, htmp->GetMean());
            hWvRmsVsT[ch]->Fill(ctime, htmp->GetRMS());
            hWvCountT[ch]->Fill(ctime, 1.0);
            
         }
      }
   }
   for (Int_t ch=0; ch<kNchans; ch++) {
      hDevVsSamp[ch]->Scale(1.0 / selevts);
      hDevVsTvsSamp[ch]->Divide(hCountTvsSamp[ch]); // average
      hWvAveVsT[ch]->Divide(hWvCountT[ch]);
      hWvRmsVsT[ch]->Divide(hWvCountT[ch]);
   }
   Printf(" ");
   
   Printf("done!");
   
   delete[] gevts;
   
   // save
   for (Int_t ch=0; ch<kNchans; ch++) {
      outf->cd();
      gAvePed[ch]->Write(gAvePed[ch]->GetName());
      gAveRms[ch]->Write(gAveRms[ch]->GetName());
      gExlPed[ch]->Write(gExlPed[ch]->GetName());
      gExlRms[ch]->Write(gExlRms[ch]->GetName());
      /*
      dvtdir[ch]->cd();
      for (Int_t sm=0; sm<kNsamps; sm++) {
         gDevVsT[ch][sm]->Write(gDevVsT[ch][sm]->GetName());
      }
      */
      
      for (Int_t i=hEvtsVsT->GetNbinsX(); i>0; i--) {
         const Float_t hev = hEvtsVsT->GetBinContent(i);
         if (hev>0) {
            for (Int_t y=hWvVsT[ch]->GetNbinsY(); y>0; y--) {
               hWvVsT[ch]->SetBinContent(i, y,
                  hWvVsT[ch]->GetBinContent(i,y) / hev);
            }
         }
      }
      
   }
   outf->Write();
   
   
   // draw
   c1 = new TCanvas("c1","c1 - overall ped",800,800);
   c1->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c1->cd(ch+1);
      gAvePed[ch]->Draw("ap");
      gExlPed[ch]->Draw("p");
   }
   c1->cd();
   c1->Update();
   
   c4 = new TCanvas("c4","c4 - overall rms",800,800);
   c4->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c4->cd(ch+1);
      gAveRms[ch]->Draw("ap");
      gExlRms[ch]->Draw("p");
   }
   c4->cd();
   c4->Update();
   
   c2 = new TCanvas("c2","c2 - dev vs t",800,800);
   c2->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c2->cd(ch+1);
      hDevVsTvsSamp[ch]->Draw("colz");
   }
   c2->cd();
   c2->Update();
   
   c3 = new TCanvas("c3","c3 - deviations",800,800);
   c3->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c3->cd(ch+1);
      hDevVsSamp[ch]->Draw("colz");
   }
   c3->cd();
   c3->Update();
   
   c5 =  new TCanvas("c5","c5 - evts vs t",500,500);
   c5->cd();
   hEvtsVsT->Draw();
   c5->Update();
   
   c6 = new TCanvas("c6","c6 - wv ave vs t",800,800);
   c6->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c6->cd(ch+1);
      hWvAveVsT[ch]->SetMinimum(wvmin);
      hWvAveVsT[ch]->SetMaximum(wvmax);
      hWvAveVsT[ch]->GetXaxis()->SetTimeFormat(timefmt);
      hWvAveVsT[ch]->GetXaxis()->SetTimeDisplay(1);
      hWvAveVsT[ch]->GetXaxis()->SetNdivisions(505);
      hWvAveVsT[ch]->Draw();
      c6->GetPad(ch+1)->SetGridx();
      c6->GetPad(ch+1)->SetGridy();
   }
   c6->cd();
   c6->Update();
   
   c7 = new TCanvas("c7","c7 - wv rms vs t",800,800);
   c7->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c7->cd(ch+1);
      hWvRmsVsT[ch]->GetXaxis()->SetTimeFormat(timefmt);
      hWvRmsVsT[ch]->GetXaxis()->SetTimeDisplay(1);
      hWvRmsVsT[ch]->GetXaxis()->SetNdivisions(505);
      hWvRmsVsT[ch]->Draw();
      c7->GetPad(ch+1)->SetGridx();
      c7->GetPad(ch+1)->SetGridy();
   }
   c7->cd();
   c7->Update();
   
   c8 = new TCanvas("c8","c8 - wv ave/rms prof vs t",800,800);
   c8->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c8->cd(ch+1);
      pWvAveRmsVsT[ch]->SetMinimum(wvmin);
      pWvAveRmsVsT[ch]->SetMaximum(wvmax);
      pWvAveRmsVsT[ch]->GetXaxis()->SetTimeFormat(timefmt);
      pWvAveRmsVsT[ch]->GetXaxis()->SetTimeDisplay(1);
      pWvAveRmsVsT[ch]->GetXaxis()->SetNdivisions(505);
      pWvAveRmsVsT[ch]->Draw();
      c8->GetPad(ch+1)->SetGridx();
      c8->GetPad(ch+1)->SetGridy();
   }
   c8->cd();
   c8->Update();
   
   c9 = new TCanvas("c9","c9 - wv adc vs t",800,800);
   c9->Divide(2,2);
   for (Int_t ch=0; ch<kNchans; ch++) {
      c9->cd(ch+1);
      hWvVsT[ch]->GetXaxis()->SetTimeFormat(timefmt);
      hWvVsT[ch]->GetXaxis()->SetTimeDisplay(1);
      hWvVsT[ch]->GetXaxis()->SetNdivisions(505);
      hWvVsT[ch]->Draw("colz");
   }
   c9->cd();
   c9->Update();
   
   
}
