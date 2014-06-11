#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TDatime.h>
#include <TString.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TH1.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#include <TH2.h>
#include <TLegend.h>
#include <TLine.h>
#include <TROOT.h>

#include "TSnMath.h"
#endif

static const Float_t kSecPerDay = 3600. * 24.;

TFile* ratef=0, * voltf=0, * statf=0, * sbgf=0;
TCanvas* c1=0, * c2=0;
TGraph* gERvsT=0, * gVolt=0, * gCurr=0;
TGraph* gStERvsT=0, * gStVvsT=0, * gStCvsT=0;

// ScottBase Weather graphs
TGraph* gMaxTemp=0, * gMinTemp=0, * gMaxGust=0, * gCTemp=0, * gAveWind=0;

const Int_t rcol = kRed;
const Int_t rmrk = 6;

enum { kRate=0, kVolt, kCurr, kStRate, kStVolt, kStCurr,
       kCTemp, kMaxTemp, kMinTemp, kAveWind, kMaxGust,
       kN };
const Int_t cols[kN] = { kRed, kMagenta+2, kOrange-3,
                         kOrange+7, kViolet+7, kOrange+2,
                         kOrange+10, kViolet-6, kAzure-6,
                         kGreen+2, kTeal-6 };
const Int_t mrks[kN] = { 6, 6, 6,
                         6, 6, 6,
                         6, 6, 6, 6, 6 };

const Char_t* grphDrawOpt = "px";

const Int_t cwid=800, chit=1100;

const Int_t rbins=100,
   vbins=100,
   zbins=100,
   cbins=100,
   mbins=90,
   wbins=120;
const Float_t rmin=5e-4, rmax=25,
   vmin=10, vmax=25,
   zmin=11, zmax=14,
   cmin=0.5, cmax=1.0,
   mmin=-60, mmax=30,
   wmin=0, wmax=60;

void setStyle(TGraph* g, const Int_t type) {
   if (g!=0) {
      g->SetMarkerStyle(mrks[type]);
      g->SetMarkerColor(cols[type]);
      g->SetLineColor(cols[type]);
   }
}

void setTimeDisp(TH1& h, const Int_t ndiv=510) {
   h.GetXaxis()->SetTimeFormat("%m/%d");
   h.GetXaxis()->SetTimeDisplay(1);
   h.GetXaxis()->SetNdivisions(ndiv);
}

void setTimeDisp(TGraph& g, const Int_t ndiv=510) {
   if ( (g.GetHistogram()!=0) &&
        (g.GetHistogram()->GetXaxis()!=0) ) {
      setTimeDisp(*(g.GetHistogram()),ndiv);
   }
}

template<class Plot_t>
TH1F* newDumHistForLegend(const Plot_t* hist) {
   // caller is responsible for deleting the hist
   TH1F* h = 0;
   if (hist!=0) {
      TString hn(Form("%s_leg%s",hist->GetName(),
                      (gPad!=0) ? gPad->GetName() : ""));
      h = new TH1F(hn.Data(), "", 1, 0, 1);
      h->SetFillColor(hist->GetLineColor());
      h->SetLineColor(kWhite);
      h->SetMarkerColor(kWhite);
      h->SetMarkerStyle(hist->GetMarkerStyle());
      h->SetMarkerSize(1);
      h->SetBit(TObject::kCanDelete);
   }
   return h;
}

void getStartFinFrom(const TGraph& gr,
                     Double_t& start, Double_t& fin,
                     const Bool_t filterZero=kTRUE) {
   // get a nice time axis for the domain of the graph
   start = fin = 0;
   const Int_t n = gr.GetN();
   Bool_t first = kTRUE;
   const Double_t* y = gr.GetX();
   for (Int_t i=0; i<n; ++i, ++y) {
      if (filterZero && ((*y)==0)) {
         continue;
      } else {
         if (first) {
            start = fin = *y;
            first = kFALSE;
         } else {
            if ( (*y) < start ) {
               start = *y;
            }
            if ( (*y) > fin ) {
               fin = *y;
            }
         }
      }
   }
}

void makeSBWcharts(const Char_t* sbgfn,
                   const Char_t* outtag,
                   const Float_t zoomDays=7.0,
                   const Bool_t  drawMinMax=kFALSE,
                   const Bool_t  finFromData=kFALSE) {
   // finFromData = true => max of time axis determined by last data point
   //             = false => determined by time at which the script is run
   
   gROOT->Reset();
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   gStyle->SetOptStat(0);

   TH1::AddDirectory(kTRUE);

   sbgf = TFile::Open(sbgfn);
   if ( (sbgf==0) || (sbgf->IsZombie()) ) {
      Error("makeSBWcharts","Could not open [%s]",sbgfn);
   }
   
   gMaxTemp = dynamic_cast<TGraph*>(sbgf->Get("gMaxTemp"));
   gMinTemp = dynamic_cast<TGraph*>(sbgf->Get("gMinTemp"));
   gMaxGust = dynamic_cast<TGraph*>(sbgf->Get("gMaxGust"));
   gCTemp   = dynamic_cast<TGraph*>(sbgf->Get("gCTemp"));
   gAveWind = dynamic_cast<TGraph*>(sbgf->Get("gAveWind"));
   
   const TDatime dnow;
   const UInt_t  now = dnow.Convert();
   Double_t start(0), fin(0);
   Int_t tbins(100);
   Double_t tmin(0), tmax(1);
   
   c1 = new TCanvas("c1","c1",cwid,static_cast<Int_t>(chit/2.0));
   c1->Divide(1,2);

   TLegend* mleg=0, * wleg=0;
   TLine* tzline=0;
   
   if (drawMinMax) {
      mleg = new TLegend(.258,.742,.410,.993);
      mleg->SetFillColor(0);
      mleg->SetBorderSize(1);
      
      wleg = new TLegend(.244,.794,.444,.993);
      wleg->SetFillColor(0);
      wleg->SetBorderSize(1);
   }
   
   if (gCTemp!=0) {
      setStyle(gCTemp, kCTemp);
      
      // get time scale from CTemp (only)
      getStartFinFrom(*gCTemp, start, fin);
      if (finFromData==kFALSE) {
         fin = now;
      }
      TSnMath::GetNiceDateAxisFor(start, fin, tbins, tmin, tmax);
      TH2F* htd = new TH2F("htd",
                           gCTemp->GetTitle(),
                           tbins, tmin, tmax,
                           mbins, mmin, mmax);
      setTimeDisp(*htd);

      tzline = new TLine(tmin, 0, tmax, 0);
      tzline->SetLineColor(kBlack);
      tzline->SetLineWidth(1);
      tzline->SetLineStyle(2);
      
      c1->cd(1);
      htd->Draw("axis");
      tzline->Draw();
      gCTemp->Draw(grphDrawOpt);
      c1->GetPad(1)->SetGridx();
      c1->GetPad(1)->SetGridy();
      if (drawMinMax) {
         mleg->AddEntry(newDumHistForLegend(gCTemp), "Temp", "fp");
         if (gMaxTemp!=0) {
            setStyle(gMaxTemp, kMaxTemp);
            gMaxTemp->Draw(grphDrawOpt);
            mleg->AddEntry(newDumHistForLegend(gMaxTemp), "Max Temp", "fp");
         }
         if (gMinTemp!=0) {
            setStyle(gMinTemp, kMinTemp);
            gMinTemp->Draw(grphDrawOpt);
            mleg->AddEntry(newDumHistForLegend(gMinTemp), "Min Temp", "fp");
         }
         mleg->Draw();
      }
      
      // use time scale from temp
      if (gAveWind!=0) {
         setStyle(gAveWind, kAveWind);
         TH2F* hwd = new TH2F("hwd",
                              gAveWind->GetTitle(),
                              tbins, tmin, tmax,
                              wbins, wmin, wmax);
         setTimeDisp(*hwd);
         
         c1->cd(2);
         hwd->Draw("axis");
         gAveWind->Draw(grphDrawOpt);
         c1->GetPad(2)->SetGridx();
         c1->GetPad(2)->SetGridy();
         if (drawMinMax) {
            wleg->AddEntry(newDumHistForLegend(gAveWind), "Wind Speed", "fp");
            if (gMaxGust!=0) {
               setStyle(gMaxGust, kMaxGust);
               gMaxGust->Draw(grphDrawOpt);
               wleg->AddEntry(newDumHistForLegend(gMaxGust), "Max Gust", "fp");
            }
            wleg->Draw();
         }
      }
   }
   
   c1->cd();
   c1->Update();
   
   c1->Print(Form("%s.root",outtag));
   c1->Print(Form("%s.gif",outtag));
   
   
   c2 = new TCanvas("c2","c2",cwid,static_cast<Int_t>(chit/2.0));
   c2->Divide(1,2);
   
   TSnMath::GetNiceDateAxisFor( fin - (zoomDays*kSecPerDay),
                                fin,
                                tbins, tmin, tmax);
   if (gCTemp!=0) {
      c2->cd(1);
      TH2F* htz = new TH2F("htz", gCTemp->GetTitle(),
                           tbins, tmin, tmax,
                           mbins, mmin, mmax);
      setTimeDisp(*htz, 515);
      htz->Draw("axis");
      gCTemp->Draw(grphDrawOpt);
      tzline->DrawLine(tmin, 0, tmax, 0);
      c2->GetPad(1)->SetGridx();
      c2->GetPad(1)->SetGridy();
      if (drawMinMax) {
         if (gMaxTemp!=0) {
            gMaxTemp->Draw(grphDrawOpt);
         }
         if (gMinTemp!=0) {
            gMinTemp->Draw(grphDrawOpt);
         }
         mleg->Draw();
      }
      if (gAveWind!=0) {
         TH2F* hwz = new TH2F("hwz",
                              gAveWind->GetTitle(),
                              tbins, tmin, tmax,
                              wbins, wmin, wmax);
         setTimeDisp(*hwz, 515);
         
         c2->cd(2);
         hwz->Draw("axis");
         gAveWind->Draw(grphDrawOpt);
         c2->GetPad(2)->SetGridx();
         c2->GetPad(2)->SetGridy();
         if (drawMinMax) {
            if (gMaxGust!=0) {
               gMaxGust->Draw(grphDrawOpt);
            }
            wleg->Draw();
         }
      }
   }
   
   c2->cd();
   c2->Update();
   
   c2->Print(Form("%s.now.root",outtag));
   c2->Print(Form("%s.now.gif",outtag));
   
   // cleanup
   delete sbgf;
   delete gMaxTemp; delete gMinTemp; delete gCTemp;
   delete gMaxGust; delete gAveWind;
   delete c1; delete c2;
   delete tzline;
   if (drawMinMax) {
      delete mleg; delete wleg;
   }
   
}

void makeStripCharts(const Char_t* ratefn,
                     const Char_t* voltfn,
                     const Char_t* outtag,
                     const Char_t* statfn=0, // def 0 for backwards compatibility
                     const Float_t zoomDays=7.0,
                     const Bool_t  finFromData=kFALSE) {
   // finFromData = true => max of time axis determined by last data point
   //             = false => determined by time at which the script is run
   
   gSystem->Setenv("TZ","UTC");
   gStyle->SetTimeOffset(0);
   gStyle->SetOptStat(0);

   ratef = TFile::Open(ratefn);
   if ( (ratef==0) || (ratef->IsZombie()) ) {
      Error("makeStripCharts","Could not open [%s]",ratefn);
   } else {
      gERvsT = dynamic_cast<TGraph*>(ratef->Get("gERvsT"));
   }
   voltf = TFile::Open(voltfn);
   if ( (voltf==0) || (voltf->IsZombie()) ) {
      Error("makeStripCharts","Could not open [%s]",voltfn);
   } else {
      gVolt  = dynamic_cast<TGraph*>(voltf->Get("gVolt"));
      gCurr  = dynamic_cast<TGraph*>(voltf->Get("gCurr"));
   }
   
   if (statfn!=0) {
      statf = TFile::Open(statfn);
      if ( (statf!=0) && (statf->IsZombie()==kFALSE) ) {
         gStERvsT = dynamic_cast<TGraph*>(statf->Get("gStERvsT"));
         gStVvsT  = dynamic_cast<TGraph*>(statf->Get("gStVvsT"));
         gStCvsT  = dynamic_cast<TGraph*>(statf->Get("gStCvsT"));
      }
      if (gERvsT==0) {
         gERvsT = gStERvsT;
      }
      if (gVolt==0) {
         gVolt = gStVvsT;
      }
      if (gCurr==0) {
         gCurr = gStCvsT;
      }
   }
   
   if ( (gERvsT!=0) && (gVolt!=0) && (gCurr!=0) ) {
   
      setStyle(gERvsT, kRate);
      setStyle(gVolt, kVolt);
      setStyle(gCurr, kCurr);

      if (gStVvsT!=0) {
         setStyle(gStVvsT, kStVolt);
      }
      if (gStCvsT!=0) {
         setStyle(gStCvsT, kStCurr);
      }
      
      // assume gERvsT is ordered
      Double_t start = (gERvsT->GetX())[0];
      Double_t fin   = (gERvsT->GetX())[(gERvsT->GetN())-1];
      if (gStERvsT!=0) {
         setStyle(gStERvsT, kStRate);
         // gStERvsT not ordered
         Double_t sst, sfn;
         getStartFinFrom(*gStERvsT, sst, sfn);
         if (sst<start) {
            start = sst;
         }
         if (sfn>fin) {
            fin = sfn;
         }
      }
      if (finFromData==kFALSE) {
         const TDatime now;
         fin = now.Convert();
      }
      Int_t tbins(100);
      Double_t tmin(0), tmax(1);
      TSnMath::GetNiceDateAxisFor(start, fin, tbins, tmin, tmax);
      TH2F* hrd = new TH2F("hrd",
                           gERvsT->GetTitle(),
                           tbins, tmin, tmax,
                           rbins, rmin, rmax);
      TH2F* hvd = new TH2F("hvd",
                           gVolt->GetTitle(),
                           tbins, tmin, tmax,
                           vbins, vmin, vmax);
      TH2F* hzd = new TH2F("hzd",
                           gVolt->GetTitle(),
                           tbins, tmin, tmax,
                           zbins, zmin, zmax);
      TH2F* hcd = new TH2F("hcd",
                           gCurr->GetTitle(),
                           tbins, tmin, tmax,
                           cbins, cmin, cmax);
      setTimeDisp(*hrd);
      setTimeDisp(*hvd);
      setTimeDisp(*hzd);
      setTimeDisp(*hcd);

      c1 = new TCanvas("c1","c1",cwid,chit);
      c1->Divide(1,4);
      c1->cd(1);
      hrd->Draw("axis");
      if (gStERvsT!=0) {
         gStERvsT->Draw(grphDrawOpt);
      }
      gERvsT->Draw(grphDrawOpt);
      c1->GetPad(1)->SetLogy();
      c1->GetPad(1)->SetGridx();
      c1->cd(2);
      hzd->Draw("axis");
      if (gStVvsT!=0) {
         gStVvsT->Draw(grphDrawOpt);
      }
      gVolt->Draw(grphDrawOpt);
      c1->GetPad(2)->SetGridx();
      c1->cd(3);
      hvd->Draw("axis");
      if (gStVvsT!=0) {
         gStVvsT->Draw(grphDrawOpt);
      }
      gVolt->Draw(grphDrawOpt);
      c1->GetPad(3)->SetGridx();
      c1->cd(4);
      hcd->Draw("axis");
      if (gStCvsT!=0) {
         gStCvsT->Draw(grphDrawOpt);
      }
      gCurr->Draw(grphDrawOpt);
      c1->GetPad(4)->SetGridx();
      c1->cd();
      c1->Update();

      c1->Print(Form("%s.root",outtag));
      c1->Print(Form("%s.gif",outtag));
      

      TSnMath::GetNiceDateAxisFor( fin - (zoomDays*kSecPerDay),
                                   fin,
                                   tbins, tmin, tmax);
      TH2F* hrz = new TH2F("hrz",
                           gERvsT->GetTitle(),
                           tbins, tmin, tmax,
                           rbins, rmin, rmax);
      TH2F* hvz = new TH2F("hvz",
                           gVolt->GetTitle(),
                           tbins, tmin, tmax,
                           zbins, zmin, zmax);
      TH2F* hzz = new TH2F("hzz",
                           gVolt->GetTitle(),
                           tbins, tmin, tmax,
                           vbins, vmin, vmax);
      TH2F* hcz = new TH2F("hcz",
                           gCurr->GetTitle(),
                           tbins, tmin, tmax,
                           cbins, cmin, cmax);
      setTimeDisp(*hrz, 515);
      setTimeDisp(*hvz, 515);
      setTimeDisp(*hzz, 515);
      setTimeDisp(*hcz, 515);
      
      c2 = new TCanvas("c2","c2",cwid,chit);
      c2->Divide(1,4);
      c2->cd(1);
      hrz->Draw("axis");
      if (gStERvsT!=0) {
         gStERvsT->Draw(grphDrawOpt);
      }
      gERvsT->Draw(grphDrawOpt);
      c2->GetPad(1)->SetLogy();
      c2->GetPad(1)->SetGridx();
      c2->cd(2);
      hvz->Draw("axis");
      if (gStVvsT!=0) {
         gStVvsT->Draw(grphDrawOpt);
      }
      gVolt->Draw(grphDrawOpt);
      c2->GetPad(2)->SetGridx();
      c2->cd(3);
      hzz->Draw("axis");
      if (gStVvsT!=0) {
         gStVvsT->Draw(grphDrawOpt);
      }
      gVolt->Draw(grphDrawOpt);
      c2->GetPad(3)->SetGridx();
      c2->cd(4);
      hcz->Draw("axis");
      if (gStCvsT!=0) {
         gStCvsT->Draw(grphDrawOpt);
      }
      gCurr->Draw(grphDrawOpt);
      c2->GetPad(4)->SetGridx();
      c2->cd();
      c2->Update();

      c2->Print(Form("%s.now.root",outtag));
      c2->Print(Form("%s.now.gif",outtag));

      
   } else {
      Error("makeStripCharts","Could not get graphs");
   }
   
}
