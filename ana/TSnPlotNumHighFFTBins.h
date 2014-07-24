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
        TSnCalWvData*       fData; 
        TObjArray*          hNHighPks;
        TH1F*               hNHighPksAllCh;
//        TH1F*               hNChWithHighPks;

    public:
        TSnPlotNumHighFFTBins( ){ }
        TSnPlotNumHighFFTBins( const Char_t* name );

        
        virtual ~TSnPlotNumHighFFTBins( ){ }

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );        


    ClassDef( TSnPlotNumHighFFTBins, 1 );
};


#endif // SNS_TSnPlotNumHighFFTBins

