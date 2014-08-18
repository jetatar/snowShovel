#ifndef SNS_TSnPlotNumHighFFTBins
#define SNS_TSnPlotNumHighFFTBins

#include "TAModule.h"

class TSnCalWvData;
class TSnEventHeader;
class TSnEventMetadata;
class TH1F;
class TH2F;

class TSnPlotNumHighFFTBins : public TAModule
{
    private:
        TSnCalWvData*   fData; 
        TObjArray*      hNHighPks;
        TH1F*           hNHighPksAllCh;
//        TH1F*               hNChWithHighPks;
        TObjArray*      hNHighPksVsMax;
        TH2F*           hNHighPksVsMaxAllCh;
        TH1F*           fhMinNHM;            // distribution of the minimum NHM on any channel
        TString         fPlotTag;            // prepend plot names with this string (default: "")

    public:
        TSnPlotNumHighFFTBins( ){ }
        TSnPlotNumHighFFTBins( const Char_t* name,
                               const Char_t* plotTag=NULL);
        virtual ~TSnPlotNumHighFFTBins( );

        TString      GetPlotTag() const { return fPlotTag; }
        void         SetPlotTag(const Char_t* t) { fPlotTag = t; }
   
    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );        


    ClassDef( TSnPlotNumHighFFTBins, 2 ); // module to make NHM (num bins in fft above half max) distribution plots
};


#endif // SNS_TSnPlotNumHighFFTBins

