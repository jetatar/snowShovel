#include "TSnFindPureThermalFromAutoCorrMod.h"

#include <TParameter.h>
#include <TObjArray.h>

#include "TSnInterp1DWvData.h"
#include "NSnConstants.h"

#include <stdexcept>

ClassImp(TSnFindPureThermalFromAutoCorrMod);
////////////////////////////////////////////////////////////////////////////////
//
// $Id$
// 
// TSnFindPureThermalFromAutoCorrMod
// 
// Determine whether an event contains only pure thermal noise from the
// amplifiers and electronics. The autocorrelation functions of each channel
// are used to make the determination.
//
// This module depends on the output of TSnChanTimeCorlAltEvtsMod when it is
// run with both the main and alt event branches having the same name, which
// results in its calculating the auto correlation. (No alternate event loader
// is needed for this.)
//
// Currently, only one algorithm for identifying a pure thermal event is
// available, but in the future, others may be settable via SetAutoCorrName.
//
// The "SmallestAnyChan" variable identifies a NON pure thermal event by
// checking whether any auto correlation value on any channel is below the
// cut (see SetAutoCorrCut).
//
// Once tagged as pure thermal or not-pure-thermal, several different
// behaviors are possible for this module:
//    (a) SetTagBehavior("AddTagToEvent")
//        This is the default behavior. No event selection will be applied,
//        instead a TParameter<Bool_t> object will be added to the event with
//        the name specified by SetPureThermalTagName (defaults to
//        "IsPureThermalFromAutoCorr"). The boolean is true of the event
//        is pure thermal and false if not.
//    (b) SetTagBehavior("CutPureThermal")
//        The module will behave as an event selection module and skip all
//        pure thermal events. (Keep non-thermal background and signal.)
//    (c) SetTagBehavior("CutNonPureThermal")
//        The module will behave as an event selection module and skip all
//        events NOT identified as pure thermal. (Keep pure thermal.)
// 
////////////////////////////////////////////////////////////////////////////////

TSnFindPureThermalFromAutoCorrMod::TSnFindPureThermalFromAutoCorrMod(
                                                   const Char_t* name,
                                                   const Char_t* autoCorrNm) :
   TAModule(name, "tag pure thermal events using auto-correlations"),
   fAutoCorrNm(autoCorrNm),
   fPureThmTagNm("IsPureThermalFromAutoCorr"),
   fAutoCorrTagAlgo(kSmallestAnyChan),
   fBehavior(kAddTagToEvent),
   fAutoCorrCut(-0.45) {
   // normal ctor
}

TSnFindPureThermalFromAutoCorrMod::EAutoCorrTagAlgo_t
TSnFindPureThermalFromAutoCorrMod::GetAutoCorrTagAlgoFromString(
                                                             const TString n) {
   // try to convert from a string to the enum value
   // throws an exception if the string is unknown
   
   if (n.CompareTo("SmallestAnyChan")==0) {
      return kSmallestAnyChan;
   } else {
      throw std::runtime_error(Form("Unknown auto correlation tag algorithm "
                                    "[%s].",n.Data()));
   }
}

TSnFindPureThermalFromAutoCorrMod::EBehavior_t 
TSnFindPureThermalFromAutoCorrMod::GetTagBehaviorFromString(const TString n) {
   // try to convert from a string to the enum value
   // throws an exception if the string is unknown
   
   if (n.CompareTo("CutPureThermal")==0) {
      return kCutPureThermal;
   } else if (n.CompareTo("CutNonPureThermal")==0) {
      return kCutNonPureThermal;
   } else if (n.CompareTo("AddTagToEvent")==0) {
      return kAddTagToEvent;
   } else {
      throw std::runtime_error(Form("Unknown tag behavior [%s].",n.Data()));
   }
}
   

void TSnFindPureThermalFromAutoCorrMod::Process() {
   // find the auto correlation object(s) and check whether this event
   // is pure thermal or not.
   //
   // the auto correlations come from the output of
   // TSnChanTimeCorlAltEvtsMod with main and alt event branch names
   // being the same
   
   const TObjArray* acs = dynamic_cast<const TObjArray*>(
      FindObjThisEvt(fAutoCorrNm.Data()));
   if (acs!=0) {

      Bool_t ok(kFALSE), isPureThm(kFALSE);
      switch (fAutoCorrTagAlgo) {
         case kSmallestAnyChan:
            ok = DoSmallestAnyChanCheck(acs, isPureThm);
            break;
         default:
            ok = kFALSE;
            break;
      };
      
      if (ok) {
         if (kCutPureThermal==fBehavior) {
            if (isPureThm) {
               SkipEvent();
               
               if (GetVerbosity()>10) {
                  SendError(kWarning, "Process",
                            "Skipping pure thermal event.");
               }
            }
         } else if (kCutNonPureThermal==fBehavior) {
            if (isPureThm==kFALSE) {
               SkipEvent();
               
               if (GetVerbosity()>10) {
                  SendError(kWarning, "Process",
                            "Skipping event that is NOT pure thermal.");
               }
            }
         } else if (kAddTagToEvent==fBehavior) {
            
            TParameter<Bool_t>* v = new TParameter<Bool_t>(fPureThmTagNm.Data(),
                                                           isPureThm);
            AddObjThisEvt(v); // owned by event

            if (GetVerbosity()>99) {
               Info("Process","Added [%s]=%s to event",
                    fPureThmTagNm.Data(),
                    (isPureThm ? "true" : "false"));
            }
            
         } else {
            SendError(kAbortAnalysis, "Process",
                      "Unknown behavior option [%d]",
                      static_cast<Int_t>(GetTagBehaviorVal()));
         }
      } // else should already have called SendError
      
      
   } else {
      SendError(kAbortAnalysis, "Process",
                "Could not get autocorrelation object named [%s]",
                fAutoCorrNm.Data());
   }
   
}

Bool_t TSnFindPureThermalFromAutoCorrMod::DoSmallestAnyChanCheck(
                                                const TObjArray* const acs,
                                                Bool_t& isPureThm) {
   // identifies a pure thermal trigger by finding the minimum value of the
   // autocorrelation on each channel and checking whether this value is
   // less than the auto correlation cut value (see SetAutoCorrCut).
   //
   // if any channel has a min auto correlation value below the cut value,
   // then the event is NOT pure thermal.

   Bool_t ok = kTRUE;
   isPureThm = kTRUE;
   const TSnInterp1DWvData* ac(0);
   for (Int_t ch=0; ch<NSnConstants::kNchans; ++ch) {
      ac = dynamic_cast<const TSnInterp1DWvData*>(acs->At(ch));
      if (ac!=0) {
         
         if (ac->FindMinimum() < fAutoCorrCut) {
            isPureThm = kFALSE;

            if (GetVerbosity()>10) {
               SendError(kWarning, "DoSmallestAnyChanCheck",
                         "Channel [%d] has min auto correlation value [%g] "
                         "< [%g] ==> not pure thermal.",
                         ch, ac->FindMinimum(), fAutoCorrCut);
            }
            
            break;
         }
         
      } else {
         SendError(kAbortAnalysis, "Process",
                   "Could not get auto correlation spline for "
                   "channel [%d]", ch);
         ok = kFALSE;
         break;
      }
   }
   return ok;
}
