#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TRandom3.h>
#include <TMath.h>
#include <TGraphErrors.h>
#include <TError.h>
#include <TParameter.h>
#include <TVector3.h>

#include <NSnConstants.h>
#endif

static const Float_t kSmpRate   = 1.92; // GH

static const Double_t kNgTopFern   = 1.33;
static const Double_t kC_m_ns      = TMath::C() / 1.0e9;

const Int_t kNstops = 8;
const Int_t stopBins[kNstops] = {
   13, 29, 45, 61,
   77, 93, 109, 125
};

TFile* outf, * pedf;
TTree* runtree;
TGraphErrors* gPed[NSnConstants::kNchans],
            * gNoise[NSnConstants::kNchans];

const TVector3& getStnPos(const Int_t ch) {
   //
   //          0             y
   //          | 6m          |
   //     3----+----1        +--- x
   //          |            /
   //          2           z
   //
   // y-axis positive = north
   // x-axis positive = east
   //
   // chan 0 points roughly north (to Mt Discovery)
   // so for now, set stations with 0 along +y axis
   
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


Double_t moreletWave(const Double_t x,          // ns
                     const Double_t norm,       // amplitude (a.u.)
                     const Double_t width,      // ns
                     const Double_t freq,       // GHz
                     const Double_t midoff=0    // offset from center sample (ns)
                     ) {
   static const Double_t middle=NSnConstants::kNsamps/(2.0*kSmpRate);
   const Double_t xo = x - (middle + midoff);
   Double_t y  = norm * TMath::Exp(-(0.5/width)*xo*xo);
   y *= TMath::Sin(freq*xo);
   return y;
}

Double_t moreletWaveFcn(Double_t* x, Double_t* p) {
   return moreletWave(x[0], p[0], p[1], p[2], p[3]);
}

Double_t squarePeak(const Double_t x,          // ns
                    const Double_t norm,       // amplitude (a.u.)
                    const Double_t width,      // ns
                    const Double_t midoff=0) { // offset from center sample (ns)
   static const Double_t middle=NSnConstants::kNsamps/(2.0*kSmpRate);
   const Double_t md = middle + midoff;
   const Double_t hw = width / 2.0;
   if ( TMath::Abs(x-md) < hw ) {
      return norm;
   }  else {
      return 0;
   }
}

Double_t squarePeakFcn(Double_t* x, Double_t* p) {
   return squarePeak(x[0], p[0], p[1], p[2]);
}

void makeFake(const Char_t* outfn,
              const Char_t* pedfn,
              const UInt_t  offsetSeed,  // ==0 => no random offsets
              const UInt_t  sourceSeed,  // ==0 => no random source
              const Int_t   wtype,       // 0 => morelet, 1 => square
              const Float_t dur=44,      // ns
              const Float_t norm=400,    // ADC
              const Float_t freq=0.220,  // GHz
              const Long64_t nevts=2000,
              const Float_t minTheta=0.7
              ) {
   
   outf = TFile::Open(outfn,"recreate");

   // load pedestals
   pedf = TFile::Open(pedfn);
   if ( (pedf==0) || (pedf->IsZombie()) ) {
      Error("makeFake","Could not open FPN file [%s]",pedfn);
      return;
   }
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      gPed[ch]   = dynamic_cast<TGraphErrors*>(
         pedf->Get(Form("gExlPed_ch%d",ch)));
      gNoise[ch] = dynamic_cast<TGraphErrors*>(
         pedf->Get(Form("gExlRms_ch%d",ch)));
      if ( (gPed[ch]==0) || (gNoise[ch]==0) ) {
         Error("makeFake",
               "Couldn't get FPN/noise graphs from [%s]",pedfn);
         return;
      }
   }

   
   UInt_t fStationCRC(0);
   UInt_t eventNumber(0);
   UInt_t unixTime(0);
   UInt_t unixTimeUs(0);
   UShort_t data[NSnConstants::kNchans][NSnConstants::kNsamps];
   UShort_t extra[57];
   Int_t dtms(0);
   memset(extra,0,57*sizeof(UShort_t));
   memset(data,0,NSnConstants::kNchans*NSnConstants::kNsamps*sizeof(UShort_t));
   runtree = new TTree("runtree","runtree");
   runtree->SetDirectory(outf);
   runtree->Branch("mbChecksum", &fStationCRC, "mbChecksum/i"); 
   runtree->Branch("HdEvtNum",&eventNumber,"HdEvtNum/i");
   runtree->Branch("EvtNum",&eventNumber,"EvEvtNum/i");
   runtree->Branch("unixTime",&unixTime,"unixTime/i");
   runtree->Branch("unixTimeUS",&unixTimeUs,"unixTimeUS/i");
   runtree->Branch("DTms",&dtms,"DTms/I");
   TString hd;
   for (UShort_t j=0; j<NSnConstants::kNchans; ++j) {
      hd = Form("data%02hu",j);
      runtree->Branch(hd.Data(),&(data[j][0]),
		 Form("%s[%hu]/s",hd.Data(),NSnConstants::kNsamps));
   }
   runtree->Branch("extra",&(extra[0]),"extra[57]/s");
   runtree->SetDirectory(outf);
   
   TRandom3 rndDt(offsetSeed), rndSc(sourceSeed), rndStop(nevts);
   TVector3 src;
   if (sourceSeed!=0) {
      src.SetMagThetaPhi(1.0,
                         rndSc.Uniform(TMath::Pi()*minTheta, TMath::Pi()),
                         rndSc.Rndm()*TMath::TwoPi());
      outf->WriteObject(&src, "source");
   }
   Double_t midoffs[NSnConstants::kNchans];
   Double_t* mo = &(midoffs[0]);
   for (UShort_t ch=0; ch<NSnConstants::kNchans; ++ch, ++mo) {
      *mo = (offsetSeed!=0) ? ((2.0*rndDt.Rndm())-1.0) : 0;
      outf->cd();
      TParameter<Double_t> mp(Form("delayCh%d",ch), *mo);
      mp.Write();
      if (sourceSeed!=0) {
         const TVector3& posCh = getStnPos(ch);
         const Double_t  disCh = -(posCh.Dot(src));
         // !!! check sign of delta(distance) and dtcor!
         const Double_t  dt = disCh * kNgTopFern / kC_m_ns; // from m to ns
         *mo += dt;
         TParameter<Double_t> dp(Form("totalDelayCh%d",ch), *mo);
         dp.Write();
      }
   }

   for (Long64_t i=0; i<nevts; ++i) {
      if ( (i%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            i, nevts,
            100.*static_cast<Float_t>(i)/static_cast<Float_t>(nevts));
      }
      eventNumber = static_cast<UInt_t>(i);
      // same stop position for each channel
      const Int_t start = stopBins[rndStop.Integer(kNstops+1)] - 6;
      mo = &(midoffs[0]);
      for (UShort_t ch=0; ch<NSnConstants::kNchans; ++ch, ++mo) {
         Int_t bin(start);
         for (UShort_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++bin) {
            while (bin>=NSnConstants::kNsamps) {
               bin -= NSnConstants::kNsamps;
            }
            if (wtype==0) {
               data[ch][bin] = static_cast<UShort_t>(
                  moreletWave(sm/kSmpRate, norm, 2.0*dur, 
                              TMath::TwoPi()*freq,
                              *mo));
            } else if (wtype==1) {
               data[ch][bin] = static_cast<UShort_t>(
                  squarePeak(sm/kSmpRate, norm, dur, *mo));
            } else {
               Fatal("makeFake","Unknown wtype [%d].",wtype);
            }
            data[ch][bin] += (gPed[ch]->GetY())[bin];
         }
      }
      runtree->Fill();
   }
   outf->Write();
   Printf("\nDone");
}
