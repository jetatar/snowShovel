#ifndef SNS_TSnCluster
#define SNS_TSnCluster

#include "NSnConstants.h"
#include "TAModule.h"

#include <vector>
#include <utility>

class TSnEventHeader; 
class TObject;


class TSnClusterTimes : public TObject
{
    private:
        std::vector< std::pair<Double_t, Double_t> > fTimes;
        Double_t fMaxTmSep;
    
    public:
        TSnClusterTimes( ){ }
        virtual ~TSnClusterTimes( ){ }

        void Set( std::pair<Double_t, Double_t> ss )
        {
            fTimes.push_back( ss );
        }           
        std::vector< std::pair<Double_t, Double_t> >& Get( void )
        {
            return( fTimes );
        }
        Double_t GetStartTm( UInt_t i )
        {
            return( fTimes.at(i).first );
        }
        Double_t GetStopTm( UInt_t i )
        {   
            return( fTimes.at(i).second );
        }
        Double_t GetMaxTmSep( void )
        {
            return( fMaxTmSep );
        }
        void SetMaxTmSep( Double_t mxtm )
        {
            fMaxTmSep = mxtm;
        }

    ClassDef( TSnClusterTimes, 1 );
};


class TSnCluster : public TAModule 
{
    private:
        TSnEventHeader*         fHdr;
        Double_t                maxtmsep;
        std::vector< Double_t > evtTmPassCut;        
        TSnClusterTimes*        fClustTimes;

    public:
        TSnCluster( );
        TSnCluster( const Char_t* name, Double_t maxsep );
        
        virtual ~TSnCluster( );


    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );

    ClassDef( TSnCluster, 1 );
};

#endif // SNS_TSnCluster

