#include "TSnAxisDetails.h"

// ROOT includes
#include "TROOT.h"
#include "Riostream.h"
#include "TIterator.h"
#include "TError.h"

ClassImp(TSnAxisDetails::TAxDet)
ClassImp(TSnAxisDetails)
////////////////////////////////////////////////////////////////////////////////
//
// $Id: TSnAxisDetails.cxx,v 1.1 2007/05/14 17:25:32 creed Exp $
//
// Stores the number of bins, the min & the max for a collection of axes.
// Lookup is done by name in a hash list (so it is fast).
//
// Useful when making many histograms with the same axis details.
//
// Normal usage:
//    TSnAxisDetails axes("DataQualityAxes","Axes for data quality plots");
//    axes.AddAxis("Time",   100, 10, 220);
//    axis.AddAxis("Charge", 100,  0,  35);
//    ...
//    Int_t  xbins, ybins;
//    Axis_t xmin, xmax, ymin, ymax;
//    axes.GetAxis("Time",   xbins, xmin, xmax);
//    axes.GetAxis("Charge", ybins, ymin, ymax);
//    TH2F* h2 = new TH2F("h2","Charge Vs Time;Time;Charge",
//                        xbins, xmin, xmax,
//                        ybins, ymin, ymax);
//
////////////////////////////////////////////////////////////////////////////////


TSnAxisDetails::TSnAxisDetails() {
   // default ctor
   fDetailList.SetOwner(kTRUE);
}

TSnAxisDetails::TSnAxisDetails(const Char_t* name, const Char_t* title) :
   TNamed(name, title) {
   // normal ctor
   fDetailList.SetOwner(kTRUE);
}

TSnAxisDetails::~TSnAxisDetails() {
   // dtor
   fDetailList.Clear();
}

void TSnAxisDetails::TAxDet::Print(Option_t *option) const {
   TROOT::IndentLevel();
   std::cout << "Axis [" << GetName() << "]: ["
             << fNbins << "] bins from ("
             << fMin << ", " << fMax << ")."
             << std::endl;
}

Bool_t TSnAxisDetails::AddAxis(const Char_t* name,
                               const Int_t   nbins,
                               const Axis_t  axMin, const Axis_t axMax) {
   // add the details of an axis
   // returns false on failure
   // will fail if the axis details already exist
   
   TAxDet* det = dynamic_cast<TAxDet*>(fDetailList.FindObject(name));
   if (det==0) {
      det = new TAxDet(name, nbins, axMin, axMax);
      fDetailList.Add(det);
      return kTRUE;
   } else {
      Error("AddAxis",
            "Details for axis [%s] already exist."
            "Use UpdateAxis instead.",
            name);
   }
   return kFALSE;
}

Bool_t TSnAxisDetails::GetAxis(const Char_t* name,
                               Int_t&  nbins,
                               Axis_t& axMin, Axis_t& axMax) const {
   // copy the details of an axis into 'nbins', 'axMin' and 'axMax'
   // returns false on failure (i.e. the requested axis is not found)
   // sets nbins, axMin and axMax to 0 on fail
   
   TAxDet* det = dynamic_cast<TAxDet*>(fDetailList.FindObject(name));
   if (det==0) {
      Error("GetAxis",
            "Details for axis [%s] not found.",
            name);
      nbins = 0;
      axMin = axMax = 0;
   } else {
      nbins = det->fNbins;
      axMin = det->fMin;
      axMax = det->fMax;
      return kTRUE;
   }
   return kFALSE;
}

Bool_t TSnAxisDetails::AxisExists(const Char_t* name) const {
   // return true only if details for the specified axis already exist
   
   TObject* obj = fDetailList.FindObject(name);
   return (obj!=0);
   
}

Bool_t TSnAxisDetails::UpdateAxis(const Char_t* name,
                                  const Int_t   nbins,
                                  const Axis_t  axMin, const Axis_t axMax) {
   // update the details of an axis
   // returns false on failure
   // will fail if the axis details do not yet exist
   
   TAxDet* det = dynamic_cast<TAxDet*>(fDetailList.FindObject(name));
   if (det==0) {
      Error("UpdateAxis",
            "Details for axis [%s] not found.",
            name);
   } else {
      det->fNbins = nbins;
      det->fMin   = axMin;
      det->fMax   = axMax;
      return kTRUE;
   }
   return kFALSE;
}

Bool_t TSnAxisDetails::RemoveAxis(const Char_t* name) {
   // remove the axis details
   // returns false on failure (i.e. the requested axis is not found)
   
   TAxDet* det = dynamic_cast<TAxDet*>(fDetailList.FindObject(name));
   if (det==0) {
      Error("RemoveAxis",
            "Details for axis [%s] not found.",
            name);
   } else {
      TObject* obj = fDetailList.Remove(det);
      if (obj==0) {
         obj = fDetailList.RemoveSlow(det);
         if (obj==0) {
            Error("RemoveAxis",
                  "Unable to remove axis [%s].",
                  det->GetName());
         }
      }
      delete det;
      return kTRUE;
   }
   return kFALSE;
}

void TSnAxisDetails::ls(Option_t* option) const {
   // print the info for all axes
   
   Info("ls","Axis details:");
   
   TROOT::IncreaseDirLevel();
   
   TIter na(fDetailList.MakeIterator());
   TObject* obj;
   while ( (obj = na()) != 0 ) {
      obj->Print(option);
   }
   
   TROOT::DecreaseDirLevel();
   
}
