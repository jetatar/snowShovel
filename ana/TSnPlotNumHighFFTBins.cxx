#include "TSnPlotNumHighFFTBins.h"
#include "TSnCalFFTData.h"
#include "TSnNumHighFFTBinsMod.h"
#include "TSnSaveCalibDataMod.h" 
#include "TSnCalWvData.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnRawTreeMaker.h"
#include "TH1F.h"
#include "TH2F.h"

ClassImp( TSnPlotNumHighFFTBins );
ClassImp( TSnNumHighFFTBinsMod );


TSnPlotNumHighFFTBins::TSnPlotNumHighFFTBins( const Char_t* name ) :
    TAModule(name, "TSnPlotNumHighFFTBins" ), fData(0), fHdr(0), fMeta(0),
    hNHighPks(0), hNHighPksAllCh(0)//, hNChWithHighPks(0)
{
    hNHighPks   = new TObjArray( NSnConstants::kNchans );
}


void TSnPlotNumHighFFTBins::SlaveBegin( )
{
    ReqBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm, fData );
    ReqBranch( TSnRawTreeMaker::kEHdBrNm, fHdr );
    ReqBranch( TSnRawTreeMaker::kEMtBrNm, fMeta );

    hNHighPksAllCh  = new TH1F( "hNHighPksAllCh", "", 61, -0.5, 60.5 );
//    hNChWithHighPks = new TH1F( "hNChWithHighPks", "", 5, -0.5, 4.5 ); 
    
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* hhigh = new TH1F( Form("hNHighPks%d", ch), "", 61, -0.5, 60.5 );

        hNHighPks->AddAt( hhigh, ch );
    }

    AddOutput( hNHighPks );
    AddOutput( hNHighPksAllCh );
}


void TSnPlotNumHighFFTBins::Process( )
{
    LoadBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm );
    LoadBranch( TSnRawTreeMaker::kEHdBrNm );
    LoadBranch( TSnRawTreeMaker::kEMtBrNm );

    TSnCalFFTData* fft              = new TSnCalFFTData( "TSnCalFFTData", "", 
                                                                    *fData );
    TSnNumHighFFTBinsMod* nhFFTBins = new TSnNumHighFFTBinsMod( "highfft", fft );

    nhFFTBins->Calculate( );

    UInt_t nHighBins[NSnConstants::kNchans];
    UChar_t totHighBins = 0;

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        nHighBins[ch] = nhFFTBins->GetNumHighBins( ch );
        hNHighPksAllCh->Fill( nHighBins[ch] );

        TH1F* hhigh = dynamic_cast<TH1F*>( hNHighPks->At(ch) );
        hhigh->Fill( nHighBins[ch] );

//        if( nHighBins[ch] >= 4 )
//        {
//          totHighBins++;
//        }
    }

    if(// (nHighBins[0] >= 6 && nHighBins[2] >= 6)  
        (nHighBins[1] >= 4 && nHighBins[3] >= 4) 
      )
    {
        totHighBins++;
    }

//    Printf( "%d %d %d %d %d %d", fMeta->GetRunNum(), fHdr->GetEvtNum(),
//                    nHighBins[0], nHighBins[1], nHighBins[2], nHighBins[3] );

    // < 0 ... >= all events pass
    // < 1 ... >= 1 channels pass
    // < 2 ... >= 2 channels pass
    // < 3 ... >= 3 channels pass
    // < 4 ... >= 4 channels pass
    if( totHighBins < 1 )
    {
//        Printf( "Event skipped: %d %d", fMeta->GetRunNum(), fHdr->GetEvtNum() );
//        Printf( "Skipping event." );
        SkipEvent( );
    }
}


void TSnPlotNumHighFFTBins::Terminate( )
{
}
