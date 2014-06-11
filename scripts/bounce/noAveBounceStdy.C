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

//#define DEBUG_SHIFTS
//#define DEBUG_CORRELS
//#define DEBUG_DTS
//#define DEBUG_THETAPHI

#define SHIFT_INTO_WINDOW

TFile* outf=0, * fpnf=0;
TNtuple* tThetaPhiFit, * tChanDTsFit;

TChain* nt, * ns;
TGraphErrors* gPed[NSnConstants::kNchans]={0},
            * gNoise[NSnConstants::kNchans]={0};
ROOT::Math::Minimizer* mini(0);

static const Double_t kNgTopFern   = 1.33;
static const Double_t kC_m_ns      = TMath::C() / 1.0e9;
static const Double_t kReallySmall = 1e-12;
static const Double_t kReallyBig   = 1e50;
Float_t dtCorrs[NSnConstants::kNchans-1] = {
   //0,0,0
   -0.341, -0.722, 0.294
   //0.803, -1.50, -0.874 // run11, siman, ll, 10-70, ave
   //1.02, -2.07, -1.81 // run11, siman, ll, 10-70, peak pos
   //-0.842, 1.91, -0.706 // run11, siman, envelope, 10-70, ave
   //-1.42, 2.80, -1.26   // run11, siman, envelope, 10-70, peak pos
   //0.394, -0.208, 2.412    // siman, envelope, 20-60
   //0.548, -0.24, 2.23    // siman, envelope, 10-70
   //-0.336, -0.718, 0.286
   //-0.379, -0.740, 0.215 // noAveBounce Migrad, gaus filter mean=0.3, sig=0.1
   //-0.424, -0.763, 0.136 // noAveBounce Migrad, gaus filter
   //-0.383, -0.783, 0.178 // noAveBounce Migrad, with filter
   //-0.293, -0.820, 0.153 // noAveBounce Migrad
   //0.459, -0.469, 0.621 // fake, Migrad
   //-0.73, 0.50, -0.68 // 1-0, 2-1, 3-2    Fumili2
   //-1.04, 0.52, -0.9 // 1-0, 2-1, 3-2
   //-0.729, 0.487, -0.660  //              GSLSimAn
};
static const Float_t kSmpRate   = 1.92; // GHz
static const Float_t kDelta     = 1.0 / kSmpRate;
static const Int_t   kHalfsamps = NSnConstants::kNsamps/2;
static const Float_t kHalfsampF = 
   static_cast<Float_t>(NSnConstants::kNsamps)/2.0;
static const Float_t kfincr     = 1.0 / (kDelta*NSnConstants::kNsamps);


TH2F* hFiltCorrCoefShiftVsEvt[NSnConstants::kNchans][NSnConstants::kNchans];
TH2F* hFiltShiftChi2VsEvt[NSnConstants::kNchans][NSnConstants::kNchans];

Double_t      maxdt(0);
TGraphErrors* gcor[NSnConstants::kNchans][NSnConstants::kNchans];
TSpline3*     gspl[NSnConstants::kNchans][NSnConstants::kNchans];
TGraph*       gchi[NSnConstants::kNchans][NSnConstants::kNchans];
TSpline3*     gspc[NSnConstants::kNchans][NSnConstants::kNchans];

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

Double_t getProbFromCorrCoef(const Double_t corco) {
   Double_t c = corco + 1.0;
   c /= 2.0;
   c *= c;
   return c;
}

Double_t getDeltaTsLL(const Double_t* dts) {
   // dts must be indexed like so:
   // [0] = 1-0
   // [1] = 2-1
   // [2] = 3-2
   // ...
   //
   // dt's are then calculated by:
   // 1-0 = dts[0] = dts[ch-(ch-xc)] = dts[1-(1-0)]
   // 2-1 = dts[1] = dts[2-(2-1)]
   // 3-0 = dts[3-(3-0)] + dts[3-(3-0-1)] + dts[3-(3-0-2)]
   //     =       dts[0] +         dts[1] +         dts[2]
   //     =          1-0 +            2-1 +            3-2 = 3-0
   Double_t chi2(1), c(0), oo(0);

   for (Int_t ch=1; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         Double_t dt=0;
         for (Int_t i=(ch-xc); i>0; --i) {
            dt += dts[ch-i];
         }
         const Double_t odt = dt;
         Bool_t oob=kFALSE;
         if (dt<-maxdt) {
            dt  = -maxdt;
            oob = kTRUE;
         } else if (dt>maxdt) {
            dt  = maxdt;
            oob = kTRUE;
         }
         dt *= kSmpRate; // convert to samples
         c = getProbFromCorrCoef(gspl[ch][xc]->Eval(dt));
         if (oob) {
            const Double_t wa = TMath::Abs(odt) - maxdt;
            oo += wa*wa;
         }
         /*
         if (TMath::Abs(dt)>maxdt) {
            // really don't like being out of bounds
            c = TMath::Power(dt, -2.0);
         } else {
            // how acceptable is this dt?
            const Double_t  corco = gspl[ch][xc]->Eval(dt);
            c = getProbFromCorrCoef(corco);
         }
         */
         if (isnan(c)) {
            Printf("d[%g, %g, %g], dt=%g, c=%g", dts[0], dts[1], dts[2], dt, c);
            Printf("ch=%d, xc=%d, gspl[ch][xc]=%p",ch,xc,gspl[ch][xc]);
            for (Int_t i=-63; i<63; ++i) {
               printf("(%d, %g), ",
                      i, gspl[ch][xc]->Eval(i));
            }
            printf("\n");
            gspl[ch][xc]->Draw("apl");
            if (gPad!=0) {
               gPad->Update();
               gPad->WaitPrimitive();
            }
         }
         chi2 *= c;
      } // xc
   } // ch
   chi2 = -TMath::Log(chi2);
   chi2 += oo;
   return chi2;
}

Double_t getShiftLL(const Double_t* thetaPhi) {
   
   Double_t chi2(1), c(0), oo(0);

   TVector3 norm; norm.SetMagThetaPhi(1.0, thetaPhi[0], thetaPhi[1]);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         Double_t dtcor=0;
         for (Int_t i=(ch-xc); i>0; --i) {
            dtcor += dtCorrs[ch-i];
         }
         const TVector3& posCh = getStnPos(ch);
         const TVector3& posXc = getStnPos(xc);
         const Double_t  disCh = -(posCh.Dot(norm));
         const Double_t  disXc = -(posXc.Dot(norm));
         // !!! check sign of delta(distance) and dtcor!
         Double_t  dt = 
            ( (disCh-disXc) * kNgTopFern / kC_m_ns ) // from m to ns
            + dtcor;                                 // correct dt offset (ns)
         const Double_t odt = dt;
         Bool_t oob=kFALSE;
         if (dt<-maxdt) {
            dt  = -maxdt;
            oob = kTRUE;
         } else if (dt>maxdt) {
            dt  = maxdt;
            oob = kTRUE;
         }
         dt *= kSmpRate;
         c = getProbFromCorrCoef(gspl[ch][xc]->Eval(dt));
         if (oob) {
            const Double_t wa = TMath::Abs(odt) - maxdt;
            oo += wa*wa;
         }
         /*
         if (TMath::Abs(dt)>maxdt) {
            // really don't like being out of bounds
            c = TMath::Exp(-dt*dt);
         } else {
            // get the correlation coefficient for this dt (in num samples)
            const Double_t  corco = gspl[ch][xc]->Eval(dt);
            c = getProbFromCorrCoef(corco);
         }
         if (c>1.0) {
            Fatal("getShiftLL","Got ll term > 1 (%g)",c);
         }
         */
         chi2 *= c;
      }
   }
   chi2 = -TMath::Log(chi2);
   chi2 += oo;
   return chi2;
}

Double_t getShiftChi2(const Double_t* thetaPhi) {
   Double_t chi2(0), c(0);

   TVector3 norm; norm.SetMagThetaPhi(1.0, thetaPhi[0], thetaPhi[1]);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         Double_t dtcor=0;
         for (Int_t i=(ch-xc); i>0; --i) {
            dtcor += dtCorrs[ch-i];
         }
         const TVector3& posCh = getStnPos(ch);
         const TVector3& posXc = getStnPos(xc);
         const Double_t  disCh = -(posCh.Dot(norm));
         const Double_t  disXc = -(posXc.Dot(norm));
         // !!! check sign of delta(distance) and dtcor!
         const Double_t  dt = kSmpRate * (             // from ns to samples
            ( (disCh-disXc) * kNgTopFern / kC_m_ns )   // from m to ns
            + dtcor );                                 // correct dt offset (ns)
         // FIXME: dt in samples, maxdt in ns
         if (TMath::Abs(dt)>maxdt) {
            // really don't like being out of bounds
            c = TMath::Exp(dt*dt);
            if (c>kReallyBig) {
               c = kReallyBig;
            }
         } else {
            // get the correlation coefficient for this dt (in num samples)
            c = gspc[ch][xc]->Eval(dt) - 1.0;
         }
         chi2 += c*c;
      }
   }
   return chi2;
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

template<typename T, typename U>
TGraphErrors* newCorrCoefGraph(const T* const dat1,
                               const U* const dat2,
                               const Int_t    nd,
                               const Int_t    minbin=-1, // -1 => 0
                               const Int_t    maxbin=-1  // -1 => nd-1
                               ) {
   const Int_t minb = (minbin<0) ?    0 : minbin;
   const Int_t maxb = (maxbin<0) ? nd-1 : maxbin;
   if ( (maxb<=minb) || (maxb>=nd) ) {
      Fatal("newCorrCoefGraph",
            "Invalid maxb=%d. (minb=%d, nd=%d)",
            maxb, minb, nd);
   }

   // find correlation coef for different shifts
   //const Int_t ndh = nd/2;
   const T* c1 = dat1+minb;
   const U* c2 = dat2+minb;
   const Int_t ndh = (maxb-minb+1)/2;
   Int_t pos, j(0);
   TGraphErrors* gc = new TGraphErrors;
   for (Int_t sh=1-ndh; sh<ndh; ++sh) {
      Double_t a1(0), a2(0), r1(0), r2(0);
      Double_t cor(0), ncr(0);
      c1 = dat1+minb;
      c2 = dat2+minb-sh;
      for (Int_t i=minb; i<=maxb; ++i, ++c1, ++c2) {
         pos = c2 - dat2;
#ifdef SHIFT_INTO_WINDOW
         if (pos<nd) { // CHECK: can c1 go above nd?
            if (pos>=0) {
#else
         if (pos<=maxb) {
            if (pos>=minb) {
#endif
               a1  += *c1;
               a2  += *c2;
               r1  += (*c1)*(*c1);
               r2  += (*c2)*(*c2);
               cor += (*c1)*(*c2);
               ncr += 1.0;
            }
         } else {
            break;
         }
      } // sample
      if (ncr>1) {
         // rms
         r1  -= (a1*a1)/ncr;
         r2  -= (a2*a2)/ncr;
         r1  /= ncr-1.0;
         r2  /= ncr-1.0;
         r1   = TMath::Sqrt(r1);
         r2   = TMath::Sqrt(r2);
         // ave
         a1  /= ncr;
         a2  /= ncr;
         // cor
         cor -= ncr*a1*a2;
         cor /= (ncr-1.0)*r1*r2;
         gc->SetPoint(j, static_cast<Double_t>(sh), cor);
         gc->SetPointError(j, 0, 
            (1.0-(cor*cor))/TMath::Sqrt(ncr-1.0));
         ++j;
      }
   } // shift

   return gc;
}



template<typename T, typename U>
TGraph* newShiftChi2Graph(const T* const dat1,
                          const Int_t    nd1,
                          const U* const dat2,
                          const Int_t    nd2,
                          const Int_t    minbin=-1,   // -1 => 0
                          const Int_t    maxbin=-1,   // -1 => nd1-1
                          const Bool_t   useSqrtErrs=kFALSE) {
   const Int_t minb = (minbin<0) ?     0 : minbin;
   const Int_t maxb = (maxbin<0) ? nd1-1 : maxbin;
   if ( (maxb<=minb) || (maxb>=nd1) || (maxb>=nd2) ) {
      Fatal("newShiftChi2Graph",
            "Invalid maxb=%d. (minb=%d, nd1=%d, nd2=%d)",
            maxb, minb, nd1, nd2);
   }
   const T* c1 = dat1;
   const U* c2 = dat2;
   Int_t pos, j(0);
   const Int_t ndh = (maxb-minb+1)/2;
   TGraphErrors* gc = new TGraphErrors;
   for (Int_t sh=1-ndh; sh<ndh; ++sh) {
      Double_t x=0, t=0, cmp=0;
      c1 = dat1+minb;
      c2 = dat2+minb-sh;

      for (Int_t i=minb; i<=maxb; ++i, ++c1, ++c2) {
         pos = c2 - dat2;
#ifdef SHIFT_INTO_WINDOW
         if (pos<nd2) {
            if (pos>=0) {
#else
         if (pos<=maxb) {
            if (pos>=minb) {
#endif
               t  = (*c1) - (*c2);
               t *= t;
               if (useSqrtErrs) {
                  t /= TMath::Abs(static_cast<Double_t>(*c2));
               }
               x += t;
               cmp += 1.0;
            }
         } else {
            break;
         }
      }
      if (cmp>1.0) {
         x /= cmp-1.0;

         gc->SetPoint(j, static_cast<Double_t>(sh), x);
         
         ++j;
      }
   }

   return gc;
}



void noAveBounceStdy(const Char_t* rtfn,
                     const Char_t* wvfn,
                     const Char_t* FPNfn,
                     const Char_t* outfn,
                     const Int_t   fitType=0,
                     const Int_t   fitOpt=0,
                     const Char_t* minner="Minuit2",
                     const Char_t* algo="Migrad",
                     const Int_t   shiftminb=-1,
                     const Int_t   shiftmaxb=-1,
                     const Bool_t  applyFilter=kTRUE) {
   // fitType:
   //   0 = fit theta phi with getShiftLL
   //   5 = fit theta phi with getShiftChi2
   //  20 = fit 3 deltaT's; use contraints for other 3
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

   nt = new TChain("runtree");
   nt->Add(rtfn);

   const Long64_t nents = nt->GetEntries();
   if (nents==0) {
      Error("noAveBunceStdy","No events in tree from [%s].",rtfn);
      return;
   }

   ns = new TChain("nShifts");
   ns->Add(wvfn);

   if (nents>ns->GetEntries()) {
      Error("bounceStudy","%lld entries in runtree but "
            "%lld entries in shift tree",nents,ns->GetEntries());
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
   Float_t  psshift[NSnConstants::kNchans][NSnConstants::kNsamps];
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

   // shift for stop tree
   Int_t nsent;
   UInt_t nsevn;
   Int_t aveShift(0), aveLen(0);
   Int_t shift[NSnConstants::kNchans];
   Int_t len[NSnConstants::kNchans];
   ns->SetBranchAddress("Ent",&nsent);
   ns->SetBranchAddress("EvtNum",&nsevn);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      ns->SetBranchAddress(Form("shift%02d",ch),&(shift[ch]));
      ns->SetBranchAddress(Form("len%02d",ch),&(len[ch]));
   }
   ns->BuildIndex("EvtNum");

   // output
   TString hn;
   outf = TFile::Open(outfn,"recreate");

   if (fitType==20) {
      hn = "EvtNum:";
      for (Int_t i=1; i<NSnConstants::kNchans; ++i) {
         hn += Form("dt%d%d:",i,i-1);
      }
      hn += "chi2";
      tChanDTsFit = new TNtuple("tChanDTsFit",
                                "fit of channel dts",
                                hn.Data());
   } else {
      tThetaPhiFit = new TNtuple("tThetaPhiFit","theta phi fit tree",
                                 "EvtNum:theta:phi:chi2");
   }


   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      for (Int_t xc=0; xc<ch; ++xc) {
         hn = Form("hFiltCorrCoefShiftVsEvt_ch%02d_ch%02d",ch,xc);
         hFiltCorrCoefShiftVsEvt[ch][xc] = new TH2F(hn.Data(),
              Form("corr coef for offset between ch%d and ch%d of filtered "
                   "wvfms vs event;event;offset (ch%d-ch%d);correlation coef",
                   ch,xc,ch,xc),
                                          nents, -0.5, nents-0.5,
                                          NSnConstants::kNsamps,
                                          1-(NSnConstants::kNsamps/2)-0.5,
                                          (NSnConstants::kNsamps/2)+0.5);
         hFiltCorrCoefShiftVsEvt[ch][xc]->SetDirectory(outf);

         hn = Form("hFiltShiftChi2VsEvt_ch%02d_ch%02d",ch,xc);
         hFiltShiftChi2VsEvt[ch][xc] = new TH2F(hn.Data(),
              Form("#chi^{2} of offset between ch%d and ch%d of filtered "
                   "wvfms vs event;event;offset (ch%d-ch%d);#chi^{2}",
                   ch,xc,ch,xc),
                                          nents, -0.5, nents-0.5,
                                          NSnConstants::kNsamps,
                                          1-(NSnConstants::kNsamps/2)-0.5,
                                          (NSnConstants::kNsamps/2)+0.5);
         hFiltShiftChi2VsEvt[ch][xc]->SetDirectory(outf);
      }
   }

   Long64_t ne(0);
   for (Long64_t ev=0; ev<nents; ++ev) {
   //for (Long64_t ev=0; ev<250; ++ev) {
      if ( (ev%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            ev, nents,
            100.*static_cast<Float_t>(ev)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(ev);
      ne = ns->GetEntryNumberWithIndex(evnum);
      if (ne>-1) {
         ns->GetEntry(ne);

         // find the stop shift
         aveShift = aveLen = 0;
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            aveShift += shift[ch];
            aveLen   += len[ch];
         }
         aveShift = TMath::Nint( static_cast<Float_t>(aveShift)/
                                 static_cast<Float_t>(NSnConstants::kNchans) );
         aveLen   = TMath::Nint( static_cast<Float_t>(aveLen)/
                                 static_cast<Float_t>(NSnConstants::kNchans) );
         const Int_t shiftStart = NSnConstants::kNsamps-1 - aveShift - aveLen;
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            // subtract FPN
            const Double_t* pd = gPed[ch]->GetY();
            const UShort_t* sp = &(samples[ch][0]);
            Float_t*        ps = &(pedsubs[ch][0]);
            Float_t*        ph = &(psshift[ch][ shiftStart ]);
            for (Int_t sm=0; sm<NSnConstants::kNsamps;
                                  ++sm, ++pd, ++sp, ++ps, ++ph) {
               *ps = *sp - *pd;
               while ( (ph-&(psshift[ch][0])) >= NSnConstants::kNsamps) {
                  ph -= NSnConstants::kNsamps;
               }
               *ph = *ps;
            }
            
            // apply filter
            memcpy(&(filtered[ch][0]), &(psshift[ch][0]),
                   NSnConstants::kNsamps*sizeof(Float_t));
            if (applyFilter) {
               filterWaveform(&(filtered[ch][0]), NSnConstants::kNsamps);
            }
            // find envelope
            memcpy(&(envelope[ch][0]), &(filtered[ch][0]),
                   NSnConstants::kNsamps*sizeof(Float_t));
            TSnSpectral::EnvelopeReal(&(envelope[ch][0]), NSnConstants::kNsamps);

#ifdef DEBUG_SHIFTS
            TCanvas* cdbgsh = new TCanvas;
            cdbgsh->cd();
            Float_t xsm[NSnConstants::kNsamps], rsmp[NSnConstants::kNsamps];
            for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
               xsm[i] = i;
               rsmp[i] = samples[ch][i];
            }
            TGraph* gsmp = new TGraph(NSnConstants::kNsamps, xsm, rsmp);
            TGraph* gps  = new TGraph(NSnConstants::kNsamps, xsm, 
                                      &(pedsubs[ch][0]));
            TGraph* gph  = new TGraph(NSnConstants::kNsamps, xsm,
                                      &(psshift[ch][0]));
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
            gph->SetMarkerStyle(7);
            gph->SetMarkerColor(kAzure-7);
            gph->SetLineColor(kAzure-7);
            gfl->SetMarkerStyle(7);
            gfl->SetMarkerColor(kGreen+2);
            gfl->SetLineColor(kGreen+2);
            gen->SetMarkerStyle(7);
            gen->SetMarkerColor(kViolet-1);
            gen->SetLineColor(kViolet-1);
            gsmp->SetMinimum(-1500);
            gsmp->SetMaximum(2500);
            Printf("ch%d: shift=%d, len=%d",
                   ch, shift[ch], len[ch]);
            gsmp->Draw("apc");
            gps->Draw("pc");
            gph->Draw("pc");
            gfl->Draw("pc");
            gen->Draw("pc");
            cdbgsh->WaitPrimitive();
            delete gsmp; delete gps; delete gph; delete gfl; delete gen;
            delete cdbgsh;
#endif
         }
         
         if (shiftmaxb!=shiftminb) {
            maxdt = (shiftmaxb - shiftminb) / (2.0*kSmpRate);
         } else {
            maxdt =  NSnConstants::kNsamps  / (2.0*kSmpRate);
         }
         
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
               gcor[ch][xc] = newCorrCoefGraph(chdat,
                                               xcdat,
                                               NSnConstants::kNsamps,
                                               shiftminb,
                                               shiftmaxb);
               gspl[ch][xc] = new TSpline3(Form("spl_%d_%d",ch,xc),
                                           gcor[ch][xc]);
               gchi[ch][xc] = newShiftChi2Graph(chdat,
                                                NSnConstants::kNsamps,
                                                xcdat,
                                                NSnConstants::kNsamps,
                                                shiftminb,
                                                shiftmaxb);
               gspc[ch][xc] = new TSpline3(Form("spc_%d_%d",ch,xc),
                                           gchi[ch][xc]);

#ifdef DEBUG_CORRELS
            TCanvas* cdbgcl = new TCanvas("cdbgcl","cdbgcl",800,1000);
            cdbgcl->Divide(1,3);
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
            cdbgcl->cd(3);
            gspc[ch][xc]->SetMarkerStyle(7);
            gspc[ch][xc]->SetMarkerColor(kViolet-6);
            gspc[ch][xc]->SetLineColor(kViolet-6);
            gspc[ch][xc]->Draw("pc");
            cdbgcl->cd();
            cdbgcl->Update();
            cdbgcl->WaitPrimitive();
            delete gcfl; delete gxfl; delete cdbgcl;
            delete gcen; delete gxen;
#endif

               const Int_t gn = gcor[ch][xc]->GetN();
               const Double_t* gx = gcor[ch][xc]->GetX(),
                             * gy = gcor[ch][xc]->GetY(),
                             * cx = gchi[ch][xc]->GetX(),
                             * cy = gchi[ch][xc]->GetY();
               for (Int_t k=0; k<gn; ++k, ++gx, ++gy, ++cx, ++cy) {
                  hFiltCorrCoefShiftVsEvt[ch][xc]->Fill(ev, *gx, *gy);
                  hFiltShiftChi2VsEvt[ch][xc]->Fill(ev, *cx, *cy);
               }

            }
         }
         
         if (fitType==20) {
            // fit for the best, consistent time offsets
            mini->Clear();
            ROOT::Math::Functor f(&getDeltaTsLL, NSnConstants::kNchans-1);
            mini->SetFunction(f);
            for (Int_t ch=1; ch<NSnConstants::kNchans; ++ch) {
               if (strcmp(minner,"Genetic")==0) {
                  mini->SetLimitedVariable(ch-1, 
                                           Form("dt%d%d",ch,ch-1), 0, 0.01,
                                           -maxdt, maxdt);
               } else {
                  mini->SetVariable(ch-1, Form("dt%d%d",ch,ch-1), 0, 0.01);
               }
            }
            mini->Minimize();
            const Double_t* result = mini->X();
            const Double_t* rs = result;
            Float_t* tofil = new Float_t[NSnConstants::kNchans+2];
            Float_t* tf = tofil;
            *tf++ = ev;
            for (Int_t ch=1; ch<NSnConstants::kNchans; ++ch, ++tf, ++rs) {
               *tf = *rs;
            }
            *tf = mini->MinValue();
            tChanDTsFit->Fill(tofil);
            delete[] tofil;
#ifdef DEBUG_DTS
            for (Int_t ch=1; ch<NSnConstants::kNchans; ++ch) {
               Printf("result[%d]=%g",ch-1,result[ch-1]);
            }
            TCanvas* cdbgdt = new TCanvas("cdbgdt","cdbgdt",1000,1000);
            cdbgdt->Divide(2,3);
            Float_t xxsm[NSnConstants::kNsamps];
            for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
               xxsm[i] = i;
            }

            TGraph* gorg[NSnConstants::kNchans];
            TGraph* gocp[NSnConstants::kNchans];
            TGraph* goen[NSnConstants::kNchans];
            TGraph* gshf[NSnConstants::kNchans][NSnConstants::kNchans];
            TGraph* gshe[NSnConstants::kNchans][NSnConstants::kNchans];
            for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
               gorg[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                     &(filtered[ch][0]));
               gocp[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                     &(filtered[ch][0]));
               goen[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                     &(envelope[ch][0]));
               for (Int_t xc=0; xc<ch; ++xc) {
                  Double_t dt=0;
                  for (Int_t i=(ch-xc); i>0; --i) {
                     dt += result[ch-i];
                  }
                  dt *= kSmpRate; // convert to samples
                  Printf("dt[%d][%d]=%g",ch,xc,dt);
                  gshf[ch][xc] = new TGraph(NSnConstants::kNsamps);
                  gshe[ch][xc] = new TGraph(NSnConstants::kNsamps);
                  for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
                     gshf[ch][xc]->SetPoint(i, xxsm[i]+dt, filtered[xc][i]);
                     gshe[ch][xc]->SetPoint(i, xxsm[i]+dt, envelope[xc][i]);
                  }
                  gshf[ch][xc]->SetMarkerStyle(7);
                  gshf[ch][xc]->SetLineColor(kRed);
                  gshf[ch][xc]->SetMarkerColor(kRed);
                  gshe[ch][xc]->SetMarkerStyle(7);
                  gshe[ch][xc]->SetLineColor(kViolet-1);
                  gshe[ch][xc]->SetMarkerColor(kViolet-1);
               }
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
               for (Int_t xc=0; xc<ch; ++xc) {
                  cdbgdt->cd(++pp);
                  hn = Form("hd_%d_%d",ch,xc);
                  TH2F* hd = new TH2F(hn.Data(),
                                      Form("black=ch%d, red/green=ch%d;"
                                           "sample;ADC",ch,xc),
                                      NSnConstants::kNsamps, -0.5,
                                      NSnConstants::kNsamps-0.5,
                                      200, -900, 900);
                  hd->SetBit(TH1::kCanDelete);
                  hd->Draw();
                  gorg[ch]->Draw("pc");
                  goen[ch]->Draw("pc");
                  gocp[xc]->Draw("pc");
                  gshf[ch][xc]->Draw("pc");
                  gshe[ch][xc]->Draw("pc");
               }
            }
            cdbgdt->cd();
            cdbgdt->Update();
            cdbgdt->WaitPrimitive();
            for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
               delete gorg[ch]; delete gocp[ch]; delete goen[ch];
               for (Int_t xc=0; xc<ch; ++xc) {
                  delete gshf[ch][xc];
                  delete gshe[ch][xc];
               }
            }
            delete cdbgdt;
#endif
         } else {

            Int_t npars(2);
            Double_t (*fitFunction)(const Double_t*) = 0;
            if (fitType==0) {
               fitFunction = &getShiftLL;
            } else if (fitType==5) {
               fitFunction = &getShiftChi2;
            } else {
               Fatal("bounceStudy","Unknown fit type %d",fitType);
            }
            
            mini->Clear();
            ROOT::Math::Functor f(fitFunction, npars);
            mini->SetFunction(f);
            
            if (strcmp(minner,"Genetic")==0) {
               mini->SetLimitedVariable(0, "theta",     3, 0.01,
                                        0, TMath::Pi());
               mini->SetLimitedVariable(1, "phi"  ,   5.5, 0.01,
                                        0, TMath::TwoPi());
            } else {
               //mini->SetVariable(0, "theta",             TMath::Pi(), 0.01);
               //mini->SetVariable(1, "phi"  ,   (gRandom->Rndm()-0.5), 0.01);
               mini->SetVariable(0, "theta",     3, 0.01);
               mini->SetVariable(1, "phi"  ,   5.5, 0.01);
               //mini->SetVariable(0, "theta",     3, 0.05);
               //mini->SetVariable(1, "phi"  ,   5.5, 0.05);
            }
            mini->Minimize();
            const Double_t* result = mini->X();
            tThetaPhiFit->Fill(ev, result[0], result[1], mini->MinValue());


#ifdef DEBUG_THETAPHI
            // regularize the angles
            Double_t theta = TVector2::Phi_mpi_pi(result[0]);
            Double_t phi   = result[1];
            if (theta<0) {
               theta *= -1.0;
               phi   += TMath::Pi();
            }
            phi   = TVector2::Phi_0_2pi(phi);
            Printf("theta=%g, phi=%g (%g, %g)",
                   theta*TMath::RadToDeg(),
                   phi*TMath::RadToDeg(),
               result[0], result[1]);
            TCanvas* cdbgtp = new TCanvas("cdbgtp","cdbgtp",1000,1000);
            cdbgtp->Divide(2,3);
            Float_t xxsm[NSnConstants::kNsamps];
            for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
               xxsm[i] = i;
            }
            
            TGraph* gorg[NSnConstants::kNchans];
            TGraph* gocp[NSnConstants::kNchans];
            TGraph* gshf[NSnConstants::kNchans][NSnConstants::kNchans];
            for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
               gorg[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                     &(filtered[ch][0]));
               gocp[ch] = new TGraph(NSnConstants::kNsamps, xxsm,
                                     &(filtered[ch][0]));
               for (Int_t xc=0; xc<ch; ++xc) {
                  Double_t dtcor=0;
                  for (Int_t i=(ch-xc); i>0; --i) {
                     dtcor += dtCorrs[ch-i];
                  }
                  const TVector3& posCh = getStnPos(ch);
                  const TVector3& posXc = getStnPos(xc);
                  TVector3 norm;
                  norm.SetMagThetaPhi(1.0, result[0], result[1]);
                  //norm.SetMagThetaPhi(1.0, 2.95833, 5.497787);
                  const Double_t  disCh = -(posCh.Dot(norm));
                  const Double_t  disXc = -(posXc.Dot(norm));
                  // !!! check sign of delta(distance) and dtcor!
                  const Double_t  dt = kSmpRate * (
                     ( (disCh-disXc) * kNgTopFern / kC_m_ns )   // from m to ns
                     + dtcor );
                  Printf("dt[%d,%d]=%g (dis[%d]=%g, dis[%d]=%g)",
                         ch,xc,dt, ch,disCh,xc,disXc);
                  gshf[ch][xc] = new TGraph(NSnConstants::kNsamps);
                  for (Int_t i=0; i<NSnConstants::kNsamps; ++i) {
                     gshf[ch][xc]->SetPoint(i, xxsm[i]+dt, filtered[xc][i]);
                  }
                  gshf[ch][xc]->SetMarkerStyle(7);
                  gshf[ch][xc]->SetLineColor(kRed);
                  gshf[ch][xc]->SetMarkerColor(kRed);
               }
               gorg[ch]->SetMarkerStyle(7);
               gorg[ch]->SetLineColor(kBlack);
               gorg[ch]->SetMarkerColor(kBlack);
               gocp[ch]->SetMarkerStyle(7);
               gocp[ch]->SetLineStyle(7);
               gocp[ch]->SetLineColor(kGreen+2);
               gocp[ch]->SetMarkerColor(kGreen+2);
            }
            gStyle->SetOptStat(0);
            Int_t pp=0;
            for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
               for (Int_t xc=0; xc<ch; ++xc) {
                  cdbgtp->cd(++pp);
                  hn = Form("hd_%d_%d",ch,xc);
                  TH2F* hd = new TH2F(hn.Data(),
                                      Form("black=ch%d, red/green=ch%d;"
                                           "sample;ADC",ch,xc),
                                      NSnConstants::kNsamps, -0.5,
                                      NSnConstants::kNsamps-0.5,
                                      200, -900, 900);
                  hd->SetBit(TH1::kCanDelete);
                  hd->Draw();
                  gorg[ch]->Draw("pc");
                  gocp[xc]->Draw("pc");
                  gshf[ch][xc]->Draw("pc");
               }
            }
            cdbgtp->cd();
            cdbgtp->Update();
            cdbgtp->WaitPrimitive();
            for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
               delete gorg[ch]; delete gocp[ch];
               for (Int_t xc=0; xc<ch; ++xc) {
                  delete gshf[ch][xc];
               }
            }
            delete cdbgtp;
#endif

         }

         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            for (Int_t xc=0; xc<ch; ++xc) {
               delete gspl[ch][xc];
               delete gcor[ch][xc];
               delete gchi[ch][xc];
               delete gspc[ch][xc];
            }
         }

      }
   } // event loop

   outf->Write();

}

