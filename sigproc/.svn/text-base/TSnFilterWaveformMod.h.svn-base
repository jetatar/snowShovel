#ifndef SNS_TSnFilterWaveformMod
#define SNS_TSnFilterWaveformMod

#include <TString.h>
class TF1;

#include "TSnCalDatModule.h"
class TSnCalWvData;


class TSnFilterWaveformMod : public TSnCalDatModule {
 public:
   static const UInt_t kDebugFilter; // verbosity level at which filter debugging is turned on (i.e. plots are drawn in each event)

 private:
   TString       fDatBrNm;  // name of the input branch containing the data (fpn sub or amp out)
   TString       fFltDatNm; // name of the output filtered data object added to the event
   TF1*          fFilter;   // the filter function (indep var must have same units as FFT of fDatBrNm) (owned)
   
   // branch data
   TSnCalWvData* fDat;      //! the fpn sub or amp out data to filter
   
 protected:
   virtual void  SlaveBegin();
   virtual void  Process();
   
 public:
   TSnFilterWaveformMod() : fFilter(0), fDat(0) {}
   TSnFilterWaveformMod(const Char_t* name, 
                        const Char_t* datBrNm,
                        const Char_t* fltDtNm,
                        const TF1& filter);
   virtual ~TSnFilterWaveformMod();
   
   const TF1*    GetFilter() const { return fFilter; }
         TF1*    GetFilter()       { return fFilter; }
   TString       GetDataBrName() const { return fDatBrNm; }
   TString       GetFltDatName() const { return fFltDatNm; }
   
   void          SetFilter(const TF1& f);
   void          SetDataBrName(const Char_t* n) { fDatBrNm = n; }
   void          SetFltDatName(const Char_t* n) { fFltDatNm = n; }
   
   ClassDef(TSnFilterWaveformMod, 1); // pass the fpn sub or amp out waveforms through a filter
};

#endif // SNS_TSnFilterWaveformMod
