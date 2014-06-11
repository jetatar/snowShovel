#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TObjArray.h>
#include <TGraphErrors.h>
#include <TVirtualFFT.h>
#include <TTimeStamp.h>
#include <TParameter.h>
#include <TPython.h>
#endif

// for FFT
#include "lombtest.C"

// for stop location
#include "stopAlgo.C"

/*
// 2010 in situ
static const Int_t   kNchans    = 9;
static const Int_t   kNsamps    = 256;
static const Int_t   kNextra    = 57;
static const Int_t   kTrgBitIdx = 10;
static const Float_t kSmpRate   = 2.86; // GHz
static const Char_t* kFFTopt    = "MAG DHT P";
*/

// 2011 station
static const Int_t   kNchans    = 4;
static const Int_t   kNsamps    = 128;
static const Int_t   kNextra    = 57;
static const Int_t   kTrgBitIdx = 10;
static const Float_t kSmpRate   = 1.92; // GHz
//static const Char_t* kFFTopt  = "MAG R2C P";

static const Float_t kDelta     = 1.0 / kSmpRate;
static const Int_t   kHalfsamps = kNsamps/2;
static const Float_t kHalfsampF = static_cast<Float_t>(kNsamps)/2.0;
static const Float_t kfincr     = 1.0 / (kDelta*kNsamps);

TChain* nt;
TFile* outf, * fpnf=0;
TObjArray* hSampVsEvent;               // [channel]
TObjArray* hPedSubSampVsEvent;         // [channel]
TObjArray* hPedSubWvfm;                // [channel][event]
TObjArray* hADCdistOfSample;           // [channel][sample]
TObjArray* gPedAndRmsVsSamp;           // [channel]
TObjArray* hStopLocs;                  // [chan]
TObjArray* hStopLocCands;              // [chan]
TObjArray* hStopLens;                  // [chan]
TObjArray* hStopLenCands;              // [chan]
TObjArray* hNumStops;                  // [chan]
TObjArray* hStopADCs;                  // [chan][sample]
TObjArray* hStopCandADCs;              // [chan][sample]
TObjArray* hAveFreqs;                  // [chan]
THashTable* hStopOffsets;
THashTable* hStopCandOffsets;

void fillStopHists(const LocLen_t& stop,
                   TH1& hloc, TH1& hlen,
                   TObjArray& hsadcs,
                   const TH1& hdata,
                   const Float_t nrm);
Int_t getOffsetMod(const Int_t s1, const Int_t s2);
void writeAndClean(TCollection* plots, TDirectory* odir,
                   const Char_t* wname=0, const Int_t woption=0);

void makeWaves(const Char_t*  infn,
   //"/home/cjreed/work/temp/nt.temp.ev_*.dat.gz.root",
               const Char_t*  outfn,
   //"/home/cjreed/work/temp/hists.temp.ev_All.dat.gz.root",
               const Long64_t firstEvt=0,
               const Bool_t   verifyChksum=kFALSE,
               const UShort_t trgForPeds=-1,
               const Int_t    stpNumChn=6,   // min num of stop samples in a row
               const Float_t  stpOffset=0.,  // absolute offset from ped of stop
               const Float_t  stpAbsWin=30., // absolute dist of stop from ped
               const Int_t    stpRelWin=25., // relative adc spread of stop
               const Int_t    fpnNumChn=6,   // num of stop samples in a row in fpn
               const Float_t  fpnNumSig=2.,  // number of sigma for outliers in fpn stop
               const Float_t  fpnNumOut=5.,  // min number of outliers in fpn stop
               const Int_t    fpnAbvBel=1,   // whether outliers should be pos, neg or either
               const Char_t*  FPNfn="/home/cjreed/work/AriannaData/"
                                    "icicleForced/pedestals.root",
               const Char_t*  configdb="/home/cjreed/work/AriannaData/"
                                       "icicle/CONFIG_INFO/icicleConf2012.dat"
               ) {
   // trgForPeds : the trigger type to use for "pedestals". -1 => all events
   //              ("pedestal" here = the average ADC of each sample)
   // FPNfn : if defined and readable, use these FPN values for the
   //         pedestal subtraction.
   // configdb : if defined and readble, use this to tell which amp is on
   
   // open the tree
   nt = new TChain("nt");
   nt->Add(infn);
   
   const Long64_t nents = nt->GetEntries() - firstEvt;
   Printf("Entries in tree = %lld",nents);
   
   // open the output file (to store histograms)
   outf = TFile::Open(outfn,"recreate");
   TH1::AddDirectory(kFALSE); // do all saving & cleaning by hand for speed
   TDirectory* tsdir   = outf->mkdir("Timestamps");
   TDirectory* endir   = outf->mkdir("DAQEvtNums");
   TDirectory* tgdir   = outf->mkdir("TrigBits");
   TDirectory* wvdir   = outf->mkdir("Waveforms");
   TDirectory* psdir   = outf->mkdir("PedSubWvfms");
   TDirectory* shdir   = outf->mkdir("ShiftedPSWvfms");
   TDirectory* smdir   = outf->mkdir("ADCdistOfSample");
   TDirectory* fqdir   = outf->mkdir("WvFrequencies");
   TDirectory* slocdir = outf->mkdir("StopLocs");
   TDirectory* slccdir = outf->mkdir("StopLocCands");
   TDirectory* slendir = outf->mkdir("StopLengths");
   TDirectory* slncdir = outf->mkdir("StopLengthCands");
   TDirectory* nstpdir = outf->mkdir("NumStops");
   TDirectory* sadcdir = outf->mkdir("StopADCs");
   TDirectory* cadcdir = outf->mkdir("StopCandADCs");
   TDirectory* soffdir = outf->mkdir("StopOffsets");
   TDirectory* coffdir = outf->mkdir("StopCandOffsets");
   hSampVsEvent        = new TObjArray(kNchans);
   hPedSubSampVsEvent  = new TObjArray(kNchans);
   hPedSubWvfm         = new TObjArray(kNchans);
   hADCdistOfSample    = new TObjArray(kNchans);
   gPedAndRmsVsSamp    = new TObjArray(kNchans);
   hStopLocs           = new TObjArray(kNchans); // [chan]
   hStopLocCands       = new TObjArray(kNchans); // [chan]
   hStopLens           = new TObjArray(kNchans); // [chan]
   hStopLenCands       = new TObjArray(kNchans); // [chan]
   hNumStops           = new TObjArray(kNchans); // [chan]
   hStopADCs           = new TObjArray(kNchans); // [chan][sample]
   hStopCandADCs       = new TObjArray(kNchans); // [chan][sample]
   hAveFreqs           = new TObjArray(kNchans); // [chan]
   hStopOffsets        = new THashTable;
   hStopCandOffsets    = new THashTable;
   
   // read from tree into...
   UShort_t samples[kNchans][kNsamps];
   UShort_t extra[kNextra];
   UInt_t   evnum, utime, utimeus, mbchksum;
   
   // setup tree, make hists
   nt->SetBranchAddress("mbChecksum",&mbchksum);
   nt->SetBranchAddress("EvtNum",&evnum);
   nt->SetBranchAddress("unixTime",&utime);
   nt->SetBranchAddress("unixTimeUS",&utimeus);
   nt->SetBranchAddress("extra",&(extra[0]));
   TH1* h, * hf, * hfa, * hs, * hp;
   TH1F* h1f;
   TH2* h2;
   TString hn;
   for (Int_t ch=0; ch<kNchans; ch++) {
      nt->SetBranchAddress(Form("data%02d",ch), &(samples[ch][0]));

      hn = Form("hSampVsEvent_ch%02d",ch);
      h2 = new TH2F(hn.Data(),
         Form("Sample values vs event number (chan %d)"
            ";Event;Sample;ADC",ch),
         nents, -0.5, nents-0.5,
         kNsamps, -0.5, kNsamps-0.5);
      hSampVsEvent->AddAt(h2,ch);
      
      hn = Form("hPedSubSampVsEvent_ch%02d",ch);
      h2 = new TH2F(hn.Data(),
         Form("Ped subtracted ADCs vs event number (chan %d)"
            ";Event;Sample;ADC-ped",ch),
         nents, -0.5, nents-0.5,
         kNsamps, -0.5, kNsamps-0.5);
      hPedSubSampVsEvent->AddAt(h2,ch);
   }
   
   // try to load amp info
   Bool_t haveAmpInfo = configdb!=NULL;
   TPython::Exec("import buildRunInfo");
   TPython::Exec(Form("configs = buildRunInfo.loadConfigs(\"%s\")",configdb));
   haveAmpInfo = ( static_cast<Int_t>(TPython::Eval("len(configs)"))>0 );
   UChar_t* const ampInf = (haveAmpInfo) ? (new UChar_t[nents]) : 0;
   if (ampInf!=0) {
      memset(ampInf,0,nents*sizeof(UChar_t));
   }
   Int_t tai;
   
   // process events
   Printf("Processing tree...");
   TGraphErrors* gr;
   TObjArray* hsmch;
   Double_t mean, rms;
   UShort_t smp;
   for (Long64_t e=0; e<nents; e++) {
      nt->GetEntry(e + firstEvt);
      
      if ( (e%500)==0 ) {
         fprintf(stderr,"Processing %lld / %lld  (%02.2f%%)            \r",
            e, nents,
            100.*static_cast<Float_t>(e)/static_cast<Float_t>(nents));
      }
      
      Bool_t ok = kTRUE;
      if (ok && verifyChksum) {
         ok &= (mbchksum == (extra[21]<<16)+extra[20]);
      }
      
      TTimeStamp ts(static_cast<time_t>(utime), utimeus*1000); // ms to ns
      tsdir->WriteObject(&ts, Form("Time_ev%06lld",e));

      TParameter<Int_t> den(Form("DAQEvtNum_ev%06lld",e),
                            static_cast<Int_t>(evnum));
      endir->WriteObject(&den, den.GetName());

      TParameter<Int_t> trg(Form("TrigBit_ev%06lld",e),
                            static_cast<Int_t>(extra[kTrgBitIdx]));
      tgdir->WriteObject(&trg, trg.GetName());
      
      if (haveAmpInfo) {
         // python casting to int is more reliable
         // but use UChar for storage to save memory
         tai = static_cast<Int_t>(
            TPython::Eval(Form("buildRunInfo.getAmpsStateBitWord(%u)",utime)));
         if ( (tai>-1) && (tai<kMaxUChar) ) {
            ampInf[e] = static_cast<UChar_t>(tai);
         } else {
            Printf("Got invalid amp bit word [%d] at event [%lld].",tai,e);
         }
      }
      
      for (Int_t ch=0; ch<kNchans; ch++) {
         
         hsmch = dynamic_cast<TObjArray*>(hADCdistOfSample->At(ch));
         if (hsmch==0) {
            hsmch = new TObjArray(kNsamps);
            hADCdistOfSample->AddAt(hsmch, ch);
         }
         
         h2 = dynamic_cast<TH2*>(hSampVsEvent->At(ch));
         if (h2!=0) {
            for (Int_t sm=0; sm<kNsamps; sm++) {
               h = dynamic_cast<TH1*>(hsmch->At(sm));
               if (h==0) {
                  hn = Form("hADCdistOfSample_ch%02d_sm%03d",ch,sm);
                  h = new TH1F(hn.Data(),
                     Form("ADC distribution of chan %d, sample %d;ADC",ch,sm),
                     4096,-0.5,4095.5);
                  hsmch->AddAt(h,sm);
               }
               smp = (ok) ? (samples[ch][sm]) : -1;
               h2->SetBinContent(e+1, sm+1, smp);
               h->Fill(smp);
            }
            
         }
         
      }

   }
   
   // find pedestals
   Printf("Finding pedestals (used events with trg %hu)",trgForPeds);
   for (Int_t ch=0; ch<kNchans; ch++) {
      hsmch = dynamic_cast<TObjArray*>(hADCdistOfSample->At(ch));
      gr = dynamic_cast<TGraphErrors*>(gPedAndRmsVsSamp->At(ch));
      if (gr==0) {
         gr = new TGraphErrors(kNsamps);
         gr->SetName(Form("gPedAndRmsVsSamp_ch%02d",ch));
         gr->SetTitle(Form("Pedestals, RMS (chan %d);sample;ADCs",ch));
         gr->SetMarkerStyle(21);
         gr->SetMarkerSize(0.6);
         gr->SetLineColor(kGray+1);
         gPedAndRmsVsSamp->AddAt(gr, ch);
      }
      if (hsmch!=0) {
         for (Int_t sb=h2->GetNbinsY(); sb>0; sb--) {
            h = dynamic_cast<TH1*>(hsmch->At(sb-1));
            if (h!=0) {
               mean = h->GetMean();
               rms  = h->GetRMS();
               // get mean excluding 2-sigma outliers
               h->GetXaxis()->SetRangeUser(mean+(2.0*rms), mean-(2.0*rms));
               mean = h->GetMean();
               rms  = h->GetRMS();
               h->GetXaxis()->SetRange();
               // save pedestal & rms to graph
               gr->SetPoint(sb-1, h2->GetXaxis()->GetBinCenter(sb),  mean);
               gr->SetPointError(
                            sb-1, h2->GetXaxis()->GetBinWidth(sb)/2., rms/2.);
            }
         }
      }
   }
   
   // subtract pedestals
   Printf("Projecting 1D waveforms, subtracting pedestals...");
   TGraphErrors* gPed[kNchans]={0}, * gNoise[kNchans]={0};
   if (FPNfn!=NULL) {
      fpnf = TFile::Open(FPNfn);
      if ((fpnf!=0) && (fpnf->IsZombie()==kFALSE)) {
         for (Int_t ch=0; ch<kNchans; ch++) {
            gPed[ch]   = dynamic_cast<TGraphErrors*>(
               fpnf->Get(Form("gExlPed_ch%d",ch)));
            gNoise[ch] = dynamic_cast<TGraphErrors*>(
               fpnf->Get(Form("gExlRms_ch%d",ch)));
            if ( (gPed[ch]==0) || (gNoise[ch]==0) ) {
               Printf("Couldn't get FPN/noise graphs from [%s]",FPNfn);
            } else {
               Printf("Using pre-calculated FPN for chan %d",ch);
            }
         }
      } else {
         Printf("Couldn't open FPN file [%s]",FPNfn);
      }
   }
   TH2* h2ps;
   Double_t pss;
   for (Int_t ch=0; ch<kNchans; ch++) {
      Printf("Chan %02d",ch);
      
      h2   = dynamic_cast<TH2*>(hSampVsEvent->At(ch));
      h2ps = dynamic_cast<TH2*>(hPedSubSampVsEvent->At(ch));
      gr   = dynamic_cast<TGraphErrors*>(gPedAndRmsVsSamp->At(ch));
      
      if (h2!=0) {
         
         TObjArray* pswvs = dynamic_cast<TObjArray*>(hPedSubWvfm->At(ch));
         if (pswvs==0) {
            pswvs = new TObjArray(h2->GetNbinsX());
            hPedSubWvfm->AddAt(pswvs, ch);
         }
         
         for (Int_t ebin=h2->GetNbinsX(); ebin>0; ebin--) {
            if ( (ebin%500)==0 ) {
               fprintf(stderr,"Counting down %d / %d           \r",
                  ebin, h2->GetNbinsX());
            }
            
            // get individual waveform
            hn = Form("hWaveform_ch%02d_ev%06d",ch,ebin-1);
            h = h2->ProjectionY(hn.Data(),ebin,ebin);
            h->SetTitle(Form("Waveform of chan %d, event %d"
               ";sample;ADC",ch,ebin-1));
            
            if (h->GetBinContent(1)>-1) {
            
               hn = Form("hPedSubWvfm_ch%02d_ev%06d",ch,ebin-1);
               hp = new TH1F(hn.Data(),
                  Form("Ped. subtracted waveform (chan %d, event %d)"
                     ";sample;ADC-ped",ch,ebin-1),
                  kNsamps, -0.5, kNsamps-0.5);
               pswvs->AddAt(hp, ebin-1);

               if ((h2ps!=0) && (gr!=0)) {
                  const Double_t* const mn = 
                     (gPed[ch]!=0) ? (gPed[ch]->GetY()) : gr->GetY();
                  for (Int_t sbin=h2->GetNbinsY(); sbin>0; sbin--) {
                     pss  = h2->GetBinContent(ebin, sbin) - mn[sbin-1];
                     h2ps->SetBinContent(ebin, sbin, pss);
                     hp->SetBinContent(        sbin, pss);
                  }
               }
            }
            
            wvdir->WriteTObject(h);
            delete h; // don't need it anymore
         }
         fprintf(stderr,"\n");
      }
   }
   
   // find stop, shift waveform, do FFT
   Printf("Finding stop, shifting waveform, getting frequency profile...");
   StopList_t stops[kNchans];
   TH1* hsloc, * hslen, * hns, * hsadc, * hsoff;
   
   const Int_t nebins = h2->GetNbinsX();
   const Float_t nrm = (nebins>0) ? (1.0 / static_cast<Float_t>(nebins)) : 1;
   
   // try to load amp info
   if (haveAmpInfo) {
      Printf("Will use config db to determine amp on status "
             "(to find stop in FPN)");
   }
   
   for (Int_t ch=0; ch<kNchans; ch++) {
      Printf("Chan %02d",ch);
      
      hn = Form("hStopLoc_ch%02d",ch);
      hsloc = new TH1F(hn.Data(),
         Form("Stop locations in chan %d;Sample;Fraction events",ch),
         kNsamps, -0.5, kNsamps-0.5);
      hStopLocs->AddAt(hsloc, ch);
      
      hn = Form("hStopLocCand_ch%02d",ch);
      hsloc = new TH1F(hn.Data(),
         Form("Stop candidate locations in chan %d;Sample;Fraction events",ch),
         kNsamps, -0.5, kNsamps-0.5);
      hStopLocCands->AddAt(hsloc, ch);
      
      hn = Form("hStopLengths_ch%02d",ch);
      hslen = new TH1F(hn.Data(),
         Form("Stop lengths in chan %d;Stop length;Fraction events",ch),
         stpNumChn*3, -0.5, (stpNumChn*3)-0.5);
      hStopLens->AddAt(hslen, ch);
      
      hn = Form("hStopLengthCand_ch%02d",ch);
      hslen = new TH1F(hn.Data(),
         Form("Stop candidate lengths in chan %d"
              ";Stop length;Fraction events",ch),
         stpNumChn*3, -0.5, (stpNumChn*3)-0.5);
      hStopLenCands->AddAt(hslen, ch);
      
      hn = Form("hNumStops_ch%02d",ch);
      hns = new TH1F(hn.Data(),
         Form("Num of stop candidates per event in chan %d"
              ";Num stop candidates;Fraction events",ch),
         stpNumChn*3, -0.5, (stpNumChn*3)-0.5);
      hNumStops->AddAt(hns, ch);
      
      TObjArray* hsadcs = new TObjArray(kNsamps);
      hStopADCs->AddAt(hsadcs, ch);
      for (Int_t sm=0; sm<kNsamps; sm++) {
         hn = Form("hStopADCs_ch%02d_sm%03d",ch,sm);
         hsadc = new TH1F(hn.Data(),
            Form("ADC of stop readout in chan %d, sample %d;ADC",ch,sm),
            2*4095.5, -4095.5, 4095.5);
         hsadcs->AddAt(hsadc, sm);
      }
      
      TObjArray* hcadcs = new TObjArray(kNsamps);
      hStopCandADCs->AddAt(hcadcs, ch);
      for (Int_t sm=0; sm<kNsamps; sm++) {
         hn = Form("hStopCandADCs_ch%02d_sm%03d",ch,sm);
         hsadc = new TH1F(hn.Data(),
            Form("ADC of candidate stop readout in chan %d, sample %d"
                 ";ADC",ch,sm),
            2*4095.5, -4095.5, 4095.5);
         hcadcs->AddAt(hsadc, sm);
      }
      
      hn = Form("hAveFreq_ch%02d",ch);
      hfa = new TH1F(hn.Data(),
         Form("Average Frequency Profile of chan %d;"
         "Frequency (GHz);Magnitude",ch),
         kHalfsamps+1, -kfincr/2.0, kHalfsamps*kfincr);
      hAveFreqs->AddAt(hfa, ch);
   }
   
   UChar_t ampon = ~0; // default: assume all amps on
   
   for (Int_t ev=0; ev<nebins; ev++) {
      if ( (ev%100)==0 ) {
         fprintf(stderr,"Processing event %d / %d           \r",
            ev, nebins);
      }
      
      StopList_t bestStops(kNchans);
      
      if (haveAmpInfo) {
         ampon = ampInf[ev];
      }
      
      for (Int_t ch=0; ch<kNchans; ch++) {
         
         stops[ch].clear();
         
         TObjArray* pswvs = dynamic_cast<TObjArray*>(hPedSubWvfm->At(ch));
         if (pswvs==0) {
            Error("shifting","hPedSubWvfm empty at ch=%d",ch);
            continue;
         }
         // get individual waveform
         h1f = dynamic_cast<TH1F*>(pswvs->At(ev));
         
         if (h1f!=0) {
            
            // find stop
            if (ampon&(1<<ch)) {
               // amp on
               getStopLocs( h1f->GetArray() + 1, // don't include underflow
                            stops[ch],
                            stpNumChn,
                            stpOffset,
                            stpAbsWin,
                            stpRelWin,
                            kNsamps );
               bestStops[ch] = (stops[ch].empty()) ? LocLen_t(0,0) :
                  getLowRMSStop(stops[ch], h1f->GetArray() + 1, kNsamps);
            } else {
               getFPNStopLocs( h1f->GetArray() + 1, // don't include underflow
                               gNoise[ch]->GetY(),
                               stops[ch],
                               fpnNumChn,
                               fpnNumSig,
                               fpnNumOut,
                               fpnAbvBel,
                               kNsamps );
               bestStops[ch] = (stops[ch].empty()) ? LocLen_t(0,0) :
                  getMostOutliersStop(stops[ch],
                     h1f->GetArray() + 1,
                     gNoise[ch]->GetY(),
                     fpnNumSig,
                     fpnAbvBel,
                     kNsamps);
            }
            
            dynamic_cast<TH1*>(hNumStops->At(ch))->Fill(stops[ch].size());
            
            // histogram each candidate stop properties
            StopList_t::const_iterator sl, slend=stops[ch].end();
            for (sl=stops[ch].begin(); sl!=slend; sl++) {
               fillStopHists(*sl,
                  *(dynamic_cast<TH1*>(hStopLocCands->At(ch))),
                  *(dynamic_cast<TH1*>(hStopLenCands->At(ch))),
                  *(dynamic_cast<TObjArray*>(hStopCandADCs->At(ch))),
                  *h1f,
                  nrm);
            }
            
            // histogram chosen stop properties
            fillStopHists(bestStops[ch],
               *(dynamic_cast<TH1*>(hStopLocs->At(ch))),
               *(dynamic_cast<TH1*>(hStopLens->At(ch))),
               *(dynamic_cast<TObjArray*>(hStopADCs->At(ch))),
               *h1f,
               nrm);
            
            // shift waveform so stop is at end
            hn = Form("hShiftedPSWvfms_ch%02d_ev%06d",ch,ev);
            hs = newShiftedHist( *h1f, bestStops.at(ch), hn.Data(),
               Form("Shifted Ped Sub Waveform of chan %d, event %d;"
                  "Sample;ADC",ch,ev) );
            
            // transform waveform to frequency domain
            hn = Form("hWvFreq_ch%02d_ev%06d",ch,ev);
            hf = new TH1F(hn.Data(),
               Form("Frequency Profile of chan %d, event %d;"
                  "Frequency (GHz);Magnitude",
                  ch,ev),
               kHalfsamps+1, -kfincr/2.0, kHalfsamps*kfincr);

            // copy the data to the FFT input/output array
            Float_t* const wy  = new Float_t[kNsamps],
                   *       dwy = wy + kNsamps - 1;
            for (Int_t i=kNsamps; i>0; i--, dwy--) {
               *dwy = hs->GetBinContent(i);
            }
            // do the FFT
            realft(wy, kNsamps, 1);
            // copy the result to the histogram
            Int_t j=0;
            dwy = wy+2;
            Float_t* dw1 = dwy+1;
            hf->SetBinContent(++j, wy[1]);
            for (Int_t i=2; i<kNsamps; i+=2, dwy+=2, dw1+=2) {
              hf->SetBinContent(++j, TMath::Sqrt((*dwy)*(*dwy)+(*dw1)*(*dw1)));
            }
            hf->SetBinContent(++j, wy[1]);
            delete[] wy;
            
            // average the fft's
            dynamic_cast<TH1*>(hAveFreqs->At(ch))->Add(hf);
            
            // save & cleanup
            shdir->WriteTObject(hs);
            fqdir->WriteTObject(hf);
            delete hs;
            delete hf;
         }
      }
      
      // offsets
      for (Int_t ich=0; ich<kNchans; ich++) {
         const StopList_t& istops = stops[ich];
         StopList_t::const_iterator is, isend=istops.end();
         
         for (Int_t jch=0; jch<ich; jch++) {
            
            // offset of best candidates
            hn = Form("hStopOffsets_ch%02d_ch%02d",ich,jch);
            hsoff = dynamic_cast<TH1*>(hStopOffsets->FindObject(hn.Data()));
            if (hsoff==0) {
               hsoff = new TH1F(hn.Data(),
                  Form("Stop offsets chan %d - chan %d"
                       ";ch%d - ch%d (ADC)",ich,jch,ich,jch),
                  kNsamps+1, -kHalfsampF, kHalfsampF);
               hStopOffsets->Add(hsoff);
            }
            
            hsoff->Fill(
               getOffsetMod(bestStops.at(ich).first, bestStops.at(jch).first),
               nrm);
            
            // offsets of all candidates
            hn = Form("hStopCandOffsets_ch%02d_ch%02d",ich,jch);
            hsoff = dynamic_cast<TH1*>(hStopCandOffsets->FindObject(hn.Data()));
            if (hsoff==0) {
               hsoff = new TH1F(hn.Data(),
                  Form("Candidate stop offsets chan %d - chan %d"
                       ";ch%d - ch%d (ADC)",ich,jch,ich,jch),
                  kNsamps+1, -kHalfsampF, kHalfsampF);
               hStopCandOffsets->Add(hsoff);
            }
            
            const StopList_t& jstops = stops[jch];
            
            StopList_t::const_iterator js, jsend=jstops.end();
            for (is=istops.begin(); is!=isend; is++) {
               for (js=jstops.begin(); js!=jsend; js++) {
                  hsoff->Fill( 
                     getOffsetMod( is->first, js->first ),
                     nrm);
               }
            }
            
         }
      }
   
   }
   
   fprintf(stderr,"\n");
   
   // normalize
   for (Int_t ch=0; ch<kNchans; ch++) {
      
      hfa = dynamic_cast<TH1*>(hAveFreqs->At(ch));
      hfa->Scale(nrm);
      const Float_t sqrthn = TMath::Sqrt(1.0/nrm);
      for (Int_t jj=hfa->GetNbinsX(); jj>0; jj--) {
         hfa->SetBinError(jj, sqrthn/2.);
      }
      
   }
   
   // save remaining
   Printf("Saving output. Please be patient.");
   outf->cd();
   hSampVsEvent->Write();
   hPedSubSampVsEvent->Write();
   const Int_t ngrs = gPedAndRmsVsSamp->GetEntries();
   for (Int_t i=0; i<ngrs; i++) {
      gr = dynamic_cast<TGraphErrors*>(gPedAndRmsVsSamp->At(i));
      gr->Write(gr->GetName());
   }
   writeAndClean(hSampVsEvent, outf);
   writeAndClean(hPedSubSampVsEvent, outf);
   writeAndClean(hADCdistOfSample, smdir);
   // hWaveform already saved & cleaned
   writeAndClean(hPedSubWvfm, psdir);
   writeAndClean(hStopLocs, slocdir);
   writeAndClean(hStopLocCands, slccdir);
   writeAndClean(hStopLens, slendir);
   writeAndClean(hStopLenCands, slncdir);
   writeAndClean(hNumStops, nstpdir);
   writeAndClean(hStopADCs, sadcdir);
   writeAndClean(hStopCandADCs, cadcdir);
   writeAndClean(hStopOffsets, soffdir);
   writeAndClean(hStopCandOffsets, coffdir);
   // hShiftedPSWvfms already saved & cleaned
   // hWvFreq already saved & cleaned
   writeAndClean(hAveFreqs, outf);
   delete outf;
   delete[] ampInf;
   
}

void writeAndClean(TCollection* plots, TDirectory* odir,
                   const Char_t* wname, const Int_t woption) {
   if (plots!=0) {
      if (odir!=0) {
         TDirectory* sdir = gDirectory;
         odir->cd();
         plots->Write(wname, woption);
         plots->Delete();
         delete plots;
         gDirectory = sdir;
      } else {
         Error("writeAndClean","Can't write to null output directory");
      }
   } else {
      Error("writeAndClean","Can't write null collection.");
   }
}

void fillStopHists(const LocLen_t& stop,
                   TH1& hloc, TH1& hlen,
                   TObjArray& hsadcs,
                   const TH1& hdata,
                   const Float_t nrm) {
   hloc.Fill(stop.first, nrm);
   hlen.Fill(stop.second, nrm);
   TH1* hsadc;
   for (Int_t ss=0; ss<stop.second; ss++) {
      Int_t b = stop.first + ss;
      if ( b >= kNsamps ) {
         b -= kNsamps;
      }
      hsadc = dynamic_cast<TH1*>(hsadcs.At(b));
      if (hsadc!=0) {
         hsadc->Fill(hdata.GetBinContent(b+1), nrm);
      }
   }
   
}

Int_t getOffsetMod(const Int_t s1, const Int_t s2) {
   Int_t off = s1 - s2;
   if (TMath::Abs(off)>kHalfsamps) {
      off = (s1 < s2) ?
            ((s1+kNsamps) - s2) :
            (s1 - (s2+kNsamps));
   }
   return off;
}
