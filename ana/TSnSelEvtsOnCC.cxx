#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"

#include "TSnSelEvtsOnCC.h"
#include "TSnCorrelateWaves.h"
#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"
#include "TSnEventMetadata.h"

#include "TH2F.h"

ClassImp( TSnSelEvtsOnCC );


TSnSelEvtsOnCC::TSnSelEvtsOnCC( const Char_t* name, 
    std::vector< Float_t > tmpltsig ) : TAModule( name, "TSnCorrelateWaves" ),
    fData(0), fHdr(0), hCCvT(0), hLrgCCvT(0), hCC(0), fMeta(0), hCCbyCh(0)
{
    tmpltwv = tmpltsig;
    
    hCCbyCh = new TObjArray( NSnConstants::kNchans );
}


void TSnSelEvtsOnCC::SlaveBegin( )
{
    ReqBranch( TSnSaveCalibDataMod::kAmpOutDatBrNm, fData );
    ReqBranch( TSnRawTreeMaker::kEHdBrNm, fHdr );
    ReqBranch( TSnRawTreeMaker::kEMtBrNm, fMeta );

    hCC         = new TH1F( "hCC", "", 110, -0.005, 1.005 );

    hCCvT       = new TH2F( "hCCvT", "", 16245, 1389000000, 1395500000, 
                                   110, -0.005, 1.005 );

    hLrgCCvT    = new TH2F( "hLrgCCvT", "", 16245, 1389000000, 1395500000,
                                   110, -0.005, 1.005 );
    
    hCCvT   ->SetTitle( ";Time (UTC); <#Chi>" );
    hLrgCCvT->SetTitle( ";Time (UTC); <#Chi> > 0.4" );

//    hCCvT       = new TH2F( "hCCvT", "", 22400,
//            1388779520, 1395500000, 110, -0.005, 1.005 );

//    hLrgCCvT    = new TH2F( "hLrgCCvT", "", 22400,
//            1388779520, 1395500000, 110, -0.005, 1.005 );
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* hccbych = new TH1F( Form("hCCbyCh%d", ch), "", 
                                                            110, -0.005, 1.005 );
        hCCbyCh->AddAt( hccbych, ch );
    }

    hCC->SetName( "hCC" );
    hCCvT->SetName( "hCCvT" );
    hLrgCCvT->SetName( "hLrgCCvT" );
    hCCbyCh->SetName( "hCCbyCh" );


    AddOutput( hCC );
    AddOutput( hCCvT );
    AddOutput( hLrgCCvT );
    AddOutput( hCCbyCh );
}


void TSnSelEvtsOnCC::Process( )
{
    LoadBranch( TSnSaveCalibDataMod::kAmpOutDatBrNm );
    LoadBranch( TSnRawTreeMaker::kEHdBrNm );
    LoadBranch( TSnRawTreeMaker::kEMtBrNm );

    TSnCorrelateWaves* cwvs = new TSnCorrelateWaves( "corrcoef", fData,
                                                                tmpltwv );
    cwvs->Correlate( );

//    Printf( "%d %d", fMeta->GetRunNum(), fHdr->GetEvtNum() );

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
//        hCCvT->Fill( fHdr->GetUnixTime(), cwvs->GetCC(ch) );
        TH1F* hccbych = dynamic_cast<TH1F*>( hCCbyCh->At(ch) );
        hccbych->Fill( cwvs->GetCC(ch) );
    }

    hCC->Fill( cwvs->GetAvgCC() );
    hCCvT->Fill( fHdr->GetUnixTime(), cwvs->GetAvgCC() );
    

    if( cwvs->GetAvgCC() < 0.45 )
    {
        SkipEvent( );
    }
    else
    {
        hLrgCCvT->Fill( fHdr->GetUnixTime(), cwvs->GetAvgCC() );

//        Printf( "%d %d %f %f %f %f %f", fMeta->GetRunNum(), fHdr->GetEvtNum(),
//            cwvs->GetCC(0), cwvs->GetCC(1), cwvs->GetCC(2), cwvs->GetCC(3),
//            cwvs->GetAvgCC() );
    }

/*    bool isbad = kTRUE;

    // Loop through CC of all channels of an event, 
    // if CC > 0.6, pass event to TSnCluster for clustering.
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        hCCvT->Fill( fHdr->GetUnixTime(), cwvs->GetCC(ch) );

        if( cwvs->GetCC(ch) > 0.6 )
        {
//            Printf( "Passing event: %lu, %f", fHdr->GetUnixTime(), 
//                                    cwvs->GetCC(ch) );
            hLrgCCvT->Fill( fHdr->GetUnixTime(), cwvs->GetCC(ch) );

            isbad = kFALSE;

            break;
        }
    }
    
    if( isbad ) 
    {
        SkipEvent( );
        
        if( GetVerbosity() > 9 ) 
        {
            SendError(kWarning, "Process",
                      "Rejecting event due to CC" );
        }
    }
*/
    AddObjThisEvt( cwvs );
}
