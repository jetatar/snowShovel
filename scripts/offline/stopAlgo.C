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

template<typename T>
StopList_t::const_reference getLowRMSStop(const StopList_t& stopLocs,
                                          const T* const dat,
                                          const Int_t Nsamps);

Bool_t isInStop(const Int_t smp, const LocLen_t& stp,
                const Int_t nsamps) {
   if (stp.first>=nsamps) {
      Fatal("isInStop","Invalid stop begin (%d) for nsamps %d",
            stp.first, nsamps);
   }
   const Int_t exlast = stp.first + stp.second;
   if ( exlast <= nsamps ) {
      // doesn't wrap around
      return ( (smp<exlast) && (smp>=stp.first) );
   } else {
      // stop wraps around
      const Int_t exl = exlast - nsamps;
      if (exl<=nsamps) {
         return ( (smp>=stp.first) || (stp.second<exl) );
      } else {
         Fatal("isInStop","Invalid stop (beg %d, len %d) for nsamps %d",
               stp.first, stp.second, nsamps);
      }
   }
   return kFALSE;
}

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
   for (sl=stopLocs.begin(); sl!=send; ++sl) {
      // count outliers for this stop
      Int_t nout=0;
      for (Int_t i=sl->first; i<sl->second; ++i) {
         Int_t j = i;
         if (j >= Nsamps) {
            j -= Nsamps;
         }
         if ( isOutlier(dat[j], rms[j], nsig, abvbel) ) {
            ++nout;
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
      for (sl=stopLocs.begin(); sl!=send; ++sl) {
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
   for (sl=stopLocs.begin(); sl!=send; ++sl) {
      Float_t mean=0, rms=0, n=0;
      for (Int_t i=sl->first; i<sl->second; ++i) {
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
         rms  -= (mean*mean)/n;
         rms  /= n-1;
         rms   = TMath::Abs(rms); // really rms2, but we only want the min.
         // mean /= n-1; // not used
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
   for (Int_t i=0; i<Nsamps; ++i, ++c, ++r) {
      if (isOutlier(*c, *r, stpNumSig, stpAbvBel)) {
         // start window
         Int_t nouts=1;
         Int_t nb;
         for (Int_t j=nouts; j<stpNumChn; ++j) {
            nb = i+j;
            if (nb >= Nsamps) {
               nb -= Nsamps;
            }
            if (isOutlier( dat[nb], rms[nb], stpNumSig, stpAbvBel)) {
               ++nouts;
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
   for (Int_t i=0; i<Nsamps; ++i) {
      hp->SetBinContent(i+1, dat[i]);
   }
   hp->Draw();
   cdbg->Update();
#endif
         
   const T* c = dat;
   for (Int_t i=0; i<Nsamps; ++i, ++c) {

      if (TMath::Abs( (*c) - stpOffset ) < stpAbsWin) {
         Int_t swid=1, nb;
         for (Int_t j=swid; j<Nsamps; ++j) {
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
               ++swid;
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
   for (Int_t i=0; i<nbins; ++i) {
      j = start + i;
      while (j >= nbins) {
         j -= nbins;
      }
      shft->SetBinContent(i+1, data.GetBinContent(j+1));
   }
   
   return shft;
   
}

TH1* newSignalCutoffHist(const Char_t* scname,
                         const Char_t* sctitle,
                         const TH1&    data,
                         const Int_t   numSmps=6) {
   // generate plot in which
   //
   //          (1/N) Sum_j=i^(i+N)[ y_j*y_j ]       ave N (y^2) after i
   // Y_i = ------------------------------------ =  --------------------
   //       (1/N) Sum_j=(i-1)^(i-N-1)[ y_j*y_j ]    ave N (y^2) before i
   //
   //
   // where Y_i is the contents of the new hist's bin i,
   // y_j is the contents of 'data's bin j,
   // and N='numSmps'
   
   if (numSmps<2) {
      Fatal("newSignalCutoffHist","Need multiple samples to calculate averages");
   }
   
   TH1* hr = dynamic_cast<TH1*>(data.Clone());
   hr->Reset();
   hr->SetName(scname);
   hr->SetTitle(sctitle);
   
   Int_t nb;
   const Int_t nbins = data.GetNbinsX();
   for (Int_t sm=nbins; sm>0; --sm) {
      Double_t a=0, n=0, c=0, x=0;
      for (Int_t i=0; i<numSmps; ++i) {
         nb = sm+i;
         if (nb>nbins) {
            nb -= nbins;
         }
         c  = data.GetBinContent(nb);
         n += 1.0;
         a += c*c;
      }
      a /= (n-1.0);
      x  = a;
      a = n = c = 0;
      for (Int_t i=-numSmps; i<0; ++i) {
         nb = sm+i;
         if (nb>nbins) {
            nb -= nbins;
         }
         c  = data.GetBinContent(nb);
         n += 1.0;
         a += c*c;
      }
      a /= (n-1.0);
      x /= a;
      
      hr->SetBinContent(sm, x);
   }
   return hr;
}

TH1* newSampleSpreadHist(const Char_t* ssname,
                         const Char_t* sstitle,
                         TH1** chdata,
                         const UInt_t nch=4) {
   if (nch<2) {
      Fatal("newSampleSpreadHist","Need multiple channels to calculate spread");
   }
   TH1** h = chdata;
   for (UInt_t i=0; i<nch; ++i, ++h) {
      if (*h==0) {
         Fatal("newSampleSpreadHist","Null histogram at channel %d",i);
      }
   }
   
   TH1* hr = dynamic_cast<TH1*>(chdata[0]->Clone());
   hr->Reset();
   hr->SetName(ssname);
   hr->SetTitle(sstitle);
   
   for (UInt_t sm=chdata[0]->GetNbinsX(); sm>0; --sm) {
      h = chdata;
      Double_t ave=0, rms=0, n=0, c=0;
      for (UInt_t i=0; i<nch; ++i, ++h) {
         c    = (*h)->GetBinContent(sm);
         n   += 1.0;
         ave += c;
         rms += c*c;
      }
      rms -= (ave*ave)/n;
      rms /= (n-1.0);
      rms  = TMath::Sqrt(rms);
      
      hr->SetBinContent(sm, rms);
      hr->SetBinError(sm, rms / TMath::Sqrt(2.0*(n-1.0)));
   }

   return hr;

}

void getStopsFromSampleSpread(StopList_t& stopLocs,
                              const TH1&  hSmpSpread,
                              const Int_t stpNumSmp=5,
                              const Float_t relWin=2.0) {
   // find the minimum in the hSmpSpread histogram
   // and look for stpNumSmp consecutive samples within a factor relWin
   // of the minimum
   //
   // hSmpSpread should come from newSampleSpreadHist
   // it is assumed that all bins in this histogram have non-negative content
   // (since the content should be an RMS)
   
   stopLocs.clear();
   
   const Double_t mc = hSmpSpread.GetMinimum();
   const Double_t cut = mc * relWin;
   Double_t c;
   Int_t nb;
   const Int_t nbins = hSmpSpread.GetNbinsX();
   for (Int_t i=0; i<nbins; ++i) {
      c = hSmpSpread.GetBinContent(i+1);
      if ( c < cut ) {
         Int_t consec=0;
         for (Int_t j=0; j<nbins; ++j) {
            nb = i+j;
            if (nb >= nbins) {
               nb -= nbins;
            }
            if (hSmpSpread.GetBinContent(nb+1)<cut) {
               ++consec;
            } else {
               break;
            }
         }
         if (consec>=stpNumSmp) {
            // stop candidate
            stopLocs.push_back( LocLen_t(i, consec) );
         }
      }
   }
}

TH1* newAveSmpSpreadGroupsHist(const Char_t* rsname,
                               const Char_t* rstitle,
                               const TH1&    hss,
                               const Int_t   grpNsmp=6) {
   TH1* hr=0;
   if (grpNsmp>1) {
      hr = dynamic_cast<TH1*>(hss.Clone());
      hr->Reset();
      hr->SetName(rsname);
      hr->SetTitle(rstitle);
   
      Int_t nb;
      const Int_t nbins = hss.GetNbinsX();
      for (Int_t i=0; i<nbins; ++i) {
         Double_t ave=0, rms=0, n=0, c=0;
         for (Int_t j=0; j<grpNsmp; ++j) {
            nb = i+j;
            if (nb >= nbins) {
               nb -= nbins;
            }
            c    = hss.GetBinContent(nb+1);
            n   += 1.0;
            ave += c;
            rms += c*c;
         }
         rms -= (ave*ave)/n;
         rms /= (n-1.0);
         rms  = TMath::Sqrt(rms);
         ave /= (n-1.0);
         //hr->SetBinContent(i+1, rms);
         //hr->SetBinError(i+1, rms / TMath::Sqrt(2.0*(n-1.0)));
         hr->SetBinContent(i+1, ave);
         hr->SetBinError(i+1, rms / TMath::Sqrt(n));
      }
   } else {
      Error("newRmsSmpSpreadGroupsHist",
            "Need grpNsmp>2 to calculate rms. Currently %d.",grpNsmp);
   }
   return hr;
}
