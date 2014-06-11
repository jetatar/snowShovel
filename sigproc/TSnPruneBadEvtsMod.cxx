#include "TSnPruneBadEvtsMod.h"

#include "TSnRawWaveform.h"
#include "TSnRawTreeMaker.h"
#include "TSnConfigTreeLoader.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnEventHeader.h"
#include "TSnRunInfo.h"
#include "TSnCalWvData.h"
#include "TSnClockSetInfo.h"

#include "TH1F.h"
#include "TH2F.h"


ClassImp( TSnPruneBadEvtsMod );


TSnPruneBadEvtsMod::TSnPruneBadEvtsMod( ) : hAdc(0), hdT(0), hdTvsAdc(0), 
    fFpnSub(0), fHdr(0), fRunInfo(0), fTrigSC(0)
{
}


TSnPruneBadEvtsMod::TSnPruneBadEvtsMod( const Char_t* name ) : 
    TAModule(name, "remove bad events"), hAdc(0), hdT(0), hdTvsAdc(0), 
    fFpnSub(0), fHdr(0), fRunInfo(0), fTrigSC(0)
{
    hAdc        = new TObjArray( NSnConstants::kNchans );
    hdTvsAdc    = new TObjArray( NSnConstants::kNchans );
}


TSnPruneBadEvtsMod::~TSnPruneBadEvtsMod( )
{
    delete fFpnSub;
    delete hAdc;
    delete hdTvsAdc;
    delete hdT;
    delete fHdr;
    delete fRunInfo;
    delete fTrigSC;
}


void TSnPruneBadEvtsMod::SlaveBegin( )
{
    ReqBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm, fFpnSub );
    ReqBranch( TSnRawTreeMaker::kEHdBrNm, fHdr );
    ReqBranch( TSnRawTreeMaker::kRunBrNm, fRunInfo );    
    ReqBranch( TSnRawTreeMaker::kCSrBrNm, fTrigSC );
//    ReqBranch( TSnRawTreeMaker::kEWvBrNm, fAdc );
    
    hdT = new TH1F( "hdT", "", 8192, -4095.5, 4095.5 );
    
    TH1F* h;
    TH2F* h2;

    for( UShort_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        h = new TH1F( Form("hAdc%d", ch), "", 8192, -4095.5, 4095.5 );
        hAdc->AddAt( h, ch );

        h2 = new TH2F( Form("hdTvsAdc%d", ch), "", 1001, -0.5, 1000.5,
                                                    8192, -4095.5, 4095.5 );
        hdTvsAdc->AddAt( h2, ch );
    }

    AddOutput( hAdc );
    AddOutput( hdTvsAdc );
    AddOutput( hdT );
}


void TSnPruneBadEvtsMod::Process( )
{
    LoadBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm );
    LoadBranch( TSnRawTreeMaker::kEHdBrNm );
    LoadBranch( TSnRawTreeMaker::kRunBrNm );
    LoadBranch( TSnRawTreeMaker::kCSrBrNm );

    if( fFpnSub != 0 )
    {
//        Printf( "We are here.\n" );
        if( fHdr->GetEvtNum() % fRunInfo->GetEvtsPerSeq() == 0 )

        {
            UInt_t dt = fHdr->GetUnixTime() - fTrigSC->GetCurrTime().GetSec(); 
            
            hdT->Fill( dt );

            for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
            {

                const Float_t* adc = fFpnSub->GetData( ch );
                
                TH1F* h     = dynamic_cast<TH1F*>( hAdc->At(ch) );
                TH2F* h2    = dynamic_cast<TH2F*>( hdTvsAdc->At(ch) );
                
                for( UChar_t s = 0; s < NSnConstants::kNsamps; s++, adc++ )
                {
                    h->Fill( *adc );
                    h2->Fill( dt, *adc );
                }
            }
        }
    } 
    else
    {
        SendError( kAbortModule, "Process", 
            "Could not get FPN subtracted data." );

    }
}



void TSnPruneBadEvtsMod::Terminate( )
{
    
}
