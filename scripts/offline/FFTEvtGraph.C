#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"

#include "TSnCalWvData.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnCalFFTData.h"


static const Char_t* inflnm     = 
                    "/w2/jtatar/Analysis/Stn3/FPNSubTree.nt.ic3.all.root";
static const Char_t* outflnm    = "fftevtgraph.root";


void FFTEvtGraph( void )
{
    TFile* fin  = new TFile( inflnm );
    TTree* tr   = (TTree*)fin->Get( "FpnSubTree" );

    TSnCalWvData* wv        = new TSnCalWvData;
    TSnCalFFTData* fft;
    TSnEventHeader* fHdr    = new TSnEventHeader;
    TSnEventMetadata* fMeta = new TSnEventMetadata;

    tr->SetBranchAddress( "FPNSubData.", &wv );
    tr->SetBranchAddress( "EventHeader.", &fHdr );
    tr->SetBranchAddress( "EventMetadata.", &fMeta );

    TGraph* g;

    ULong64_t nevts = tr->GetEntries( );

    for( ULong64_t i = 0; i < nevts; i++ )
    {
        tr->GetEntry( i );        

        if( fMeta->GetRunNum() == 406 && fHdr->GetEvtNum() == 34851 )
        {
            fft = new TSnCalFFTData( "", "", *wv );

            g = fft->NewFFTGraph( 3 );

            break;
        }
    }

    TFile* fout = new TFile( outflnm, "recreate" );
    fout->cd( );
    g->Write( "gFFTBkgd" );
    fout->Close( );
}
