#include "TSnPlotCorrelations.h"
#include "TSnCalWvData.h"
#include "TSnCorrelateWaves.h"

#include "TH1F.h"

ClassImp( TSnPlotCorrelations );


void TSnPlotCorrelations::SlaveBegin( )
{
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* hcc = new TH1F( Form("hCC%d", ch), "", 110, -0.005, 1.005 );

        hCC->AddAt( hcc, ch );
    }


    AddOutput( hCC );
}


void TSnPlotCorrelations::Process( )
{
    const TSnCorrelateWaves* cc = dynamic_cast< const TSnCorrelateWaves* >
                                              ( FindObjThisEvt("corrcoef") );

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH1F* hcc = dynamic_cast< TH1F* >( hCC->At(ch) );

        hcc->Fill( cc->GetCC(ch) );
    }
}


void TSnPlotCorrelations::Terminate( )
{
}
