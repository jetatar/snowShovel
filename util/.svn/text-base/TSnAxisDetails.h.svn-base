#ifndef SNS_TSnAxisDetails
#define SNS_TSnAxisDetails

// ROOT inlcudes
#ifndef ROOT_TNamed
#include "TNamed.h"
#endif
#ifndef ROOT_THashTable
#include "THashTable.h"
#endif
#ifndef ROOT_Htypes
#include "Htypes.h" // to use Axis_t
#endif

////////////////////////////////////////////////////////////////////////////////
//
// $Id$
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

class TSnAxisDetails : public TNamed {

 public:
   struct TAxDet : TNamed {
      // little struct to store histogram axis details
      Int_t fNbins;
      Axis_t fMin, fMax;
      TAxDet(const Char_t* name,
             const Int_t nb, const Axis_t mn, const Axis_t mx) :
         TNamed(name,name), fNbins(nb), fMin(mn), fMax(mx) {}
      virtual ~TAxDet() {}
      void Print(Option_t *option) const;
      ClassDef(TAxDet, 1)
   };
   
 private:  
   THashTable        fDetailList;   //  the list of axis details
 
 public:
   TSnAxisDetails();
   TSnAxisDetails(const Char_t* name, const Char_t* title);
   virtual ~TSnAxisDetails();
   
   virtual Bool_t    AddAxis(const Char_t* name,
                             const Int_t   nbins,
                             const Axis_t  axMin, const Axis_t axMax);
   
   virtual Bool_t    GetAxis(const Char_t* name,
                             Int_t&  nbins,
                             Axis_t& axMin, Axis_t& axMax) const;
   
   virtual Bool_t    UpdateAxis(const Char_t* name,
                                const Int_t   nbins,
                                const Axis_t  axMin, const Axis_t axMax);
   
           Bool_t    AxisExists(const Char_t* name) const;
           Bool_t    IsEmpty() const { return fDetailList.IsEmpty(); }
   
   virtual Bool_t    RemoveAxis(const Char_t* name);
   
   virtual void      Clear(Option_t* option) { fDetailList.Clear(option); }

   virtual void      ls(Option_t* option) const;

   ClassDef(TSnAxisDetails, 1) // stores histogram axis details
};

#endif //SNS_TSnAxisDetails
