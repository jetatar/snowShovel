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
#include "TMath.h"
#include "TGraph.h"

ClassImp( TSnPlotNumHighFFTBins );
ClassImp( TSnNumHighFFTBinsMod );


TSnPlotNumHighFFTBins::TSnPlotNumHighFFTBins( const Char_t* name,
                                              const Char_t* plotTag ) :
    TAModule(name, "TSnPlotNumHighFFTBins" ), fData(0), hNHighPks(0), 
    hNHighPksAllCh(0), hNHighPksVsMax(0), hNHighPksVsMaxAllCh(0),
    //, hNChWithHighPks(0), 
    fhMinNHM(0),
    fPlotTag(plotTag)
{
    hNHighPks       = new TObjArray( NSnConstants::kNchans );
    hNHighPksVsMax  = new TObjArray( NSnConstants::kNchans );
    
    // make them delete the histograms they contain when the
    // arrays get deleted
    hNHighPks->SetOwner(kTRUE);
    hNHighPksVsMax->SetOwner(kTRUE);
}

TSnPlotNumHighFFTBins::~TSnPlotNumHighFFTBins() {
   // cleanup!
   delete hNHighPks;
   delete hNHighPksVsMax;
   delete hNHighPksAllCh;
   delete hNHighPksVsMaxAllCh;
   delete fhMinNHM;
}


void TSnPlotNumHighFFTBins::SlaveBegin( )
{
//    ReqBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm, fData );
    ReqBranch( TSnSaveCalibDataMod::kAmpOutDatBrNm, fData );

    hNHighPksAllCh      = new TH1F( Form("%shNHighPksAllCh",fPlotTag.Data()),
                                    "", 61, -0.5, 60.5 );
    hNHighPksVsMaxAllCh = new TH2F( Form("%shNHighPksVsMaxAllCh",fPlotTag.Data()),
                                    "", 
            TSnCalFFTData::kFftPts + 1, -0.5, TSnCalFFTData::kFftPts + 0.5,
            5001, -0.5, 5000.5 );
//    hNChWithHighPks = new TH1F( "hNChWithHighPks", "", 5, -0.5, 4.5 ); 

    fhMinNHM            = new TH1F(Form("%sfhMinNHM",fPlotTag.Data()),
                                   ";Min num high FFT bins on any chan"
                                   ";Events",
                                   61, -0.5, 60.5);
    
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* hhigh = new TH1F( Form("%shNHighPks%d", fPlotTag.Data(), ch), "",
            TSnCalFFTData::kFftPts + 1, -0.5, TSnCalFFTData::kFftPts + 0.5 );
//        TH1F* hhigh = new TH1F( Form("hNHighPks%d", ch), "", 61, -0.5, 60.5 );
        TH2F* hhivsmax = new TH2F( 
            Form("%shNHighPksVsMax%d", fPlotTag.Data(), ch), "", 
            TSnCalFFTData::kFftPts + 1, -0.5, TSnCalFFTData::kFftPts + 0.5,
            5001, -0.5, 5000.5 );
                
        hNHighPks->AddAt( hhigh, ch );
        hNHighPksVsMax->AddAt( hhivsmax, ch );
    }

    hNHighPks->SetName( "hNHighPks" );
    hNHighPksVsMax->SetName( "hNHighPksVsMax" );

    AddOutput( hNHighPks );
    AddOutput( hNHighPksAllCh );
    AddOutput( hNHighPksVsMax );
    AddOutput( hNHighPksVsMaxAllCh );
    AddOutput( fhMinNHM );
}


void TSnPlotNumHighFFTBins::Process( )
{
//    LoadBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm );
    LoadBranch( TSnSaveCalibDataMod::kAmpOutDatBrNm );

    TSnCalFFTData* fft              = 
                            new TSnCalFFTData( "TSnCalFFTData", "", *fData );

    TSnNumHighFFTBinsMod* nhFFTBins = 
                            new TSnNumHighFFTBinsMod( "highfft", fft );

    nhFFTBins->Calculate( );


    UInt_t nHighBins[NSnConstants::kNchans];
    UChar_t totHighBins = 0;

    //for( UChar_t ch = 0; ch < 1; ch++ )
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TGraph* gMax    = fData->NewGraphForChan( ch );
        Float_t max     = TMath::MaxElement( NSnConstants::kNsamps, 
                                                                gMax->GetY() );
        UInt_t nhibins  = nhFFTBins->GetNumHighBins( ch );
        nHighBins[ch]   = nhibins;

        hNHighPksAllCh->Fill( nHighBins[ch] );

        TH1F* hhigh = dynamic_cast<TH1F*>( hNHighPks->At(ch) );
        hhigh->Fill( nHighBins[ch] );

        TH2F* hhivsmax = dynamic_cast<TH2F*>( hNHighPksVsMax->At(ch) );
        hhivsmax->Fill( nhibins, max );

        hNHighPksVsMaxAllCh->Fill( nhibins, max );
//        if( nHighBins[ch] >= 4 )
//        {
//          totHighBins++;
//        }
    }

    fhMinNHM->Fill( TMath::MinElement( NSnConstants::kNchans, nHighBins) );

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
    
    // cleanup!
    delete fft;
    delete nhFFTBins;
    
}


void TSnPlotNumHighFFTBins::Terminate( )
{
}
