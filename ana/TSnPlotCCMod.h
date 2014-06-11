#ifndef SNS_TSnPlotCCMod
#define SNS_TSnPlotCCMod

#include "TAModule.h"
#include "TFile.h"
#include "TTree.h"

#include "NSnConstants.h"

#include <vector>
#include <utility>

class TSnEventHeader;
class TSnCalWvData;
class TH2F;
class TSnEventMetadata;
class TH1F;
class TObjArray;


class TSnRunEvtOut : public TObject
{
    private:
        std::vector< std::pair<UInt_t, UInt_t> > fRunEvt;
    
    public:
        TSnRunEvtOut( ) { }
        virtual ~TSnRunEvtOut( ){ }

        void Set( std::pair<UInt_t, UInt_t> ss )
        {
            fRunEvt.push_back( ss );
        }           
        std::vector< std::pair<UInt_t, UInt_t> >& Get( void )
        {
            return( fRunEvt );
        }
        UInt_t GetRunNum( UInt_t i )
        {
            return( fRunEvt.at(i).first );
        }
        UInt_t GetEvtNum( UInt_t i )
        {   
            return( fRunEvt.at(i).second );
        }
        UInt_t Size( void )
        {
            return( fRunEvt.size() );
        }

    ClassDef( TSnRunEvtOut, 1 );
};


class TSnPlotCCMod : public TAModule
{
    private:
        TSnEventHeader*         fHdr;
        TH2F*                   hAvgCCvT;    
        TSnEventMetadata*       fMeta;
        TSnRunEvtOut*           fEvtIdOut;
        TH1F*                   hCC;
        TSnCalWvData*           fData;
        TObjArray*              hFFTPeak;
        TObjArray*              hNHighPks;
        TH1F*                   hNHighPksAllCh;
        TObjArray*              hcc;
        TObjArray*              hccinv;

    public:
        TSnPlotCCMod( ) : fHdr(0), hAvgCCvT(0), fMeta(0), fEvtIdOut(0), 
            fData(0), hFFTPeak(0), hNHighPks(0), hNHighPksAllCh(0), hcc(0),
            hccinv(0)
        { }
        TSnPlotCCMod( const Char_t* name ) : TAModule( name, "TSnPlotCCMod" ),
            fHdr(0), hAvgCCvT(0), fMeta(0), fEvtIdOut(0), hCC(0), fData(0),
            hFFTPeak(0), hNHighPks(0), hNHighPksAllCh(0), hcc(0), hccinv(0)
        {
            hFFTPeak    = new TObjArray( NSnConstants::kNchans ); 
            hNHighPks   = new TObjArray( NSnConstants::kNchans );
            hcc         = new TObjArray( NSnConstants::kNchans );
            hccinv      = new TObjArray( NSnConstants::kNchans );
        }       

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );

    ClassDef( TSnPlotCCMod, 1 );
};


#endif // SNS_TSnPlotCCMod

