#include "TSnCalibSet.h"

#include <TObjArray.h>

#include "TSnCalibObject.h"

#include <stdexcept>
#include <algorithm>

ClassImp(TSnCalibSet);

TSnCalibSet::TSnCalibSet(const Char_t* name,
                         const Char_t* title,
                         const Int_t n) :
   TNamed(name, title),
   fCalibs(new TObjArray(n)) {
   fCalibs->SetOwner(kTRUE);
}

TSnCalibSet::TSnCalibSet(const TSnCalibSet& c) :
   fCalibs(new TObjArray(c.fCalibs->GetSize())) {
   // copy ctor - a full deep copy
   TObject* x(0);
   const Int_t n = c.fCalibs->GetSize();
   for (Int_t i=0; i<n; ++i) {
      x = c.fCalibs->At(i);
      if (x!=0) {
         fCalibs->AddAt(x->Clone(), i);
      }
   }
   fCalibs->SetOwner(kTRUE);
}

TSnCalibSet::~TSnCalibSet() {
   delete fCalibs;
}

void TSnCalibSet::Swap(TSnCalibSet& a, TSnCalibSet& b) {
   std::swap(a.fCalibs, b.fCalibs);
}

void TSnCalibSet::Print(Option_t* option) const {
   TIter nc(fCalibs);
   const TSnCalibObject* cv = 0;
   while ( (cv = dynamic_cast<const TSnCalibObject*>(nc()))!=0 ) {
      cv->Print(option);
   }
}

const TSnCalibObject& TSnCalibSet::GetCalibFor(const Int_t smp) const {
   // get the calib object for the given sample
   const TSnCalibObject* co =
      dynamic_cast<const TSnCalibObject*>(fCalibs->At(GetCalibIndex(smp)));
   if (co==0) {
      throw std::out_of_range(Form("Could not get calib obj for sample [%d]",
                                   smp));
   }
   return *co;
}

TSnCalibObject& TSnCalibSet::GetCalibFor(const Int_t smp) {
   // get the calib object for the given sample
   
   // this is safe as long as fCalibs never becomes const
   return const_cast<TSnCalibObject&>(
      const_cast<const TSnCalibSet*>(this)->GetCalibFor(smp));
}

Bool_t TSnCalibSet::HasCalibsFor(const Int_t smp) const {
   const Int_t idx = GetCalibIndex(smp);
   if ( (idx>=0) && (idx<fCalibs->GetEntriesFast()) ) {
      return (0!=fCalibs->UncheckedAt(idx));
   }
   return kFALSE;
}
   
Float_t TSnCalibSet::Calibrate(const Int_t smp, const Float_t x) const {
   return GetCalibFor(smp).Calibrate(x);
}

Float_t TSnCalibSet::CalibError(const Int_t smp,
                                const Float_t x,
                                const Float_t ex) const {
   return GetCalibFor(smp).CalibError(x, ex);
}

void TSnCalibSet::CalibrateWithErr(const Int_t smp,
                                   const Float_t x, const Float_t ex,
                                   Float_t& y, Float_t& ey) const {
   return GetCalibFor(smp).CalibrateWithErr(x, ex, y, ey);
}
   
void TSnCalibSet::ChangeCalibOf(const Int_t smp, const TSnCalibObject& vals) {
   // change the calibration parameters for the given sample
   GetCalibFor(smp) = vals;
}

void TSnCalibSet::AddCalibAt(const Int_t smp, const TSnCalibObject& vals) {
   // add a calibration object for the given sample (or change the values
   // if there's already one there)
   if (HasCalibsFor(smp)) {
      ChangeCalibOf(smp, vals);
   } else {
      fCalibs->AddAt(vals.Clone(), GetCalibIndex(smp));
   }
}

void TSnCalibSet::PassCalibTo(const Int_t smp, TSnCalibObject* vals) {
   // take the calib object and pass it into the set at the given sample
   // the calib obj will now be owned by the set.
   
   fCalibs->AddAt(vals, GetCalibIndex(smp));
}

