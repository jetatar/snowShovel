#include "TSnCluster.h"

#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"
#include "TAModule.h"

#include <algorithm>

ClassImp( TSnCluster );
ClassImp( TSnClusterTimes );


TSnCluster::TSnCluster( ) : fHdr(0), fClustTimes(0)
{
}



TSnCluster::TSnCluster( const Char_t* name, Double_t maxsep ) :
    TAModule(name, "Tag event clusters."), fHdr(0), fClustTimes(0)
{
    maxtmsep = maxsep;
}


TSnCluster::~TSnCluster( )
{
    delete fHdr;
}


void TSnCluster::SlaveBegin( )
{
    ReqBranch( TSnRawTreeMaker::kEHdBrNm, fHdr );
}


void TSnCluster::Process( )
{
    LoadBranch( TSnRawTreeMaker::kEHdBrNm );

    evtTmPassCut.push_back( fHdr->GetUnixTime() );

//    Printf( "Event pass CC cut: %lu", fHdr->GetUnixTime() );
}


void TSnCluster::Terminate( )
{
    UInt_t csize = 1;  // cluster size

    fClustTimes = new TSnClusterTimes;
    AddOutput( fClustTimes );

    fClustTimes->SetMaxTmSep( maxtmsep );

    // Sort time windows in ascending order
    std::sort( evtTmPassCut.begin(), evtTmPassCut.end() );

    UInt_t nclust = evtTmPassCut.size( );

    for( ULong64_t i = 1; i < nclust; i++ )
    {
//        Printf( "Checking evt %f vs evt %f", evtTmPassCut[i], 
//                                                  evtTmPassCut[i-1] );

        if( (evtTmPassCut[i] - evtTmPassCut[i-1]) < maxtmsep 
            && i < (nclust - 1) )
        {
            csize++;
//            Printf( "Separation: %f, csize: %d", 
//                evtTmPassCut[i] - evtTmPassCut[i-1], csize );
        }
        else
        {
            if( csize > 1 )
            {
//                Printf( "First evt in clust: %f, last evt in clust: %f, sep: %f",                         evtTmPassCut[i - csize], evtTmPassCut[i - 1],
//                          evtTmPassCut[i] - evtTmPassCut[i-1] );
                std::pair< Double_t, Double_t > ss;
        
                if( i < nclust - 1 )
                {
                    ss = std::make_pair( evtTmPassCut[i - csize], 
                                         evtTmPassCut[i - 1] );
                }
                else
                {
                    ss = std::make_pair( evtTmPassCut[i - csize], 
                                         evtTmPassCut[i] );
                }

                fClustTimes->Set( ss );

                csize = 1;
            }
            else
            {
//                Printf( "==> Skipping evt: %f", evtTmPassCut[i] );
            }

        }
    }
}
