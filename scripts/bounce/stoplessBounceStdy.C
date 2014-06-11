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
#include <TVector3.h>
#include <TSpline.h>
#include <TRandom3.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <NSnConstants.h>
#include <TSnSpectral.h>

#include <cmath> // for isnan
#endif

//#define DEBUG_FILT
//#define DEBUG_CORRELS
//#define DEBUG_DTS

TFile* outf=0, * fpnf=0;
TChain* nt;
TNtuple* tchdts, * tpf;
TGraphErrors* gPed[NSnConstants::kNchans]={0},
            * gNoise[NSnConstants::kNchans]={0};
ROOT::Math::Minimizer* mini(0);

TGraph*   gcor[NSnConstants::kNchans][NSnConstants::kNchans];
TSpline3* gspl[NSnConstants::kNchans][NSnConstants::kNchans];


static const Float_t kDelta     = 1.0 / NSnConstants::kSampRate;
static const Int_t   kHalfsamps = NSnConstants::kNsamps/2;
static const Float_t kHalfsampF = 
   static_cast<Float_t>(NSnConstants::kNsamps)/2.0;
static const Float_t kfincr     = 1.0 / (kDelta*NSnConstants::kNsamps);
static const Double_t maxdt = NSnConstants::kNsamps  / 
   (2.0*NSnConstants::kSampRate);
static const UInt_t ncombs = static_cast<UInt_t>(
   TMath::Binomial(NSnConstants::kNchans,2));
static const Double_t kNgTopFern   = 1.33;
static const Double_t kC_m_ns      = TMath::C() / 1.0e9;

static const Float_t dtCorrs[] = {
   -0.379, -1.120, -0.740, -0.903, -0.523, 0.217   // miniuit2
   //-0.180, -0.990, -0.787, -0.660, -0.550, 0.316   // simanl
};


const TVector3& getStnPos(const Int_t ch) {
   //
   //          0             y
   //          | 3m          |
   //     3----+----1        +--- x
   //          |            /
   //          2           z
   //
   // y-axis positive = north
   // x-axis positive = east
   //
   // chan 0 points roughly north (to Mt Discovery)
   // so for now, set stations with 0 along +y axis
   
   //static const Float_t ad = 6.0 / TMath::Sqrt2();
   static const Float_t ad = 3.0;
   static TVector3* apos[NSnConstants::kNchans] = {0};
   static Bool_t init = kTRUE;
   if (init) {
      Printf("Setting station positions...");
      apos[0] = new TVector3(  0,  ad, 0);
      apos[1] = new TVector3( ad,   0, 0);
      apos[2] = new TVector3(  0, -ad, 0);
      apos[3] = new TVector3(-ad,   0, 0);
      init = kFALSE;
   }
   return *(apos[ch]);
}

Double_t getProbFromNormCorrCoef(const Double_t corco) {
   const Double_t c = (corco + 1.0) / 2.0;
   return c*c;
}

Double_t getDeltaTsLL(const Double_t* dts) {
   // expect 6 parameters ordered like so:
   // dts[0] = ch1 - ch0        dts[3] = ch3 - ch0
   // dts[1] = ch2 - ch0        dts[4] = ch3 - ch1
   // dts[2] = ch2 - ch1        dts[5] = ch3 - ch2
   // (i.e. ordered i,j for i=0..3, j<i)
   //
   // the index of chX - chY is then
   // dt10 + dt20 + dt21 + ... + dt(X-1)    + dtX0 + dtX1 ... + dtXY
   // = (1     + 2       + ...    + X-1)         + Y
   // = ( X*(X-1)/2 ) + Y
   //
   // expect gspl to already be constructed with a spline fit
   // to the NORMALIZED correlation function

   Double_t c, dt, od(1), tc(1.0);
   Int_t di=0;
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         dt = dts[di++];
         Bool_t oob=kFALSE;
         if (dt<-maxdt) {
            od = dt + maxdt;
            dt = -maxdt;
            oob = kTRUE;
         } else if (dt>maxdt) {
            od = dt - maxdt;
            dt = maxdt;
            oob = kTRUE;
         }
         dt *= NSnConstants::kSampRate;
         c = getProbFromNormCorrCoef(gspl[ch][xc]->Eval(dt));
         if (oob) {
            c /= (od*od);
         }
         if (isnan(c)) {
            Printf("NAN at ch=%d, xc=%d, dt=%g, "
                   "dts=(%g, %g, %g, %g, %g, %g), spl=%g:",
               ch, xc, dt, dts[0], dts[1], dts[2], dts[3],
               dts[4], dts[5], gspl[ch][xc]->Eval(dt));
         }
         tc *= c;
      }
   }
   // apply the contraints
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         if ((ch-xc)>1) {
            dt = 0;
            for (Int_t xi=xc+1; xi<=ch; ++xi) {
               // see comments of this function for how to
               // calculate the index of dts. we want dt_xi - dt_(xi-1)
               dt += dts[ (xi*(xi-1)/2)+(xi-1) ];
            }
            tc *= TMath::Gaus( dts[ (ch*(ch-1)/2)+xc ],
                               dt,
                               0.05 ); // ns
         }
      }
   }
   
   return (-TMath::Log(tc));
}

Double_t getAngleLL(const Double_t* thetaPhi) {
   // thetaPhi[0] = theta (rad)
   // thetaPhi[1] = phi (rad)
   
   TVector3 norm;
   norm.SetMagThetaPhi(1.0, thetaPhi[0], thetaPhi[1]);
   Double_t dts[ncombs];
   Int_t i=0;
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         const TVector3& posCh = getStnPos(ch);
         const TVector3& posXc = getStnPos(xc);
         const Double_t  disCh = -(posCh.Dot(norm));
         const Double_t  disXc = -(posXc.Dot(norm));
         dts[i] = dtCorrs[i] + ( (disCh-disXc) * kNgTopFern / kC_m_ns );
         Printf("dt (%d,%d) = %g",ch,xc,(disCh-disXc) * kNgTopFern / kC_m_ns );
         ++i;
      }
   }
   
   return getDeltaTsLL(dts);
}

Double_t getAngleLLDeg(const Double_t* thetaPhi) {
   // thetaPhi[0] = theta (deg)
   // thetaPhi[1] = phi (deg)
   const Double_t tp[2] = { thetaPhi[0]*TMath::DegToRad(),
                            thetaPhi[1]*TMath::DegToRad() };
   return getAngleLL(tp);
}

template<typename T>
void filterWaveform(T* const dat,
                    const Int_t    n) {

   // do the FFT
   TSnSpectral::RealFFT(dat, n, kFALSE);
   // apply the filter
   static TF1* wvfilt = new TF1("wvfilt",
                                //"ROOT::Math::chisquared_pdf([0]*x,[1])",
                                //0,2);
                                "gaus(0) + gaus(3)", -1.1, 1.1);
   wvfilt->SetNpx(1000);
   wvfilt->SetParameters(1.0, -0.3, 0.1, 1.0, 0.3, 0.1);

   T* ed = dat+2, * od = dat+1;
   Double_t f,
            x = -kfincr/2.0;
   const Double_t bw = ((kHalfsamps*kfincr)-x)/(kHalfsamps+1);
   for (Int_t i=2; i<n; i+=2, ed+=2, od+=2, x+=bw) {
      f = wvfilt->Eval(x);
      *ed *= f;
      *od *= f;
   }

   // undo the FFT
   TSnSpectral::RealFFT(dat, n, kTRUE);
   
}

void stoplessBounceStdy(const Char_t* rtfn,
                        const Char_t* FPNfn,
                        const Char_t* outtag,
                        const Int_t   fitType=0,
                        const Int_t   fitOpt=0,
                        const Char_t* minner="Minuit2",
                        const Char_t* algo="Migrad",
                        const Int_t   filter=1) {
   // fitType:
   //   0 = fit theta phi with getDeltaTsLL
   //  10 = scan theta,phi space instead of fitting
   //  20 = fit deltaT's
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
   //
   // filter:
   //   0 = no filter
   //   1 = double gaussian at +/- 300MHz with sig=100MHz
   
   if (mini==0) {
      mini = ROOT::Math::Factory::CreateMinimizer(minner, algo);
      mini->SetMaxFunctionCalls(1000000);
      mini->SetMaxIterations(10000);
      mini->SetTolerance(0.001);
      mini->SetPrintLevel(0);
   }

   nt = new TChain("runtree");
   nt->Add(rtfn);

   const Long64_t nents = nt->GetEntries();
   if (nents==0) {
      Error("noAveBunceStdy","No events in tree from [%s].",rtfn);
      return;
   }

   fpnf = TFile::Open(FPNfn);
   if ( (fpnf==0) || (fpnf->IsZombie()) ) {
      Error("bounceStudy","Could not open FPN file [%s]",FPNfn);
      return;
   }
   
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      gPed[ch]   = dynamic_cast<TGraphErrors*>(
         fpnf->Get(Form("gExlPed_ch%d",ch)));
      gNoise[ch] = dynamic_cast<TGraphErrors*>(
         fpnf->Get(Form("gExlRms_ch%d",ch)));
      if ( (gPed[ch]==0) || (gNoise[ch]==0) ) {
         Error("bounceStudy",
               "Couldn't get FPN/noise graphs from [%s]",FPNfn);
         return;
      }
   }
   
   // read from tree into...
   Float_t  pedsubs[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  filtered[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  envelope[NSnConstants::kNchans][NSnConstants::kNsamps];
   UShort_t samples[NSnConstants::kNchans][NSnConstants::kNsamps];
   UInt_t   evnum, utime, utimeus, mbchksum;
   
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("EvtNum",&evnum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
   }
   
   // output
   TString hn;
   hn = Form("%s.ft%dfo%d.%s%s.flt%d.root",
             outtag,fitType,fitOpt,minner,algo,filter);
   outf = TFile::Open(hn.Data(),"recreate");
   
   if (fitType==20) {
      hn = "EvtNum:";
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         for (Int_t xc=0; xc<ch; ++xc) {
            hn += Form("dt%d%d:",ch,xc);
         }
      }
      hn += "chi2";
      tchdts = new TNtuple("tChanDTsFit","fit channel DTs",
                           hn.Data());
   } else {
      tpf = new TNtuple("tThetaPhiFit","theta,phi fit",
                        "EvtNum:theta:phi:chi2");
   }

   for (Long64_t ev=0; ev<nents; ++ev) {
      //for (Long64_t ev=0; ev<250; ++ev) {
      if ( (ev%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            ev, nents,
            100.*static_cast<Float_t>(ev)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(ev);
      
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         // subtract FPN
         const Double_t* pd = gPed[ch]->GetY();
         const UShort_t* sp = &(samples[ch][0]);
         Float_t*        ps = &(pedsubs[ch][0]);
         for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++pd, ++sp, ++ps) {
            *ps = *sp - *pd;
         }
         
         // apply filter
         memcpy(&(filtered[ch][0]), &(pedsubs[ch][0]),
                NSnConstants::kNsamps * sizeof(Float_t));
         if (filter>0) {
            filterWaveform(&(filtered[ch][0]), NSnConstants::kNsamps);
         }
         // find envelope
         memcpy(&(envelope[ch][0]), &(filtered[ch][0]),
                NSnConstants::kNsamps * sizeof(Float_t));
         TSnSpectral::EnvelopeReal(&(envelope[ch][0]), NSnConstants::kNsamps);
         
#ifdef DEBUG_FILT
         TCanvas* cdbgflt = new TCanvas;
         cdbgflt->cd();
         Float_t xsm[NSnConstants::kNsamps], rsmp[NSnConstants::kNsamps];
         for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
            xsm[i] = i;
            rsmp[i] = samples[ch][i];
         }
         TGraph* gsmp = new TGraph(NSnConstants::kNsamps, xsm, rsmp);
         TGraph* gps  = new TGraph(NSnConstants::kNsamps, xsm, 
                                   &(pedsubs[ch][0]));
         TGraph* gfl  = new TGraph(NSnConstants::kNsamps, xsm,
                                      &(filtered[ch][0]));
         TGraph* gen  = new TGraph(NSnConstants::kNsamps, xsm,
                                   &(envelope[ch][0]));
         gsmp->SetMarkerStyle(7);
         gsmp->SetMarkerColor(kBlack);
         gsmp->SetLineColor(kBlack);
         gps->SetMarkerStyle(7);
         gps->SetMarkerColor(kRed);
         gps->SetLineColor(kRed);
         gfl->SetMarkerStyle(7);
         gfl->SetMarkerColor(kGreen+2);
         gfl->SetLineColor(kGreen+2);
         gen->SetMarkerStyle(7);
         gen->SetMarkerColor(kViolet-1);
         gen->SetLineColor(kViolet-1);
         gsmp->SetMinimum(-1500);
         gsmp->SetMaximum(2500);
         gsmp->Draw("apc");
         gps->Draw("pc");
         gfl->Draw("pc");
         gen->Draw("pc");
         cdbgflt->cd();
         cdbgflt->Update();
         cdbgflt->WaitPrimitive();
         delete gsmp; delete gps; delete gfl; delete gen;
         delete cdbgflt;
#endif         
         
      } // end chan loop
      
      // have filtered data for each channel. can correlate them now.
      const Float_t* chdat(0), * xcdat(0);
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         if (fitOpt==0) {
            chdat = &(filtered[ch][0]);
         } else if (fitOpt==10) {
            chdat = &(envelope[ch][0]);
         } else {
            Fatal("noAveBounceStdy","Unknown fitOpt [%d]",fitOpt);
         }
         for (Int_t xc=0; xc<ch; ++xc) {
            if (fitOpt==0) {
               xcdat = &(filtered[xc][0]);
            } else if (fitOpt==10) {
               xcdat = &(envelope[xc][0]);
            } else {
               Fatal("noAveBounceStdy","Unknown fitOpt [%d]",fitOpt);
            }
            
            gcor[ch][xc] = TSnSpectral::NewCorrelateGraph(chdat, xcdat,
                                                          NSnConstants::kNsamps,
                                                          kTRUE);
            gspl[ch][xc] = new TSpline3(Form("spl_%d_%d",ch,xc),
                                        gcor[ch][xc]);

#ifdef DEBUG_CORRELS
            TCanvas* cdbgcl = new TCanvas("cdbgcl","cdbgcl",800,1000);
            cdbgcl->Divide(1,2);
            cdbgcl->cd(1);
            Float_t xxsm[NSnConstants::kNsamps];
            for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
               xxsm[i] = i;
            }
            TGraph* gcfl  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(filtered[ch][0]));
            TGraph* gxfl  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(filtered[xc][0]));
            TGraph* gcen  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(envelope[ch][0]));
            TGraph* gxen  = new TGraph(NSnConstants::kNsamps, xxsm,
                                       &(envelope[xc][0]));
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
            gspl[ch][xc]->SetMarkerStyle(7);
            gspl[ch][xc]->SetMarkerColor(kAzure-7);
            gspl[ch][xc]->SetLineColor(kAzure-7);
            gspl[ch][xc]->Draw("pc");
            cdbgcl->cd();
            cdbgcl->Update();
            cdbgcl->WaitPrimitive();
            delete gcfl; delete gxfl; delete cdbgcl;
            delete gcen; delete gxen;
#endif

         } // end xc
      } // end ch
      
      if (fitType==20) {
         // fit for best correlations
         mini->Clear();
         ROOT::Math::Functor f(&getDeltaTsLL, ncombs);
         mini->SetFunction(f);
         Int_t di(0);
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            for (Int_t xc=0; xc<ch; ++xc) {
               if (strcmp(minner,"Genetic")==0) {
                  mini->SetLimitedVariable(di, Form("dt%d%d",ch,xc), 0, 0.01,
                                           -maxdt, maxdt);
               } else {
                  mini->SetVariable(di, Form("dt%d%d",ch,xc), 0, 0.01);
               }
               ++di;
            }
         }
         mini->Minimize();
         const Double_t* result = mini->X();
         Float_t* tofil = new Float_t[ncombs+2];
         Float_t* tf = tofil;
         *tf++ = ev;
         for (UInt_t i=0; i<ncombs; ++i, ++tf) {
            *tf = static_cast<Float_t>(result[i]);
         }
         *tf = mini->MinValue();
         tchdts->Fill(tofil);
         delete[] tofil;

      } else if (fitType==10) {
         // scan theta/phi space
         if (ev<10) {
            Printf("Scanning...");
            Double_t tp[2] = { 0,0 };
            const Double_t step=0.0100;
            const Int_t nts = static_cast<Int_t>(TMath::Pi() / step);
            const Int_t pts = 2*nts;
            for (Int_t i=0; i<=nts; ++i, tp[0]+=step) {
               tp[1] = 0;
               for (Int_t j=0; j<=pts; ++j, tp[1]+=step) {
                  tpf->Fill(ev, i, j, getAngleLL(tp));
               }
            }
         } else break;
         
      } else {
         // fit theta and phi
         mini->Clear();
         ROOT::Math::Functor f(&getAngleLL, 2);
         mini->SetFunction(f);
         //if (strcmp(minner,"Genetic")==0) {
            mini->SetLimitedVariable(0, "theta", 3.14, 0.001,
                                     //0, TMath::Pi());
                                     TMath::Pi()/2.0, TMath::Pi());
            mini->SetLimitedVariable(1, "phi", 5.5, 0.001,
                                     0, TMath::TwoPi());
            /*
         } else {
            mini->SetVariable(0, "theta", 3, 0.01);
            mini->SetVariable(1, "phi", 5.5, 0.01);
         }
            */
         mini->Minimize();
         const Double_t* result = mini->X();
         tpf->Fill(ev, result[0], result[1], mini->MinValue());
         
      }
      
      for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         for (Int_t xc=0; xc<ch; ++xc) {
            delete gspl[ch][xc];
            delete gcor[ch][xc];
         }
      }

   } // end event loop
   
   outf->Write();
   
}
