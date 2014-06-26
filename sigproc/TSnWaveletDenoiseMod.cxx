#include "TSnWaveletDenoiseMod.h"

#include <TMath.h>
#include <TClass.h>

#include "NSnConstants.h"
#include "TSnCalWvData.h"
#include "TSnWavelet.h"
#include "TSnWaveletDaub4.h"

#include <stdexcept>

ClassImp(TSnWaveletDenoiseMod);

TSnWaveletDenoiseMod::TSnWaveletDenoiseMod() :
   fWv(0),
   fThresh(20.0 * TMath::Sqrt(2.0 * TMath::Log10(NSnConstants::kNsamps))),
   fThrType(kHardThresh) {
   // default ctor
}   

TSnWaveletDenoiseMod::TSnWaveletDenoiseMod(const Char_t* name,
                                           const Char_t* datbrnm, 
                                           const Char_t* outdatnm) :
   TSnCalDatModule(name, "denoise waveform via DWT"),
   fWv(new TSnWaveletDaub4(kTRUE)),
   fThresh(20.0 * TMath::Sqrt(2.0 * TMath::Log10(NSnConstants::kNsamps))),
   fThrType(kHardThresh),
   fDatBrNm(datbrnm),
   fOutDatNm(outdatnm) {
   // normal ctor
}

TSnWaveletDenoiseMod::~TSnWaveletDenoiseMod() {
   delete fWv;
}

void TSnWaveletDenoiseMod::SetWavelet(const TSnWavelet& wv,
                                      const Bool_t buildWavelet) {
   // copy the wavelet using Clone (streamer facility)
   delete fWv;
   fWv = dynamic_cast<TSnWavelet*>(wv.Clone());
   if (buildWavelet) {
      fWv->BuildWavelet();
   }
}

void TSnWaveletDenoiseMod::SetWavelet(const Char_t* wvClassName) {
   // set the wavelet using the specified class name
   delete fWv; fWv=0;
   TClass* c = TClass::GetClass(wvClassName);
   if (c!=0) {
      fWv = reinterpret_cast<TSnWavelet*>(c->New());
      if (fWv!=0) {
         fWv->BuildWavelet();
      } else {
         throw std::runtime_error(Form("Unable to construct [%s] wavelet.",
                                       wvClassName));
      }
   } else {
      throw std::runtime_error(Form("Invalid wavelet class name [%s].",
                                    wvClassName));
   }
}

void TSnWaveletDenoiseMod::SlaveBegin() {
   if (fWv!=0) {
      ReqCalData(fDatBrNm.Data(), fDat);
   } else {
      SendError(kAbortAnalysis, "SlaveBegin",
                "No wavelet available for transform!");
   }
}

void TSnWaveletDenoiseMod::Process() {
   LoadCalData(fDatBrNm.Data());
   if (fDat!=0) {
      
      TSnCalWvData* denoised = new TSnCalWvData(fOutDatNm.Data(),
                                                Form("denoised [%s] with [%s]",
                                                     fDatBrNm.Data(),
                                                     fWv->GetLabel()));
      
      Float_t trns[NSnConstants::kNsamps];
      for (UChar_t ch=0; ch<NSnConstants::kNchans; ++ch) {
         const Float_t* const d = fDat->GetData(ch);
         fWv->Transform(d, NSnConstants::kNsamps, trns);
         Float_t* t = trns;
         for (UChar_t sm=0; sm<NSnConstants::kNsamps; ++sm, ++t) {
            if (TMath::Abs(*t) < fThresh) {
               *t = 0.0;
            } else if (fThrType==kSoftThresh) {
               if (*t<0) {
                  *t += fThresh;
               } else {
                  *t -= fThresh;
               }
            }
         }
         fWv->Inverse(trns, NSnConstants::kNsamps, denoised->GetData(ch));
      }
      
      AddObjThisEvt(denoised);
      
   } else {
      SendError(kAbortModule, "Process",
                "Could not load data [%s]",
                fDatBrNm.Data());
   }

}
