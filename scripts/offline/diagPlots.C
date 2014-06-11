#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TError.h>
#include <TObjArray.h>
#include <TGraphErrors.h>
#include <TVirtualFFT.h>
#include <TTimeStamp.h>
#include <TParameter.h>
#include <TPython.h>
#include <TCanvas.h>

#include <NSnConstants.h>
#include <TSnDAQConfig.h>
#include <TSnSpectral.h>
#endif

// for stop location
#include "stopAlgo.C"

//#define DEBUG
//#define REMOVE_STOP

enum {
   kRunInfo=0, kDAQConfig=1, kNetAdrSet=2
};

const Float_t kSecPerDay   = 3600. * 24.;

const Int_t kTrgWordIdx    = 10;
const Int_t kRunIdxHi      = 30;
const Int_t kRunIdxLo      = 31;
const Int_t kSeqIdx        = 35;

const Float_t kSmpRate   = 1.92; // GHz
const Float_t kDelta     = 1.0 / kSmpRate;
const Int_t   kHalfsamps = NSnConstants::kNsamps/2;
const Float_t kHalfsampF = static_cast<Float_t>(NSnConstants::kNsamps)/2.0;
const Float_t kfincr     = 1.0 / (kDelta*NSnConstants::kNsamps);

TFile* outf=0, * fpnf=0;
TChain* nt, * vt;
TGraphErrors* gPed[NSnConstants::kNchans]={0},
            * gNoise[NSnConstants::kNchans]={0};

const Int_t kNextra = 57;

const Int_t dtbins  = 280,
            //tbins   = 100,
            vbins   = 150,
            cbins   = 150,
            adcbins = 1001,
            thrbins = 4096,
            rbins   = 100,
            frqbins = kHalfsamps+1,
            fqrbins = 100;
const Float_t dtmin=0, dtmax=140,
              vmin=10, vmax=25,
              cmin=0.5, cmax=1.0,
              adcmin=-1001, adcmax=1001,
              thrmin=-0.5, thrmax=4095.5,
              frqmin=-0.5*kfincr, frqmax=(kHalfsamps+0.5)*kfincr,
              fqrmin=0, fqrmax=1.0,
              rmin=0, rmax=100;

const Float_t vfullcut = 17.0;
const Float_t vchrgcut = 13.2;
const Float_t vdeplcut = 8;

// output hists
TH1F* hAdcFrcAllSmp[NSnConstants::kNchans],
    * hAdcFrcSelSmp[NSnConstants::kNchans],
    * hAdcThmAllSmp[NSnConstants::kNchans],
    * hAdcThmSelSmp[NSnConstants::kNchans],
    * hDelTFrc,
    * hDelTThm,
    * hDelTAll,
    * hNThmVsT, * hNFrcVsT, * hNPwrVsT,
    * hBatOffVsT, * hBatDepVsT, * hBatChgVsT, * hBatFulVsT;
TH2F* hSigVsTFrcAllSmp[NSnConstants::kNchans],
    * hSigVsTFrcSelSmp[NSnConstants::kNchans],
    * hSigVsTThmAllSmp[NSnConstants::kNchans],
    * hSigVsTThmSelSmp[NSnConstants::kNchans],
    * hDelTVsTFrc,
    * hDelTVsTThm,
    * hDelTVsTAll,
    * hV1vsT,
    * hCurVsT,
    * hRunVsT,
    * hHiThrshVsT[NSnConstants::kNchans],
    * hFrqThmVsT[NSnConstants::kNchans],
    * hFrqFrcVsT[NSnConstants::kNchans],
    * hMaxFrqThmVsT[NSnConstants::kNchans],
    * hMaxFrqFrcVsT[NSnConstants::kNchans],
    * hLowPwrThmVsT[NSnConstants::kNchans],
    * hLowPwrFrcVsT[NSnConstants::kNchans];
   

void getNiceDateAxisFor(const Float_t start,
                        const Float_t fin,
                        Int_t& tbins,
                        Float_t& tmin, Float_t& tmax) {
   // get a nice time axis for the specified range
   const TDatime tslo(start);
   const TDatime tshi(fin+(0.08*(fin-start)));
   const TDatime dmin(tslo.GetYear(), tslo.GetMonth(), tslo.GetDay(), 0,0,0);
   const TDatime dmax(tshi.GetYear(), tshi.GetMonth(), tshi.GetDay(), 0,0,0);
   tmin = dmin.Convert();
   tmax = dmax.Convert() + kSecPerDay;
   tbins = TMath::CeilNint((tmax-tmin)/kSecPerDay);
}

Bool_t isSmpBad(const Int_t ch, const Int_t smp) {
   switch (ch) {
      case 0:
         // 41, 91
         return (smp==41) || (smp==63) || (smp==91) || (smp==127);
      case 1:
         // 100
         return (smp==44) || (smp==63) || (smp==100) || (smp==127);
      case 2:
         // 39, 76
         return (smp==39) || (smp==55) || (smp==76) || (smp==127);
      case 3:
         // 63, 96, 116
         return (smp==63) || (smp==96) || (smp==116) || (smp==127);
      default:
         Fatal("isSmpGood","Unknown channel %d",ch);
         return kFALSE;
   };
}

Int_t getRun(const UShort_t* extra) {
   return ( (extra[31]<<16) + extra[30] );
}

void diagPlots(const Char_t* rtfn,
               const Char_t* FPNfn,
               const Char_t* outfn,
                     Int_t   tbins=0,           // if 0, use daily bins
               const Float_t freqcut=0.200,
               const UInt_t  lotime=1349049600, // Oct 1, 2012
               const UInt_t  hitime=1380585600, // Oct 1, 2013
               const Float_t vgain = 6.0649e-4,
               const Float_t cped  = 1.112,
               const Float_t cgain = -2.6828e-5) {

   const Bool_t doTimeCut = (lotime<hitime);

   nt = new TChain("runtree");
   nt->Add(rtfn);

   const Long64_t nents = nt->GetEntries();
   if (nents==0) {
      Error("diagPlots","No events in tree from [%s].",rtfn);
      return;
   }

   vt = new TChain("volttree");
   vt->Add(rtfn);
   const Long64_t vents = vt->GetEntries();
   if (vents==0) {
      Error("diagPlots","No events in volt tree from [%s].",rtfn);
      return;
   }

   fpnf = TFile::Open(FPNfn);
   if ( (fpnf==0) || (fpnf->IsZombie()) ) {
      Error("diagPlots","Could not open FPN file [%s]",FPNfn);
      return;
   }
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      gPed[ch]   = dynamic_cast<TGraphErrors*>(
         fpnf->Get(Form("gExlPed_ch%d",ch)));
      gNoise[ch] = dynamic_cast<TGraphErrors*>(
         fpnf->Get(Form("gExlRms_ch%d",ch)));
      if ( (gPed[ch]==0) || (gNoise[ch]==0) ) {
         Error("diagPlots","Couldn't get FPN/noise graphs from [%s]",FPNfn);
         return;
      }
   }

   // read from tree into...
   Float_t  freqs[NSnConstants::kNchans][NSnConstants::kNsamps];
   Float_t  pedsubs[NSnConstants::kNchans][NSnConstants::kNsamps];
   UShort_t samples[NSnConstants::kNchans][NSnConstants::kNsamps];
   UShort_t extra[kNextra];
   UInt_t   evnum, utime, utimeus, mbchksum;
   Int_t    dtms;
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("EvtNum",&evnum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("DTms",&dtms);
   nt->SetBranchAddress("extra",&(extra[0]));
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));
   }
   
   // the voltage reading tree
   Float_t av1, av2, rm1, rm2;
   UInt_t vtime;
   vt->SetBranchAddress("aveV1",&av1);
   vt->SetBranchAddress("aveV2",&av2);
   vt->SetBranchAddress("rmsV1",&rm1);
   vt->SetBranchAddress("rmsV2",&rm2);
   vt->SetBranchAddress("vtime",&vtime);

   TObjArray* rsc=0;
   TFile* cfile = nt->GetFile();
   TFile* ofile = cfile;
   cfile->GetObject("RunSeqConfs",rsc);
   
   Printf("Finding time range...");

   Bool_t first=kTRUE, ok;
   UInt_t mintime(0), maxtime(0);
   for (Long64_t ev=0; ev<nents; ++ev) {
      if ( (ev%10000)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            ev, nents,
            100.*static_cast<Float_t>(ev)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(ev);

      ok = (doTimeCut) ? ( (utime>lotime) && (utime<hitime) )
         : kTRUE;
      
      if (ok) {
         if (first) {
            mintime = utime;
            maxtime = utime;
            first   = kFALSE;
         } else {
            if (utime<mintime) {
               mintime = utime;
            }
            if (utime>maxtime) {
               maxtime = utime;
            }
         }
      }
   }
   
   // convert to daily bins
   Float_t tmin(mintime), tmax(maxtime);
   if (tbins==0) {
      getNiceDateAxisFor(mintime, maxtime, tbins, tmin, tmax);
      mintime = tmin;
      maxtime = tmax;
      Printf("Using daily bins (%d total bins)...",tbins);
   }

   // output
   TString hn;
   outf = TFile::Open(outfn,"recreate");
   outf->cd();
   hNThmVsT = new TH1F("hNThmVsT","num thermal trg events vs time;"
                      "date, time (UTC);num evts",
                      tbins, tmin, tmax);
   hNFrcVsT = new TH1F("hNFrcVsT","num minbias events vs time;"
                      "date, time (UTC);num evts",
                      tbins, tmin, tmax);
   hNPwrVsT = new TH1F("hNPwrVsT","num voltage readings vs time;"
                       "date, time (UTC);num voltage readings",
                       tbins, tmin, tmax);
   hBatOffVsT = new TH1F("hBatOffVsT","fraction of time battery off;"
                         "date, time (UTC);fraction of time",
                         tbins, tmin, tmax);
   hBatDepVsT = new TH1F("hBatDepVsT","fraction of time battery depleting;"
                         "date, time (UTC);fraction of time",
                         tbins, tmin, tmax);
   hBatChgVsT = new TH1F("hBatChgVsT","fraction of time battery charging;"
                         "date, time (UTC);fraction of time",
                         tbins, tmin, tmax);
   hBatFulVsT = new TH1F("hBatFulVsT","fraction of time battery full;"
                         "date, time (UTC);fraction of time",
                         tbins, tmin, tmax);
   hDelTFrc = new TH1F("hDelTFrc",Form("#Deltat consecutive events (minbias)"
                          ";#Deltat (s)"),
                       dtbins, dtmin, dtmax);
   hDelTFrc->Sumw2();
   hDelTThm = new TH1F("hDelTThm",Form("#Deltat consecutive events (thermal trg)"
                          ";#Deltat (s)"),
                       dtbins, dtmin, dtmax);
   hDelTThm->Sumw2();
   hDelTAll = new TH1F("hDelTAll",Form("#Deltat consecutive events (all)"
                          ";#Deltat (s)"),
                       dtbins, dtmin, dtmax);
   hDelTAll->Sumw2();
   hDelTVsTFrc = new TH2F("hDelTVsTFrc",
                          Form("#Deltat consecutive events vs time (minbias)"
                          ";date, time (UTC);#Deltat (s)"),
                          tbins, tmin, tmax,
                          dtbins, dtmin, dtmax);
   hDelTVsTFrc->Sumw2();
   hDelTVsTThm = new TH2F("hDelTVsTThm",
                          Form("#Deltat consecutive events vs time (thermal trg)"
                          ";date, time (UTC);#Deltat (s)"),
                          tbins, tmin, tmax,
                          dtbins, dtmin, dtmax);
   hDelTVsTThm->Sumw2();
   hDelTVsTAll = new TH2F("hDelTVsTAll",
                          Form("#Deltat consecutive events vs time (all)"
                          ";date, time (UTC);#Deltat (s)"),
                          tbins, tmin, tmax,
                          dtbins, dtmin, dtmax);
   hDelTVsTAll->Sumw2();
   
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      hn = Form("hAdcFrcAllSmp_ch%d",ch);
      hAdcFrcAllSmp[ch] = new TH1F(hn.Data(),
                                   Form("ADC distribution (chan %d, minbias, "
                                        "all samples);ADC-pedestal", ch),
                                   adcbins, adcmin, adcmax);
      hn = Form("hAdcFrcSelSmp_ch%d",ch);
      hAdcFrcSelSmp[ch] = new TH1F(hn.Data(),
                                   Form("ADC distribution (chan %d, minbias, "
                                        "good samples);ADC-pedestal", ch),
                                   adcbins, adcmin, adcmax);
      hn = Form("hAdcThmAllSmp_ch%d",ch);
      hAdcThmAllSmp[ch] = new TH1F(hn.Data(),
                                   Form("ADC distribution (chan %d, thermal trg, "
                                        "all samples);ADC-pedestal", ch),
                                   adcbins, adcmin, adcmax);
      hn = Form("hAdcThmSelSmp_ch%d",ch);
      hAdcThmSelSmp[ch] = new TH1F(hn.Data(),
                                   Form("ADC distribution (chan %d, thermal trg, "
                                        "good samples);ADC-pedestal", ch),
                                   adcbins, adcmin, adcmax);


      hn = Form("hSigVsTFrcAllSmp_ch%d",ch);
      hSigVsTFrcAllSmp[ch] = new TH2F(hn.Data(),
                                   Form("ADC dist vs time (chan %d, minbias, "
                                        "all samples)"
                                        ";date, time (UTC);ADC-pedestal", ch),
                                   tbins, tmin, tmax,
                                   adcbins, adcmin, adcmax);
      hn = Form("hSigVsTFrcSelSmp_ch%d",ch);
      hSigVsTFrcSelSmp[ch] = new TH2F(hn.Data(),
                                   Form("ADC dist vs time (chan %d, minbias, "
                                        "good samples)"
                                        ";date, time (UTC);ADC-pedestal", ch),
                                   tbins, tmin, tmax,
                                   adcbins, adcmin, adcmax);
      hn = Form("hSigVsTThmAllSmp_ch%d",ch);
      hSigVsTThmAllSmp[ch] = new TH2F(hn.Data(),
                                   Form("ADC dist vs time (chan %d, thermal trg, "
                                        "all samples)"
                                        ";date, time (UTC);ADC-pedestal", ch),
                                   tbins, tmin, tmax,
                                   adcbins, adcmin, adcmax);
      hn = Form("hSigVsTThmSelSmp_ch%d",ch);
      hSigVsTThmSelSmp[ch] = new TH2F(hn.Data(),
                                   Form("ADC dist vs time (chan %d, thermal trg, "
                                        "good samples)"
                                        ";date, time (UTC);ADC-pedestal", ch),
                                   tbins, tmin, tmax,
                                   adcbins, adcmin, adcmax);
      hn = Form("hHiTrshVsT_ch%d",ch);
      hHiThrshVsT[ch] = new TH2F(hn.Data(),
                                 Form("high threshold vs time (chan %d)"
                                      ";date, time (UTC);hi thresh (DAC)",ch),
                                 tbins, tmin, tmax,
                                 thrbins, thrmin, thrmax);
      hn = Form("hFrqThmVsT_ch%d",ch);
      hFrqThmVsT[ch] = new TH2F(hn.Data(),
                                Form("freq spectrum vs time "
                                     "(chan %d, thermal trg)"
                                     ";date, time (UTC);freqency (GHz)",ch),
                                tbins, tmin, tmax,
                                frqbins, frqmin, frqmax);
      hn = Form("hFrqFrcVsT_ch%d",ch);
      hFrqFrcVsT[ch] = new TH2F(hn.Data(),
                                Form("freq spectrum vs time "
                                     "(chan %d, minbias)"
                                     ";date, time (UTC);freqency (GHz)",ch),
                                tbins, tmin, tmax,
                                frqbins, frqmin, frqmax);
      hn = Form("hMaxFrqThmVsT_ch%d",ch);
      hMaxFrqThmVsT[ch] = new TH2F(hn.Data(),
                                   Form("max freq vs time (chan %d, thermal trg)"
                                        ";date, time (UTC);max freqency (GHz)",
                                        ch),
                                   tbins, tmin, tmax,
                                   frqbins, frqmin, frqmax);
      hn = Form("hMaxFrqFrcVsT_ch%d",ch);
      hMaxFrqFrcVsT[ch] = new TH2F(hn.Data(),
                                   Form("max freq vs time (chan %d, minbias)"
                                        ";date, time (UTC);max freqency (GHz)",
                                        ch),
                                   tbins, tmin, tmax,
                                   frqbins, frqmin, frqmax);
      hn = Form("hLowPwrThmVsT_ch%d",ch);
      hLowPwrThmVsT[ch] = new TH2F(hn.Data(),
                                   Form("fraction of power below %g GHz vs time "
                                        "(chan %d, thermal trg)"
                                        ";date, time (UTC);fraction power < "
                                        "%g GHz",freqcut,ch,freqcut),
                                   tbins, tmin, tmax,
                                   fqrbins, fqrmin, fqrmax);
      hn = Form("hLowPwrFrcVsT_ch%d",ch);
      hLowPwrFrcVsT[ch] = new TH2F(hn.Data(),
                                   Form("fraction of power below %g GHz vs time "
                                        "(chan %d, minbias)"
                                        ";date, time (UTC);fraction power < "
                                        "%g GHz",freqcut,ch,freqcut),
                                   tbins, tmin, tmax,
                                   fqrbins, fqrmin, fqrmax);
   }

   hRunVsT = new TH2F("hRunVsT","run number vs time;"
                      "date, time (UTC);run",
                      tbins, tmin, tmax,
                      rbins, rmin, rmax);
   hV1vsT = new TH2F("hV1vsT","input voltage vs time;"
                     "date, time (UTC);voltage (V)",
                     tbins, tmin, tmax,
                     vbins, vmin, vmax);
   hCurVsT = new TH2F("hCurVsT","current vs time;date, time (UTC);current (A)",
                      tbins, tmin, tmax,
                      cbins, cmin, cmax);

   const Int_t fcbin = hFrqThmVsT[0]->GetYaxis()->FindFixBin(freqcut) - 1;
   if ( (fcbin<1) || (fcbin>frqbins) ) {
      Error("diagPlots","Could not find freq bin for cut %g. Got %d.",
            freqcut, fcbin);
      return;
   }
   
   Printf("\nFilling volt histograms...");
   for (Long64_t ev=0; ev<vents; ++ev) {
      if ( (ev%10000)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            ev, vents,
            100.*static_cast<Float_t>(ev)/static_cast<Float_t>(vents));
      }
      vt->GetEntry(ev);
      
      ok = (doTimeCut) ? ( (vtime>lotime) && (vtime<hitime) )
         : kTRUE;
      
      if (ok) {
         hNPwrVsT->Fill(vtime);
         const Float_t v = av1 * vgain;
         hV1vsT->Fill(vtime, v);
         hCurVsT->Fill(vtime, cped + ( (av2-av1)*cgain ));
         if (v>vfullcut) {
            hBatFulVsT->Fill(vtime);
         } else if (v>vchrgcut) {
            hBatChgVsT->Fill(vtime);
         } else if (v>vdeplcut) {
            hBatDepVsT->Fill(vtime);
         } else {
            hBatOffVsT->Fill(vtime);
         }
      }

   }
   
   hBatOffVsT->Divide(hNPwrVsT);
   hBatDepVsT->Divide(hNPwrVsT);
   hBatChgVsT->Divide(hNPwrVsT);
   hBatFulVsT->Divide(hNPwrVsT);
   
   Printf("\nFilling data histograms...");

#ifdef REMOVE_STOP
   Double_t dumNoise[NSnConstants::kNsamps];
   for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
      dumNoise[sm]=1.5;
   }
#endif
   Int_t lrun(-1), lseq(-1), crun;
   Double_t lthmtime(-1), lfrctime(-1), ctime, ltime(-1);
   for (Long64_t ev=0; ev<nents; ++ev) {
      if ( (ev%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            ev, nents,
            100.*static_cast<Float_t>(ev)/static_cast<Float_t>(nents));
      }
      nt->GetEntry(ev);
      
      ok = (doTimeCut) ? ( (utime>lotime) && (utime<hitime) )
         : kTRUE;
      
      if (ok) {
         const Bool_t isThm = (extra[kTrgWordIdx] & 1)!=0; // thermal
         const Bool_t isFrc = (extra[kTrgWordIdx] & 2)!=0; // forced

         const Int_t run = extra[kRunIdxHi] + (extra[kRunIdxLo]<<16);
         const Int_t seq = extra[kSeqIdx];
         hRunVsT->Fill(utime, run);

         if (isThm) {
            hNThmVsT->Fill(utime);
         }
         if (isFrc) {
            hNFrcVsT->Fill(utime);
         }
         
         cfile = nt->GetFile();
         if (cfile!=ofile) {
            cfile->GetObject("RunSeqConfs",rsc);
            ofile = cfile;
         }
         if (rsc!=0) {
            TObjArray* seqconfs = dynamic_cast<TObjArray*>(rsc->At(run));
            if (seqconfs!=0) {
               TObjArray* rsconfs = dynamic_cast<TObjArray*>(seqconfs->At(seq));
               if (rsconfs!=0) {
                  TSnDAQConfig* dqc = 
                     dynamic_cast<TSnDAQConfig*>(rsconfs->At(kDAQConfig));
                  if (dqc!=0) {
                     for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
                        hHiThrshVsT[ch]->Fill(utime, dqc->GetDac(ch, 0));
                     }
                  }
               }
            }
         }
         
         // delta t's
         const Double_t dts = dtms/1000.0;
         hDelTAll->Fill(dts);
         hDelTVsTAll->Fill(utime, dts);
         // try to re-construct the delta t's between events of specific triggers
         crun = getRun(extra);
         if ( (lrun==crun) && (lseq==extra[35]) ) {
            if (ltime>0) { // <0 if first of new run/seq
               ctime = ltime + dts;
            } else {
               ctime = static_cast<Double_t>(utime); // can't get this right
            }
            if (isThm) {
               if (lthmtime>0) { // <0 if first of new run/seq
                  const Double_t dt = ctime - lthmtime;
                  hDelTThm->Fill(dt);
                  hDelTVsTThm->Fill(utime, dt);
               }
               lthmtime = ctime;
            } else if (isFrc) {
               if (lfrctime>0) { // <0 if first of new run/seq
                  const Double_t dt = ctime - lfrctime;
                  hDelTFrc->Fill(dt);
                  hDelTVsTFrc->Fill(utime, dt);
               }
               lfrctime = ctime;
            }
            ltime = ctime;
         } else {
            // reset if run/seq changed
            lthmtime = lfrctime = ltime = -1;
         }
         lrun = crun;
         lseq = extra[35];
      
#ifdef REMOVE_STOP
         StopList_t stops[NSnConstants::kNchans],
            bestStops(NSnConstants::kNchans);
#endif
         // adc dists
         for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
            const Double_t* const peds = gPed[ch]->GetY();
            // subtract FPN
            const Double_t* pd = peds;
            const UShort_t* sp = &(samples[ch][0]);
            Float_t*        ps = &(pedsubs[ch][0]);
            
            for (Int_t sm=0; sm<NSnConstants::kNsamps;
                 ++sm, ++pd, ++ps, ++sp) {
               *ps = *sp - *pd;
#ifdef REMOVE_STOP
            }            

            ps = &(pedsubs[ch][0]);
            for (Int_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++ps) {
#endif
               const Bool_t isgood = (isSmpBad(ch,sm)==kFALSE);
            
               if (isThm) { // thermal
                  hAdcThmAllSmp[ch]->Fill(*ps);
                  hSigVsTThmAllSmp[ch]->Fill(utime, *ps);
                  if (isgood) {
                     hAdcThmSelSmp[ch]->Fill(*ps);
                     hSigVsTThmSelSmp[ch]->Fill(utime, *ps);
                  }
               } else if (isFrc) { // forced
#ifdef REMOVE_STOP
                  // find stop
                  getFPNStopLocs( &(pedsubs[ch][0]),
                                  dumNoise,//gNoise[ch]->GetY(),
                                  stops[ch],
                                  6, 2., 5., 1,
                                  NSnConstants::kNsamps );
                  bestStops[ch] = (stops[ch].empty()) ? LocLen_t(0,0) :
                     getMostOutliersStop(stops[ch],
                                         &(pedsubs[ch][0]),
                                         dumNoise,//gNoise[ch]->GetY(),
                                         2., 1,
                                         NSnConstants::kNsamps);
                  if ( isInStop(sm, bestStops.at(ch), NSnConstants::kNsamps)
                       ==kFALSE ) {
#endif
                     hAdcFrcAllSmp[ch]->Fill(*ps);
                     hSigVsTFrcAllSmp[ch]->Fill(utime, *ps);
                     if (isgood) {
                        hAdcFrcSelSmp[ch]->Fill(*ps);
                        hSigVsTFrcSelSmp[ch]->Fill(utime, *ps);
                     }
#ifdef REMOVE_STOP
                  }
#endif
               }
            } // sm
            
            memcpy(&(freqs[ch][0]), &(pedsubs[ch][0]), 
                   sizeof(Float_t)*NSnConstants::kNsamps);
            TSnSpectral::RealFFT(&(freqs[ch][0]), NSnConstants::kNsamps);
            Float_t lowpThm=0, lowpFrc=0, totpThm=0, totpFrc=0,
               fmaxThm=0, fmaxFrc=0, pmaxThm=0, pmaxFrc=0;
            if (isThm) {
               totpThm += freqs[ch][0];
               totpThm += freqs[ch][1];
               if (fcbin>0) {
                  lowpThm += freqs[ch][0];
               }
               if (fcbin>frqbins-2) {
                  lowpThm += freqs[ch][1];
               }
               
               if (freqs[ch][0]>freqs[ch][1]) {
                  pmaxThm = freqs[ch][0];
                  fmaxThm = hMaxFrqThmVsT[ch]->GetYaxis()->GetBinCenter(1);
               } else {
                  pmaxThm = freqs[ch][1];
                  fmaxThm = hMaxFrqThmVsT[ch]->GetYaxis()->GetBinCenter(2);
               }
               
               hFrqThmVsT[ch]->Fill(utime,
                                    hFrqThmVsT[ch]->GetYaxis()->GetBinCenter(1),
                                    TMath::Abs(freqs[ch][0]));
               hFrqThmVsT[ch]->Fill(utime,
                                    hFrqThmVsT[ch]->GetYaxis()->GetBinCenter(
                                       frqbins),
                                    TMath::Abs(freqs[ch][1]));
            }
            if (isFrc) {
               totpFrc += freqs[ch][0];
               totpFrc += freqs[ch][1];
               if (fcbin>0) {
                  lowpFrc += freqs[ch][0];
               }
               if (fcbin>frqbins-2) {
                  lowpFrc += freqs[ch][1];
               }

               if (freqs[ch][0]>freqs[ch][1]) {
                  pmaxFrc = freqs[ch][0];
                  fmaxFrc = hMaxFrqFrcVsT[ch]->GetYaxis()->GetBinCenter(1);
               } else {
                  pmaxFrc = freqs[ch][1];
                  fmaxFrc = hMaxFrqFrcVsT[ch]->GetYaxis()->GetBinCenter(2);
               }

               hFrqFrcVsT[ch]->Fill(utime,
                                    hFrqFrcVsT[ch]->GetYaxis()->GetBinCenter(1),
                                    TMath::Abs(freqs[ch][0]));
               hFrqFrcVsT[ch]->Fill(utime,
                                    hFrqFrcVsT[ch]->GetYaxis()->GetBinCenter(
                                       frqbins),
                                    TMath::Abs(freqs[ch][1]));
            }
            Float_t* dfr = &(freqs[ch][2]);
            Float_t* dfi = &(freqs[ch][3]);
            Float_t pwr=0;
            for (Int_t i=2, j=2; i<NSnConstants::kNsamps;
                 i+=2, dfr+=2, dfi+=2, ++j) {
               pwr = TMath::Sqrt( ((*dfr)*(*dfr)) + ((*dfi)*(*dfi)) );
               if (isThm) {
                  totpThm += pwr;
                  if (fcbin>=j) {
                     lowpThm += pwr;
                  }
                  hFrqThmVsT[ch]->Fill(utime,
                                       hFrqThmVsT[ch]->GetYaxis()->
                                          GetBinCenter(j),
                                       pwr);
                  if (pwr > pmaxThm) {
                     pmaxThm = pwr;
                     fmaxThm = hMaxFrqFrcVsT[ch]->GetYaxis()->GetBinCenter(j);
                  }
               }
               if (isFrc) {
                  totpFrc += pwr;
                  if (fcbin>=j) {
                     lowpFrc += pwr;
                  }
                  hFrqFrcVsT[ch]->Fill(utime,
                                       hFrqFrcVsT[ch]->GetYaxis()->
                                          GetBinCenter(j),
                                       pwr);
                  if (pwr > pmaxFrc) {
                     pmaxFrc = pwr;
                     fmaxFrc = hMaxFrqFrcVsT[ch]->GetYaxis()->GetBinCenter(j);
                  }
               }
            }
            
            if (isThm) {
               hMaxFrqThmVsT[ch]->Fill(utime, fmaxThm, pmaxThm);
               if (totpThm>0) {
                  hLowPwrThmVsT[ch]->Fill(utime, lowpThm/totpThm);
               }
            }
            if (isFrc) {
               hMaxFrqFrcVsT[ch]->Fill(utime, fmaxFrc, pmaxFrc);
               if (totpFrc>0) {
                  hLowPwrFrcVsT[ch]->Fill(utime, lowpFrc/totpFrc);
               }
            }
            
         } // ch
      } // ok
   } // ev


   outf->Write();
   
}

