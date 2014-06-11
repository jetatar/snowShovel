#include "TSnCorrelateWavesMod.h"

#include "TSnCalWvData.h"
#include "TSnCorrelateWavesMod.h"
#include "TSnCorrelateWaves.h"
#include "TSnSaveCalibDataMod.h"

#include "TH1F.h"

ClassImp( TSnCorrelateWavesMod );


TSnCorrelateWavesMod::TSnCorrelateWavesMod( const Char_t* name,
    std::vector< Float_t > tmpltsig ) : TAModule( name, "TSnCorrelateWaves" ),
    fData(0)
{
    tmpltwv = tmpltsig;

    hCC     = new TObjArray( NSnConstants::kNchans );
    hCCInv  = new TObjArray( NSnConstants::kNchans );
}


void TSnCorrelateWavesMod::SlaveBegin( )
{
    ReqBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm, fData );

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* hcc   = new TH1F( Form("hCC%d", ch), "", 110, -0.005, 1.005 );
        TH1F* hccinv= new TH1F( Form("hCCInv%d", ch), "", 110, -0.005, 1.005 );

        hCC->AddAt( hcc, ch );
        hCCInv->AddAt( hccinv, ch );
    }

//    AddOutput( hCC );
//    AddOutput( hCCInv );
}


void TSnCorrelateWavesMod::Process( )
{
    LoadBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm );

    TSnCorrelateWaves* cwvs = new TSnCorrelateWaves( "corrcoef", fData,
                                                            tmpltwv );
    cwvs->Correlate( );

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* h     = dynamic_cast< TH1F* >( hCC->At(ch) );
        TH1F* hi    = dynamic_cast< TH1F* >( hCCInv->At(ch) );

        h->Fill( cwvs->GetCC(ch) );
        hi->Fill( cwvs->GetCCInv(ch) );
    }

    AddObjThisEvt( cwvs );
}
