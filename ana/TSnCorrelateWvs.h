#ifndef SNS_TSnCorrelateWvs
#define SNS_TSnCorrelateWvs

#include "TAModule.h"
#include "TSnRecoResult.h"
#include "TArrayF.h"
#include "NSnConstants.h"
#include "TArrayI.h"

#include <vector>

class TSnCalWvData;
class TH2F;
class TSnEventMetadata;
class TSnEventHeader;
class TObjArray;
class TSnPowerReading;
class TSnWindData;
class TSnCalWvData;

class TSnCorrCoef : public TSnRecoResult
{
    private:
        TArrayF corrcoef;

    public:
        TSnCorrCoef( ) : corrcoef( 0 ){ }
        TSnCorrCoef( const Char_t* name ) : TSnRecoResult( name, "" ), 
            corrcoef(NSnConstants::kNchans )
        { }

        // a virtual function is one whose behavior can be overwritten
        // within an inheriting class 
        // TSnRecoResult::CopyFrom has the following format:
        // virtual void CopyFrom( ) = 0 this defines a pure virtual function
        // A pure virtual function is a virtual function that is used when the 
        // designer of the class wants to force derived classes to override the 
        // function and provide their own implementation.
        //
        //
        // const TSnRecoResult& x make sure that the reference variable passed
        // to x cannot have its value changed
        virtual void CopyFrom( const TSnRecoResult& x )
        {
            *this = dynamic_cast< const TSnCorrCoef& >( x );
        }
        virtual ~TSnCorrCoef( ){ }

    friend class TSnCorrelateWvs;

    ClassDef( TSnCorrCoef, 1 );
};


class TSnClust : public TSnRecoResult
{
    private:
        TArrayI cluster;

    public:
        TSnClust( ) : cluster( 0 ){ }
        TSnClust( const Char_t* name ) : TSnRecoResult( name, "" ),
                  cluster( NSnConstants::kNchans )
        { }

    virtual void CopyFrom( const TSnRecoResult& x )
    {
        *this = dynamic_cast< const TSnClust& >( x );
    }
    
    virtual ~TSnClust( ){ }

    friend class TSnCorrelateWvs;

    ClassDef( TSnClust, 1 );
};


class TSnCorrelateWvs : public TAModule
{
    public:
        TSnCorrelateWvs( );
        TSnCorrelateWvs( const Char_t* name, std::vector< Float_t > wvtmplt, 
                         const Char_t* fftNm );
        virtual ~TSnCorrelateWvs( );

    private:
        TSnCalWvData*           fData;      // event data
        TSnEventMetadata*       fMeta;
        TSnEventHeader*         fHdr;
        TSnPowerReading*        fPwr;
        TSnWindData*            fWind;

        TString                 fFFT;
        std::vector< Float_t >  wvtp;
        TSnCorrCoef*            fCC;        // correlation coefficients
        TH2F*                   hCC;
        TObjArray*              hPrCC;
        TH2F*                   hpcNS;      // parallel channels NS
        TH2F*                   hpcEW;
        TH2F*                   hCCAvg;     // Avg C.C. of parallel chans
        TH2F*                   hVvsCC;
        TH2F*                   hWndvsCC;
        TH2F*                   hPwrvsCC;
        TH2F*                   hPwrvsWnd;
        TObjArray*              hPwr;
        TSnClust*               fClust;
        TH2F*                   hAvgCCvT;    

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );
    
    ClassDef( TSnCorrelateWvs, 1 );
};

#endif
