#include "TSnPlotCCMod.h"

#include "TAModule.h"
#include "NSnConstants.h"
#include "TSnRawTreeMaker.h"
#include "TSnCorrelateWaves.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"
#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnCalFFTData.h"

#include "TH2F.h"
#include "TH1F.h"

#include <utility>
#include <algorithm>
#include <iterator>


ClassImp( TSnPlotCCMod ); 
ClassImp( TSnRunEvtOut );

static const Float_t kDelta     = 1. / 1.92;
static const Int_t kHalfsamps   = NSnConstants::kNsamps / 2;
static const Float_t kfincr     = 1. /( kDelta * NSnConstants::kNsamps );


void TSnPlotCCMod::SlaveBegin( )
{
    ReqBranch( TSnRawTreeMaker::kEHdBrNm, fHdr );
    ReqBranch( TSnRawTreeMaker::kEMtBrNm, fMeta );
    ReqBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm, fData );

    hAvgCCvT    = new TH2F( "hAvgCCvT", "", 16245,
            1389000000, 1395500000, 110, -0.005, 1.005 );

    hCC         = new TH1F( "hCC", "", 110, -0.005, 1.005 );

    hAvgCCvT->SetTitle( ";Time (UTC); <#Chi>" );
//    hAvgCCvT    = new TH2F( "hAvgCCvT", "", 22400,
//            1388779520, 1395500000, 110, -0.005, 1.005 );

    hNHighPksAllCh = new TH1F( "hNHighPksAllCh", "", 31, -0.5, 30.5 ); 

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* hpeak = new TH1F( Form("hFFTPeak%d", ch), "", 
                        kHalfsamps + 1, -0.5 * kfincr, 
                        kHalfsamps * kfincr + kfincr / 2. ); 
        TH1F* hhigh = new TH1F( Form("hNHighPks%d", ch), "", 
//                        kHalfsamps + 1, -0.5, kHalfsamps + 0.5 ); 
                        31, -0.5, 30.5 );  
        TH1F* hc    = new TH1F( Form("hcc%d", ch), "", 110, -0.005, 1.005 );
        TH1F* hci   = new TH1F( Form("hccinv%d", ch), "", 110, -0.005, 1.005 );
     
        hFFTPeak->AddAt( hpeak, ch );
        hNHighPks->AddAt( hhigh, ch );
        hcc->AddAt( hc, ch );
        hccinv->AddAt( hci, ch );
    }

    fEvtIdOut = new TSnRunEvtOut;

    hNHighPks->SetName( "hNHighPks" );    
    hNHighPksAllCh->SetName( "hNHighPksAllCh" );
    hcc->SetName( "hcc" );
    hccinv->SetName( "hccinv" );

    AddOutput( hAvgCCvT );
    AddOutput( hCC );
    AddOutput( hFFTPeak );
    AddOutput( hNHighPks );
    AddOutput( hNHighPksAllCh );
    AddOutput( hcc );
    AddOutput( hccinv );
}


void TSnPlotCCMod::Process( )
{
    LoadBranch( TSnRawTreeMaker::kEHdBrNm );
    LoadBranch( TSnRawTreeMaker::kEMtBrNm );
    LoadBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm );

    const TSnCorrelateWaves* cc = dynamic_cast< const TSnCorrelateWaves* >
                                            ( FindObjThisEvt("corrcoef") );

    hAvgCCvT->Fill( fHdr->GetUnixTime(), cc->GetAvgCC() );

    UInt_t run;
    UInt_t evt;
    std::pair< UInt_t, UInt_t > el;

    TSnCalFFTData* fft = new TSnCalFFTData( "TSnCalFFTData", "", *fData );

//    if( cc->GetAvgCC() >= 0.45 )
    {
        run = fMeta->GetRunNum( );
        evt = fHdr->GetEvtNum( );    
        el  = std::make_pair( run, evt );

        fEvtIdOut->Set( el );


        UInt_t Peaks[NSnConstants::kNchans];        

        for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
        {
            Float_t maxElem;
            Float_t* pfft       = fft->GetFFTData( ch );
            Float_t* peakValue  = std::max_element( pfft,
                                                pfft + TSnCalFFTData::kFftPts);
            UInt_t nhighbins    = 0;
            TH1F* hhigh         = dynamic_cast<TH1F*>( hNHighPks->At(ch) ); 

            for( UChar_t pt = 0; pt < TSnCalFFTData::kFftPts; pt++ )
            {
                Float_t binVal  = *( pfft + pt );

                if( binVal > *peakValue/2 )
                {
                    nhighbins++;
                }
            }

            Peaks[ch] = nhighbins;

            hhigh->Fill( nhighbins );
            hNHighPksAllCh->Fill( nhighbins );

            maxElem     = fft->GetFreqData( std::distance(pfft, peakValue) ); 
            TH1F* hpeak = dynamic_cast<TH1F*>( hFFTPeak->At(ch) );

            hpeak->Fill( maxElem ); 
//            Printf( "ch %d max element: %f nhbins: %d", 
//                                                    ch, maxElem, nhighbins );
//            Printf( "Num high bins: %d", nhighbins );
        }

        UChar_t itt = 0; // number of channels having # bins >= 6 
        
        for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
        {
            if( Peaks[ch] >= 7 )
            {
                itt++;
            }
        }

        if( itt >= 2 )
        {
            if( cc->GetAvgCC() >= 0.42 )
            {
                Printf( "run: %d seq: %d evt: %d avg CC: %f", 
                    fMeta->GetRunNum(), fMeta->GetSeqNum(), fHdr->GetEvtNum(),
                    cc->GetAvgCC() );
            }
//            Printf( "%d %d %d %d", Peaks[0], Peaks[1], Peaks[2], Peaks[3] );

            for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
            {
                TH1F* h     = dynamic_cast< TH1F* >( hcc->At(ch) );
                TH1F* hi    = dynamic_cast< TH1F* >( hccinv->At(ch) );
        
                h->Fill( cc->GetCC(ch) );
                hi->Fill( cc->GetCCInv(ch) );         
            }

            hCC->Fill( cc->GetAvgCC() );
//            Printf( "%d %d", fMeta->GetRunNum(), fHdr->GetEvtNum() );
        }

//        Printf( "Remaining high <X> event: r%d, s%d, e%d", 
//            fMeta->GetRunNum(), fMeta->GetSeqNum(), fHdr->GetEvtNum() );
    }
//    else
    {
//        hCC->Fill( cc->GetAvgCC() );
    }
}



void TSnPlotCCMod::Terminate( )
{
    AddOutput( fEvtIdOut );

    Printf( "Events remaining after Clustering: %d", fEvtIdOut->Size() );

//    for( UInt_t i = 0; i < fEvtIdOut.size(); i++ )
//        Printf( "Run: %d, Evt: %d", fEvtIdOut->GetRunNum(0), fEvtIdOut->GetEvtNum(0));
}
