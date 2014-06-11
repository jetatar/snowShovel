#include "TSnCorrelateChans.h"
#include "TAModule.h"
#include "NSnConstants.h"
#include "TSnSpectral.h"
#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"
#include "TSnRecoChanOffsets.h"

#include "TH1F.h"
#include "TMath.h"

#include <vector>
#include <algorithm>


ClassImp( TSnCorrelateChans );


const UChar_t TSnCorrelateChans::kNChCmbs = 
                                TMath::Binomial( NSnConstants::kNchans, 2 );


TSnCorrelateChans::TSnCorrelateChans( ) : fData(0), fMeta(0), fHdr(0), hCC(0) 
{
}


TSnCorrelateChans::TSnCorrelateChans( const Char_t* name ) : 
    TAModule(name, "calculate correlation coefficients"),
    fData(0), fMeta(0), fHdr(0), hCC(0)
{
    hCC = new TObjArray( TSnCorrelateChans::kNChCmbs );
}


TSnCorrelateChans::~TSnCorrelateChans( )
{
    delete fData;
    delete fMeta;
    delete fHdr;
    delete hCC;
}



void TSnCorrelateChans::SlaveBegin( )
{
    ReqBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm, fData );
    ReqBranch( TSnRawTreeMaker::kEMtBrNm, fMeta );
    ReqBranch( TSnRawTreeMaker::kEHdBrNm, fHdr );

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        for( UChar_t ch2 = ch + 1; ch2 < NSnConstants::kNchans; ch2++ )
        {
            TH1F* h = new TH1F( Form("hCC%d%d", ch, ch2), "",
                                110, -0.005, 1.005 ); 
            h->SetTitle( Form(";CC of Ch%d vs Ch%d;", ch, ch2) );

            hCC->AddAt( h, TSnRecoChanOffsets::IndexFor(ch2, ch) );
        }
    }
    
    AddOutput( hCC );
}


void TSnCorrelateChans::Process( )
{
    std::vector< std::vector<Float_t> > wvdt( NSnConstants::kNchans, 
                                std::vector<Float_t>(NSnConstants::kNsamps) ); 
    std::vector< std::vector<Float_t> > maxcc( NSnConstants::kNchans, 
                         std::vector<Float_t>(NSnConstants::kNsamps, -999.) );

    std::vector< std::vector<Float_t> > invevtwv( NSnConstants::kNchans,
                                std::vector<Float_t>(NSnConstants::kNsamps) );


    LoadBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm );
    LoadBranch( TSnRawTreeMaker::kEMtBrNm );
    LoadBranch( TSnRawTreeMaker::kEHdBrNm );

//    Printf( "Evt %d\n", fHdr->GetEvtNum() );


//    if( fHdr->GetDTms() == 999 )
//    if( (fHdr->GetDTms() >= 2990 && fHdr->GetDTms() <= 3010) )// ||
//        (fHdr->GetDTms() >= 990 && fHdr->GetDTms() <= 1010) )
    {

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++  )
    {
        const Float_t* adc = fData->GetData( ch );

        for( UShort_t sm = 0; sm < NSnConstants::kNsamps; sm++, adc++ )
        {
            wvdt[ch][sm]     = *adc; 
            invevtwv[ch][sm] = (*adc) * (-1);

//            Printf( "Ch %u: Sm: %d V: %f\n", ch, sm, *adc );
        }    
    }

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++  )
    {
        for( UChar_t ch2 = ch + 1; ch2 < NSnConstants::kNchans; ch2++  )
        {
            std::vector<Float_t> tcc( NSnConstants::kNsamps );
            std::vector<Float_t> invtcc( NSnConstants::kNsamps );

            TSnSpectral::PearsonSmpCorrCoefCirc( wvdt[ch], wvdt[ch2], tcc );
            TSnSpectral::PearsonSmpCorrCoefCirc( wvdt[ch], invevtwv[ch2], 
                                                                    invtcc );

            TH1F* h = dynamic_cast<TH1F*>( 
                        hCC->At(TSnRecoChanOffsets::IndexFor(ch2, ch)) );
    
            Float_t maxcc   = *( std::max_element(tcc.begin(), tcc.end()) );
            Float_t invmaxcc= *( std::max_element(invtcc.begin(), 
                                                            invtcc.end()) );
//            UInt_t maxpos = std::distance( tcc.begin(), std::max_element(tcc.begin(), tcc.end()) );
//            UInt_t maxposinv = std::distance( invtcc.begin(), std::max_element(invtcc.begin(), invtcc.end()) );


//            Printf( "Ch %u vs %u: maxcc: %f", ch, ch2, maxcc );

            if( maxcc >= invmaxcc )
            {
                h->Fill( maxcc );
//                Printf( ">>> Max pos: %d\n", maxpos );
            }
            else
            {
                h->Fill( invmaxcc );
//                Printf( ">>> Max pos inv: %d\n", maxposinv );
            }
/*
            for( UShort_t sm = 0; sm < NSnConstants::kNsamps; sm++ )
            {
                if( tcc[sm] > maxcc[ch][ch2] )
                {
                    maxcc[ch][ch2] = tcc[sm];
                }
            } 

            h->Fill( maxcc[ch][ch2] );

            if( maxcc[ch][ch2] < 0.4 )
            {
                Printf( "Time: %lu, Run: %i, Seq: %i, Evt: %i, "\
                    "cc: %f ch%d,%d\n",
                    fHdr->GetUnixTime(), fMeta->GetRunNum(), fMeta->GetSeqNum(),                    fHdr->GetEvtNum(), maxcc[ch][ch2], ch, ch2 );
            }
*/
        }    
    }

    } 
}


void TSnCorrelateChans::Terminate( )
{
}

