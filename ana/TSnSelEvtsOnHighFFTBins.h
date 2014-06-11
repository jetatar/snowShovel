#ifndef SNS_TSnSelEvtsOnHighFFTBins
#define SNS_TSnSelEvtsOnHighFFTBins

#include "TAModule.h"

class TSnCalWvData;
class TSnEventHeader;
class TSnEventMetadata;
class TH1F;
class TH2F;

class TSnSelEvtsOnHighFFTBins : public TAModule
{
    private:
        TSnCalWvData*       fData; 
        TSnEventHeader*     fHdr;
        TSnEventMetadata*   fMeta;
        TObjArray*          hNHighPks;
        TH1F*               hNHighPksAllCh;
//        TH1F*               hNChWithHighPks;

    public:
        TSnSelEvtsOnHighFFTBins( ){ }
        TSnSelEvtsOnHighFFTBins( const Char_t* name );

        
        virtual ~TSnSelEvtsOnHighFFTBins( ){ }

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );        


    ClassDef( TSnSelEvtsOnHighFFTBins, 1 );
};


#endif // SNS_TSnSelEvtsOnHighFFTBins
