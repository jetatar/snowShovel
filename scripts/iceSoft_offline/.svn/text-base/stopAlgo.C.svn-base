#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TCanvas.h>
#include <TH1.h>
#include <TMath.h>
#include <TString.h>
#include <TError.h>

#include <vector>
#include <utility>
#endif

/*
** This file can be included in other macros.
** It contains algorithms for locating the stop in time dep waveform data.
*/

//#define DEBUG

TCanvas* cdbg;

typedef std::pair<Int_t, Int_t>   LocLen_t;
typedef std::vector< LocLen_t >   StopList_t;
typedef std::vector< StopList_t > ChStopLists_t;
typedef std::pair<Int_t, Int_t>   ChanPair_t;

typedef std::vector<Int_t>        Offsets_t;

template<typename T, typename U>

Bool_t isOutlier(const T val, const U rms,
                 const Float_t nsig,
                 const Int_t abvbel) {
   // abvbel = +1 to test for positive outlier
   //          -1 to test for negative outlier
   //           0 to test for any outlier
   //
   // rms and nsig must be positive (not checked)
   
   if (abvbel>0) {
      return (val > (nsig*rms));
   } else if (abvbel<0) {
      return (val < -(nsig*rms));
   } else {
      return (TMath::Abs(val) > (nsig*rms));
   }
}

StopList_t::const_reference getFirstStop(const StopList_t& stopLocs) {
   return stopLocs.front();
}

template<typename T, typename U>
StopList_t::const_reference getMostOutliersStop(const StopList_t& stopLocs,
                                                const T* const dat,
                                                const U* const rms,
                                                const Float_t nsig,
                                                const Int_t   abvbel,
                                                const Int_t   Nsamps) {
   // given the stop locations and the data & rms of one channel,
   // find the stop that has the most outliers.
   // if multiple, find the remaining stop with lowest rms
   //
   // see getFPNStopLocs for variable definitions
   
   StopList_t bestest;
   Int_t mout=-1;
   StopList_t::const_iterator sl, send=stopLocs.end(), best;
   for (sl=stopLocs.begin(); sl!=send; sl++) {
      // count outliers for this stop
      Int_t nout=0;
      for (Int_t i=sl->first; i<sl->second; i++) {
         Int_t j = i;
         if (j >= Nsamps) {
            j -= Nsamps;
         }
         if ( isOutlier(dat[j], rms[j], nsig, abvbel) ) {
            nout++;
         }
      }
      if ( (mout<0) || (nout>mout) ) {
         mout = nout;
         best = sl;
         bestest.clear();
         bestest.push_back(*sl);
      } else if (nout==mout) {
         bestest.push_back(*sl);
      }
   }
   
   if (bestest.size()<=1) {
      return *best;
   } else {
      StopList_t::const_reference best = getLowRMSStop(bestest, dat, Nsamps);
      for (sl=stopLocs.begin(); sl!=send; sl++) {
         if ( (sl->first == best.first) && (sl->second == best.second) ) {
            return *sl;
         }
      }
      // shouldn't get here
      Error("getMostOutliersStop",
         "Had multiple outlier candidates (mout=%d), but couldn't find best.",
         mout);
      return getFirstStop(stopLocs);
   }
}

template<typename T>
StopList_t::const_reference getLowRMSStop(const StopList_t& stopLocs,
                                          const T* const dat,
                                          const Int_t Nsamps) {
   // given the stop locations and the data of one channel,
   // find the stop that has the lowest spread in its ADC values
   
   Float_t minRms=-1;
   StopList_t::const_iterator sl, send=stopLocs.end(), best=stopLocs.begin();
   for (sl=stopLocs.begin(); sl!=send; sl++) {
      Float_t mean=0, rms=0, n=0;
      for (Int_t i=sl->first; i<sl->second; i++) {
         Int_t j = i;
         if (j >= Nsamps) {
            j -= Nsamps;
         }
         const T c = dat[j];
         mean += c;
         rms  += c*c;
         n    += 1.0;
      }
      if (n>1) {
         mean /= n-1;
         rms  /= n-1;
         rms  -= mean*mean;
         rms   = TMath::Abs(rms); // really rms2, but we only want the min.
         if ( (minRms<0) || (rms<minRms) ) {
            minRms = rms;
            best   = sl;
         }
      }
   }
   return *best;
}

template<typename T, typename U>
void getFPNStopLocs(const T* const dat,
                    const U* const rms,
                    StopList_t&    stopLocs,
                    const Int_t    stpNumChn,
                    const Float_t  stpNumSig,
                    const Int_t    stpNumOut,
                    const Int_t    stpAbvBel,
                    const Int_t    Nsamps) {
   //
   // dat = the array of adc values for each sample. should be Nsamps in length.
   //       this MUST be pedestal subtracted data for this algorithm to work!
   //
   // rms = the array of noise values (in adc units) of each sample
   // 
   // stopLocs = "output" vector to be filled (will be reset)
   //    a list of stop candidates; each entry is a pair of
   //    <stop location sample # , stop length (num samps)>
   //
   // stpNumChn = number of samples in the window (i.e. expected stop length)
   // stpNumSig = the number of sigmas away from 0 for a sample to be counted
   // stpNumOut = the minimum number of outliers in the window (inclusive)
   // stpAbvBel = +1 if all stop samples should be positive outliers
   //             -1 if all stop samples should be negative outliers
   //              0 if stop samples can be mixed outliers
   //
   // note that with this algorithm, the stop length will always be stpNumChn!
   
   stopLocs.clear();
   
   const T* c = dat;
   const U* r = rms;
   for (Int_t i=0; i<Nsamps; i++, c++, r++) {
      if (isOutlier(*c, *r, stpNumSig, stpAbvBel)) {
         // start window
         Int_t nouts=1;
         Int_t nb;
         for (Int_t j=nouts; j<stpNumChn; j++) {
            nb = i+j;
            if (nb >= Nsamps) {
               nb -= Nsamps;
            }
            if (isOutlier( dat[nb], rms[nb], stpNumSig, stpAbvBel)) {
               nouts++;
            }
         }
         if (nouts>=stpNumOut) {
            stopLocs.push_back( LocLen_t(i, stpNumChn) );
         }
      }
   }
   
}

template<typename T>
void getStopLocs(const T* const dat,
                 StopList_t&   stopLocs,
                 const Int_t   stpNumChn,
                 const Float_t stpOffset,
                 const Float_t stpAbsWin,
                 const Int_t   stpRelWin,
                 const Int_t   Nsamps) {
   //
   // dat = the array of adc values for each sample. should be Nsamps in length
   //
   // stopLocs = "output" vector to be filled (will be reset)
   //    a list of stop candidates; each entry is a pair of
   //    <stop location sample # , stop length (num samps)>
   //
   // stpNumChn = min num of stop samples in a row
   // stpOffset = absolute offset from ped of stop
   // stpAbsWin = absolute dist of stop from ped
   // stpRelWin = relative adc spread of stop  
   //
   
#ifdef DEBUG
   cdbg = new TCanvas;
#endif
   
   stopLocs.clear();
   
#ifdef DEBUG
   cdbg->Clear();
   cdbg->cd();
   TH1* hp = new TH1F("hdbg","",Nsamps+0,-0.5,Nsamps);
   for (Int_t i=0; i<Nsamps; i++) {
      hp->SetBinContent(i+1, dat[i]);
   }
   hp->Draw();
   cdbg->Update();
#endif
         
   const T* c = dat;
   for (Int_t i=0; i<Nsamps; i++, c++) {

      if (TMath::Abs( (*c) - stpOffset ) < stpAbsWin) {
         Int_t swid=1, nb;
         for (Int_t j=swid; j<Nsamps; j++) {
         //for (Int_t j=swid; j<stpNumChn; j++) {
            nb = i+j;
            if (nb >= Nsamps) {
               nb -= Nsamps;
            }
            const T cn = dat[nb];
#ifdef DEBUG
            Printf("cn[%d]=%g . del=%g . relwin=%d",
               nb, cn, (*c-cn), stpRelWin);
#endif
            if ( TMath::Abs( (*c) - cn ) > stpRelWin ) {
               // no longer in a row
               break;
            } else {
               swid++;
            }
         }
         
#ifdef DEBUG
         Printf("swid=%d, numchn=%d", swid, stpNumChn);
#endif
         if (swid>=stpNumChn) {
            stopLocs.push_back( LocLen_t(i, swid) );
         }
         
      }
   }
   
#ifdef DEBUG
   cdbg->WaitPrimitive();
   delete hp;
#endif
   
}

TH1* newShiftedHist( TH1& data,
                    const LocLen_t& stop,
                    const Char_t* shftName,
                    const Char_t* shftTitle) {
   // assumes stop inside        [0, nbins)
   // while histograms goes from [1, nbins]
   //
   // returns a new histogram that should be deleted by the caller
   //
   // note: don't use the Form() macro in both shftName and shftTitle
   
   const Int_t nbins = data.GetNbinsX();
   if ( (stop.first<0) || (stop.first>=nbins) ) {
      Fatal("getShiftedHist","Stop (loc %d, len %d) out of bounds (nbins %d)",
         stop.first, stop.second, nbins);
      return 0;
   }
   
   TH1* shft = dynamic_cast<TH1*>(data.Clone());
   shft->Reset();
   shft->SetName(shftName);
   shft->SetTitle(shftTitle);
   
   // put the last stop sample at nbins
   const Int_t start = stop.first + stop.second; // ok if >= nbins here
   Int_t j;
   for (Int_t i=0; i<nbins; i++) {
      j = start + i;
      while (j > nbins) {
         j -= nbins;
      }
      shft->SetBinContent(i+1, data.GetBinContent(j+1));
   }
   
   return shft;
   
}


