#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TSystem.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include <TPython.h>
#include <TVector3.h>
#include <TSpline.h>
#include <TRandom3.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <NSnConstants.h>
#include <TSnSpectral.h>

#include <cmath>
#endif

#include "noAveBounceStdy.C"

#define DEBUG_CORRELS
#define DEBUG_DTS

TChain* rbt, * bbt;
TNtuple* tChanDelays;


const Float_t chDelays[NSnConstants::kNchans] = {
   0,0,0,0
};

TGraph* gdbgfit[NSnConstants::kNchans]={0,0,0,0};
TGraph* gdbgll[NSnConstants::kNchans]={0,0,0,0};

Double_t getOneChanDelayLL(const Double_t* dels) {
   Double_t ll(1);
   const Int_t ch = TMath::Nint(dels[1]);
   Double_t xpos = dels[0];
   Bool_t oob=kFALSE;
   if (xpos<-maxdt) {
      xpos = -maxdt;
      oob  = kTRUE;
   } else if (xpos>maxdt) {
      xpos = maxdt;
      oob  = kTRUE;
   }
   const Double_t dtsmp = xpos * kSmpRate;
   const Double_t  corco = gspl[ch][0]->Eval(dtsmp);
   ll = -TMath::Log(getProbFromCorrCoef(corco));
   if (oob) {
      // really don't like being out of bounds
      Double_t wa = TMath::Abs(dels[0])-maxdt;
      ll += wa*wa;
   } // ch
   if (gdbgfit[ch]!=0) {
      gdbgfit[ch]->SetPoint(gdbgfit[ch]->GetN(),dels[0],ll);
   }
   return ll;
}

Double_t getMaxFromCor(TGraph* gc) {
   const Int_t n = gc->GetN();
   const Double_t* y = gc->GetY();
   const Double_t* x = gc->GetX();
   Double_t maxy(*y);
   Double_t maxx(*x);
   for (Int_t i=0; i<n; ++i, ++y, ++x) {
      if (*y > maxy) {
         maxy = *y;
         maxx = *x;
      }
   }
   return maxx;
}


void twoRunBounce(const Char_t* refBtfn,
                  const Char_t* bncBtfn,
                  const Char_t* outfn,
                  const UInt_t  rseed,
                  const Int_t   fitOpt=0,
                  const Char_t* minner="Minuit2",
                  const Char_t* algo="Migrad",
                  const Int_t   shiftminb=-1,
                  const Int_t   shiftmaxb=-1,
                  const Bool_t  applyFilter=kTRUE) {
   // compare an event from the bounce file with a random event
   // in the reference file
   //
   //
   // fitOpt:
   //   0 = fit (filtered) waveforms
   //  10 = fit envelope of (filtered) waveforms
   //
   //     minner              algo
   //  Minuit /Minuit2        Migrad,Simplex,Combined,Scan  (default is Migrad)
   //  Minuit2                Fumili2
   //  Fumili
   //  GSLMultiMin            ConjugateFR, ConjugatePR, BFGS, 
   //                         BFGS2, SteepestDescent
   //  GSLMultiFit
   //  GSLSimAn
   //  Genetic

   if (mini==0) {
      mini = ROOT::Math::Factory::CreateMinimizer(minner, algo);
      mini->SetMaxFunctionCalls(1000000);
      mini->SetMaxIterations(10000);
      mini->SetTolerance(0.001);
      mini->SetPrintLevel(0);
   }

   Float_t  rpsshift[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  rfiltered[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  renvelope[NSnConstants::kNchans][NSnConstants::kNsamps];
   UInt_t   revnum, rutime;
   rbt = new TChain("bt");
   rbt->Add(refBtfn);
   const Long64_t rents = rbt->GetEntries();
   if (rents==0) {
      Error("twoRunBounce","No events in reference tree from [%s].",refBtfn);
      return;
   }
   rbt->SetBranchAddress("EvtNum",&revnum);
   rbt->SetBranchAddress("unixTime",&rutime);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      rbt->SetBranchAddress(Form("psd%02d",ch), &(rpsshift[ch][0]));
   }
   rbt->BuildIndex("EvtNum");
   
   Float_t  bpsshift[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  bfiltered[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  benvelope[NSnConstants::kNchans][NSnConstants::kNsamps];
   UInt_t   bevnum, butime;
   bbt = new TChain("bt");
   bbt->Add(bncBtfn);
   const Long64_t bents = bbt->GetEntries();
   if (bents==0) {
      Error("twoRunBounce","No events in bounce tree from [%s].",bncBtfn);
      return;
   }
   bbt->SetBranchAddress("EvtNum",&bevnum);
   bbt->SetBranchAddress("unixTime",&butime);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      bbt->SetBranchAddress(Form("psd%02d",ch), &(bpsshift[ch][0]));
   }
   
   // output
   TString hn;
   outf = TFile::Open(outfn,"recreate");
   hn = "RefEvtNum:BncEvtNum";
   for (Int_t i=0; i<NSnConstants::kNchans; ++i) {
      hn += Form(":dt%d:ll%d",i,i);
   }
   tChanDelays = new TNtuple("tChanDelays",
                             "fit of (two run) channel delays",
                             hn.Data());
   
   // process
   TRandom3 rnd(rseed);
   UInt_t ren(0);
   for (Long64_t ev=0, re=0; ev<bents; ++ev) {
      if ( (ev%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
                 ev, bents,
                 100.*static_cast<Float_t>(ev)/static_cast<Float_t>(bents));
      }
      bbt->GetEntry(ev);
      ren = rnd.Integer(rents); // NOTE: same event can be pulled multiple times
      //re = rbt->GetEntryNumberWithIndex(bevnum);
      re = rbt->GetEntryNumberWithIndex(ren);
      if (re>-1) {
         rbt->GetEntry(re);
         
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {

            // apply filter
            memcpy(&(rfiltered[ch][0]), &(rpsshift[ch][0]),
                   NSnConstants::kNsamps*sizeof(Float_t));
            memcpy(&(bfiltered[ch][0]), &(bpsshift[ch][0]),
                   NSnConstants::kNsamps*sizeof(Float_t));
            if (applyFilter) {
               filterWaveform(&(rfiltered[ch][0]), NSnConstants::kNsamps);
               filterWaveform(&(bfiltered[ch][0]), NSnConstants::kNsamps);
            }
            // find envelope
            memcpy(&(renvelope[ch][0]), &(rfiltered[ch][0]),
                   NSnConstants::kNsamps*sizeof(Float_t));
            memcpy(&(benvelope[ch][0]), &(bfiltered[ch][0]),
                   NSnConstants::kNsamps*sizeof(Float_t));
            TSnSpectral::EnvelopeReal(&(renvelope[ch][0]), NSnConstants::kNsamps);
            TSnSpectral::EnvelopeReal(&(benvelope[ch][0]), NSnConstants::kNsamps);
            
         } // chan
         
         if (shiftmaxb!=shiftminb) {
            maxdt = (shiftmaxb - shiftminb) / (2.0*kSmpRate);
         } else {
            maxdt =  NSnConstants::kNsamps  / (2.0*kSmpRate);
         }
         
         const Float_t* rdat(0), * bdat(0);
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            if (fitOpt==0) {
               rdat = &(rfiltered[ch][0]);
               bdat = &(bfiltered[ch][0]);
            } else if (fitOpt==10) {
               rdat = &(renvelope[ch][0]);
               bdat = &(benvelope[ch][0]);
            } else {
               Fatal("noAveBounceStdy","Unknown fitOpt [%d]",fitOpt);
            }
            gcor[ch][0] = newCorrCoefGraph(rdat,
                                           bdat,
                                           NSnConstants::kNsamps,
                                           shiftminb,
                                           shiftmaxb);
            gspl[ch][0] = new TSpline3(Form("spl_%d",ch),
                                        gcor[ch][0]);
            gchi[ch][0] = newShiftChi2Graph(rdat,
                                            NSnConstants::kNsamps,
                                            bdat,
                                            NSnConstants::kNsamps,
                                            shiftminb,
                                            shiftmaxb);
            gspc[ch][0] = new TSpline3(Form("spc_%d",ch),
                                       gchi[ch][0]);

#ifdef DEBUG_CORRELS
            TCanvas* cdbgcl = new TCanvas("cdbgcl","cdbgcl",800,1000);
            cdbgcl->Divide(1,3);
            cdbgcl->cd(1);
            Float_t xxsm[NSnConstants::kNsamps];
            for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
               xxsm[i] = i;
            }
            TGraph* gcfl  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(rfiltered[ch][0]));
            TGraph* gxfl  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(bfiltered[ch][0]));
            TGraph* gcen  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(renvelope[ch][0]));
            TGraph* gxen  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(benvelope[ch][0]));
            gcfl->SetMarkerStyle(7);
            gcfl->SetMarkerColor(kBlack);
            gcfl->SetLineColor(kBlack);
            gxfl->SetMarkerStyle(7);
            gxfl->SetMarkerColor(kRed);
            gxfl->SetLineColor(kRed);
            gcen->SetMarkerStyle(7);
            gcen->SetMarkerColor(kGray);
            gcen->SetLineColor(kGray);
            gxen->SetMarkerStyle(7);
            gxen->SetMarkerColor(kViolet-1);
            gxen->SetLineColor(kViolet-1);
            gcfl->Draw("apc");
            gxfl->Draw("pc");
            gcen->Draw("pc");
            gxen->Draw("pc");
            cdbgcl->cd(2);
            gspl[ch][0]->SetMarkerStyle(7);
            gspl[ch][0]->SetMarkerColor(kAzure-7);
            gspl[ch][0]->SetLineColor(kAzure-7);
            gspl[ch][0]->Draw("pc");
            cdbgcl->cd(3);
            gspc[ch][0]->SetMarkerStyle(7);
            gspc[ch][0]->SetMarkerColor(kViolet-6);
            gspc[ch][0]->SetLineColor(kViolet-6);
            gspc[ch][0]->Draw("pc");
            cdbgcl->cd();
            cdbgcl->Update();
            cdbgcl->WaitPrimitive();
            delete gcfl; delete gxfl; delete cdbgcl;
            delete gcen; delete gxen;
#endif
         } // chan

         Float_t* tofil = new Float_t[(2*NSnConstants::kNchans)+2];
         Float_t* tf = tofil;
         *tf++ = revnum;
         *tf++ = bevnum;
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
#ifdef DEBUG_DTS
            gdbgll[ch] = new TGraph;
            gdbgll[ch]->SetTitle(";sample shift;ll");
            Int_t jll=0;
            const Float_t mmx = static_cast<Float_t>(NSnConstants::kNsamps) /
               (2.0 * kSmpRate);
            for (Float_t lx=-mmx; lx<mmx; lx+=0.025) {
               Double_t lxx[] = { lx/kSmpRate, ch };
               gdbgll[ch]->SetPoint(jll++, lx, 
                                    getOneChanDelayLL(lxx));
            }
            Printf("start ch%d at %g (%g)",
                   ch,getMaxFromCor(gcor[ch][0])/kSmpRate,
                   getMaxFromCor(gcor[ch][0]));
            gdbgfit[ch] = new TGraph;
            gdbgfit[ch]->SetTitle(";dt shift (ns);ll");
#endif
            // fit for the best, consistent time offsets
            mini->Clear();
            ROOT::Math::Functor f(&getOneChanDelayLL, 2);
            mini->SetFunction(f);
            if (strcmp(minner,"Genetic")==0) {
               mini->SetLimitedVariable(0, Form("dt%d",ch), 
                                        getMaxFromCor(gcor[ch][0])/kSmpRate, 0.01,
                                        -maxdt, maxdt);
            } else {
               mini->SetVariable(0, Form("dt%d",ch), 
                                 getMaxFromCor(gcor[ch][0])/kSmpRate, 0.01);
            }
            mini->SetFixedVariable(1, "ch", ch);
            mini->Minimize();
            const Double_t* result = mini->X();
            *tf++ = *result;
            *tf++ = mini->MinValue();
         }
         tChanDelays->Fill(tofil);
#ifdef DEBUG_DTS
         TCanvas* cdbgco = new TCanvas("cdbgco","cdbgco",800,800);
         cdbgco->Divide(2,2);
         TCanvas* cdbgfit = new TCanvas("cdbgfit","cdbgfit",800,800);
         cdbgfit->Divide(2,2);
         TCanvas* cdbgll = new TCanvas("cdbgll","cdbgll",800,800);
         cdbgll->Divide(2,2);
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            Printf("delay[%d]=%g",ch,tofil[2+(2*ch)]);
            cdbgco->cd(ch+1);
            gcor[ch][0]->SetMarkerStyle(24);
            gcor[ch][0]->SetMarkerColor(kRed);
            gcor[ch][0]->SetLineColor(kRed);
            gcor[ch][0]->Draw("ap");
            gspl[ch][0]->SetMarkerStyle(7);
            gspl[ch][0]->SetMarkerColor(kAzure-7);
            gspl[ch][0]->SetLineColor(kAzure-7);
            gspl[ch][0]->Draw("pcsame");
            cdbgfit->cd(ch+1);
            //gdbgfit[ch]->SetMarkerStyle(7);
            gdbgfit[ch]->Draw("ap");
            cdbgll->cd(ch+1);
            gdbgll[ch]->Draw("ap");
         }
         cdbgco->cd();
         cdbgco->Update();
         cdbgfit->cd();
         cdbgfit->Update();
         cdbgll->cd();
         cdbgll->Update();
         TCanvas* cdbgdt = new TCanvas("cdbgdt","cdbgdt",800,800);
         cdbgdt->Divide(2,2);
         Float_t xxsm[NSnConstants::kNsamps];
         for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
            xxsm[i] = i;
         }

         TGraph* gorg[NSnConstants::kNchans];
         TGraph* gocp[NSnConstants::kNchans];
         TGraph* goen[NSnConstants::kNchans];
         TGraph* gshf[NSnConstants::kNchans];
         TGraph* gshe[NSnConstants::kNchans];
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            gorg[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                  &(rfiltered[ch][0]));
            goen[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                  &(renvelope[ch][0]));
            gocp[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                  &(bfiltered[ch][0]));
            Double_t dt=tofil[(2*ch)+2];
            dt *= kSmpRate; // convert to samples
            Printf("dt[%d]=%g",ch,dt);
            gshf[ch] = new TGraph(NSnConstants::kNsamps);
            gshe[ch] = new TGraph(NSnConstants::kNsamps);
            for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
               gshf[ch]->SetPoint(i, xxsm[i]+dt, bfiltered[ch][i]);
               gshe[ch]->SetPoint(i, xxsm[i]+dt, benvelope[ch][i]);
            }
            gshf[ch]->SetMarkerStyle(7);
            gshf[ch]->SetLineColor(kRed);
            gshf[ch]->SetMarkerColor(kRed);
            gshe[ch]->SetMarkerStyle(7);
            gshe[ch]->SetLineColor(kViolet-1);
            gshe[ch]->SetMarkerColor(kViolet-1);
            gorg[ch]->SetMarkerStyle(7);
            gorg[ch]->SetLineColor(kBlack);
            gorg[ch]->SetMarkerColor(kBlack);
            goen[ch]->SetMarkerStyle(7);
            goen[ch]->SetLineColor(kGray);
            goen[ch]->SetMarkerColor(kGray);
            gocp[ch]->SetMarkerStyle(7);
            gocp[ch]->SetLineStyle(7);
            gocp[ch]->SetLineColor(kGreen+2);
            gocp[ch]->SetMarkerColor(kGreen+2);
         }
         gStyle->SetOptStat(0);
         Int_t pp=0;
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            cdbgdt->cd(++pp);
            hn = Form("hd_%d",ch);
            TH2F* hd = new TH2F(hn.Data(),
                                "black=ref, red/green=bounce;sample;ADC",
                                NSnConstants::kNsamps, -0.5,
                                NSnConstants::kNsamps-0.5,
                                200, -900, 900);
            hd->SetBit(TH1::kCanDelete);
            hd->Draw();
            gorg[ch]->Draw("pc");
            goen[ch]->Draw("pc");
            gocp[ch]->Draw("pc");
            gshf[ch]->Draw("pc");
            gshe[ch]->Draw("pc");
         }
         cdbgdt->cd();
         cdbgdt->Update();
         cdbgco->Update();
         cdbgfit->Update();
         cdbgdt->WaitPrimitive();
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            delete gorg[ch]; delete gocp[ch]; delete goen[ch];
            delete gshf[ch];
            delete gshe[ch];
            delete gdbgfit[ch]; gdbgfit[ch]=0;
         }
         delete cdbgdt;
         delete cdbgco;
         delete cdbgfit;
         delete cdbgll;
#endif
         delete[] tofil;

      }

      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         delete gcor[ch][0]; gcor[ch][0]=0;
         delete gspl[ch][0]; gspl[ch][0]=0;
         delete gchi[ch][0]; gchi[ch][0]=0;
         delete gspc[ch][0]; gspc[ch][0]=0;
      }

   } // event
   
   
   outf->Write();
   
}
   
