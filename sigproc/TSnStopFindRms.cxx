#include "TSnStopFindRms.h"

#include <TMath.h>

#include "TSnStopPosnSet.h"
#include "TSnCalWvData.h"
#include "NSnConstants.h"

ClassImp(TSnStopFindRms);

Bool_t TSnStopFindRms::GetRmsInStop(const UChar_t ch,
                                    const UChar_t startSample,
                                    const UChar_t smpwid,
                                    Float_t& rms) const {
   Bool_t ok(kFALSE);
   Float_t ave(0), n(0);
   rms = 0;
   const Float_t* const dat = fDat->GetData(ch);
   const Float_t* d = dat + startSample;
   for (UChar_t s=0, ss=startSample; s<smpwid; ++s, ++ss, ++d) {
      d += NSnConstants::WrapSampleNum(ss);
      ave += *d;
      rms += (*d) * (*d);
      n   += 1.0;
   }
   if (n>1.0) {
      rms -= (ave*ave)/n;
      rms /= (n-1.0);
      rms  = TMath::Sqrt(rms);
      ok = kTRUE;
   }
   return ok;
}

Bool_t TSnStopFindRms::FindStopInRange(TSnStopPosnSet& stops,
                                       Float_t& minAveRms,
                                       const UChar_t chan,
                                       const UChar_t beginSmp,
                                       const UChar_t endSmp) const {
   // endSmp is exclusive
   
   Float_t rms;
   minAveRms = 0;
   UChar_t minAveRmsSmp(0);
   Bool_t  first(kTRUE);
   for (UChar_t sm=beginSmp; sm!=endSmp; NSnConstants::WrapSampleNum(++sm)) {
      const Bool_t ok = GetRmsInStop(chan, sm, fStopWid, rms);
      if (ok) {
         if ( first || (rms<minAveRms) ) {
            minAveRms    = rms;
            minAveRmsSmp = sm;
            first        = kFALSE;
         }
      }
   }
   if (first!=kTRUE) {
      stops.SetFirstSample(chan, minAveRmsSmp);
      
   }
   return (!first);
}

void TSnStopFindRms::FindStopByRms(TSnStopPosnSet& stops) const {

   // set to max because fStopWid ignored if it's less than fSrchWid
   stops.SetStopWidth( TMath::Max(fStopWid, fSrchWid) );
   
   // find the fSrchWid wide group of samples that have the lowest rms
   // in all channels together (since the stop should be mostly or
   // completely overlapping across all channels)
   Float_t minAveRms(0);
   UChar_t minAveRmsSmp(0);
   Bool_t  first(kTRUE);
   Float_t rms;
   for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm) {
      Float_t aveRms(0), n(0);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         const Bool_t ok = GetRmsInStop(ch, sm, fSrchWid, rms);
         if (ok) {
            aveRms += rms;
            n      += 1.0;
         }
      }
      if (n>0) {
         aveRms /= n;
         if ( first || (aveRms<minAveRms) ) {
            minAveRms    = aveRms;
            minAveRmsSmp = sm;
            first        = kFALSE;
         }
      }
   }
   
   const Int_t shift = fStopWid - fSrchWid;
   if (shift>0) {
      // now find the group of fStopWid containing fSrchWid that is most
      // likely to be the stop. different channels are shifted independently.
      // assume the correct stop samples have the lowest rms.
      
      Int_t begin = minAveRmsSmp - shift;
      NSnConstants::WrapSampleNum(begin);
      Int_t end   = begin + shift+1;
      NSnConstants::WrapSampleNum(end);
      Float_t qual(0), nch(0);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         Float_t rms;
         const Bool_t ok = FindStopInRange(stops, rms, ch, begin, end);
         if (ok) {
            qual += rms;
            nch  += 1.0;
         }
      }
      if (nch>1) {
         qual /= nch;
      }
      stops.SetStopQuality(qual);
      
   } else {
      // with no shift, simply set all channels to the same stop position
      //
      // negative shift treated same as no shift. (i.e. fStopWid ignored)
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         stops.SetFirstSample(ch, minAveRmsSmp);
      }
      stops.SetStopQuality(minAveRms);
   }

   // finally, allow the stops in each channel to be further shifted
   // independently. some of these checks may have been tried already,
   // but not all because fJitter may be larger than shift. the checks
   // are quite fast in any case.
   if (fJitter>0) {
      Float_t qual(0), nch(0);
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         Int_t begin = stops.GetFirstSample(ch) - fJitter;
         NSnConstants::WrapSampleNum(begin);
         Int_t end   = stops.GetFirstSample(ch) + fJitter;
         NSnConstants::WrapSampleNum(end);
         Float_t rms;
         const Bool_t ok = FindStopInRange(stops, rms, ch, begin, end);
         if (ok) {
            qual += rms;
            nch  += 1.0;
         }
      }
      if (nch>1) {
         qual /= nch;
      }
      stops.SetStopQuality(qual);
   }
}

void TSnStopFindRms::SlaveBegin() {
   ReqBranch(fDatBrNm.Data(), fDat);
}

void TSnStopFindRms::Process() {
   
   LoadBranch(fDatBrNm.Data());
   if (fDat!=0) {
   
      TString pt = Form("TSnStopFindRms: StopWidth=%hu, SearchWidth=%hu",
                        fStopWid, fSrchWid);
      TSnStopPosnSet* stops = new TSnStopPosnSet(fStopNm.Data(), pt.Data());
   
      FindStopByRms(*stops);
   
      AddObjThisEvt(stops);
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not load data from branch [%s]",
                fDatBrNm.Data());
   }
}


