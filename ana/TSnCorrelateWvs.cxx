#include "TSnCorrelateWvs.h"
#include "TAModule.h"
#include "NSnConstants.h"
#include "TSnSpectral.h"
#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnEventMetadata.h"
#include "TSnEventHeader.h"
#include "TSnRawTreeMaker.h"
#include "TSnSaveRecoResultsMod.h"
#include "TSnPowerReading.h"
#include "TSnWindData.h"
#include "TSnFFTCalcMod.h"
#include "TSnCalFFTData.h"

#include "TClass.h"
#include "TArrayF.h"
#include "TH2F.h"

#include <vector>


ClassImp( TSnCorrelateWvs );
ClassImp( TSnCorrCoef );
ClassImp( TSnClust );


UInt_t minclustsze                      = 2;
Float_t maxsep                          = 3600.;
UInt_t tmlastevt[NSnConstants::kNchans] = { 0 };
UInt_t cnum[NSnConstants::kNchans]      = { 1 };


TSnCorrelateWvs::TSnCorrelateWvs( ) : fData(0), fMeta(0), fHdr(0), fPwr(0),
    fWind(0), fCC(0), hCC(0), hPrCC(0), hpcNS(0), hpcEW(0), hCCAvg(0), 
    hVvsCC(0), hWndvsCC(0), hPwrvsCC(0), hPwrvsWnd(0), hPwr(0), fClust(0), hAvgCCvT(0)
{
}


TSnCorrelateWvs::TSnCorrelateWvs( const Char_t* name, 
    std::vector< Float_t > wvtmplt, const Char_t* fftNm ) : 
    TAModule(name, "calculate correlation coefficients"),
    fData(0), fMeta(0), fHdr(0), fPwr(0), fWind(0), fFFT(fftNm), fCC(0),
    hCC(0), hPrCC(0), hpcNS(0), hpcEW(0), hCCAvg(0), hVvsCC(0), hWndvsCC(0),
    hPwrvsCC(0), hPwrvsWnd(0), hPwr(0), fClust(0), hAvgCCvT(0)
{
    //fCC     = new TSnCorrCoef( Form("%s.", name) );

    wvtp    = wvtmplt;
    hPrCC   = new TObjArray( NSnConstants::kNchans );
    hPwr    = new TObjArray( NSnConstants::kNchans );
}


TSnCorrelateWvs::~TSnCorrelateWvs( )
{
    delete fData;
    delete fMeta;
    delete fHdr;
    delete fPwr;
    delete fWind;
    //delete fCC;
    delete hCC;
    delete hPrCC;
    delete hpcNS;
    delete hpcEW;
    delete hCCAvg;
    delete hVvsCC;
    delete hWndvsCC;
    delete hPwrvsCC;
    delete hPwrvsWnd;
    delete hPwr;
    delete hAvgCCvT;
}



void TSnCorrelateWvs::SlaveBegin( )
{
    ReqBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm, fData );
    ReqBranch( TSnRawTreeMaker::kEMtBrNm, fMeta );
    ReqBranch( TSnRawTreeMaker::kEHdBrNm, fHdr );
    ReqBranch( TSnRawTreeMaker::kPwrBrNm, fPwr );
    ReqBranch( "WindData", fWind );

    hCC         = new TH2F( "hCC", "", 110, -0.05, 1.05, 110, -0.05, 1.05 );
    hpcNS       = new TH2F( "hpcNS", "", 110, -0.05, 1.05, 110, -0.05, 1.05 );
    hpcEW       = new TH2F( "hpcEW", "", 110, -0.05, 1.05, 110, -0.05, 1.05 );
    hCCAvg      = new TH2F( "hCCAvg", "", 110, -0.05, 1.05, 110, -0.05, 1.05 );
    hVvsCC      = new TH2F( "hVvsCC", "", 41, 9.5, 30.5, 110, -0.05, 1.05 );
    hWndvsCC    = new TH2F( "hWndvsCC", "", 41, -0.5, 40.5, 110, -0.05, 1.05 );
    hPwrvsCC    = new TH2F( "hPwrvsCC", "", 1000, 0, 120000, 110, -0.05, 1.05 );
    hPwrvsWnd   = new TH2F( "hPwrvsWnd", "", 1000, 0, 120000, 41, -0.5, 40.5 );
    hAvgCCvT    = new TH2F( "hAvgCCvT", "", 16245,
            1389000000, 1395500000, 110, -0.05, 1.05 );

    for( UShort_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        TH2F* hp = new TH2F( Form("hCCvT%i", ch), "", 16245,
            1389000000, 1395500000, 110, -0.05, 1.05 ); 

        TH2F* hpr = new TH2F( Form("hPwr%i", ch), "", 
            16245, 1389000000, 1395500000, 1000, 7000, 120000 );

        hPrCC->AddAt( hp, ch );
        hPwr->AddAt( hpr, ch );
    }

    AddOutput( hCC );
    AddOutput( hPrCC );
    AddOutput( hpcNS );
    AddOutput( hpcEW );
    AddOutput( hCCAvg );
    AddOutput( hVvsCC );
    AddOutput( hWndvsCC );
    AddOutput( hPwrvsCC );
    AddOutput( hPwrvsWnd );
    AddOutput( hPwr );
    AddOutput( hAvgCCvT );
}


void TSnCorrelateWvs::Process( )
{
    std::vector< Float_t > wvdt( NSnConstants::kNsamps );
    Float_t maxcc[NSnConstants::kNchans] = { -999. };

    LoadBranch( TSnSaveCalibDataMod::kFPNSubDatBrNm );
    LoadBranch( TSnRawTreeMaker::kEMtBrNm );
    LoadBranch( TSnRawTreeMaker::kEHdBrNm );
    LoadBranch( TSnRawTreeMaker::kPwrBrNm );
    LoadBranch( "WindData" );

    fCC     = new TSnCorrCoef( Form("%s.", GetName()) );
    fClust  = new TSnClust( "Clusters" );
    
    const TSnCalFFTData* fft = 
        dynamic_cast< const TSnCalFFTData* >( FindObjThisEvt(fFFT) );

    if( fft != 0 ) 
    {
        Float_t vsum[NSnConstants::kNchans] = {0., 0., 0., 0.};

        for( UShort_t ch = 0; ch < NSnConstants::kNchans; ch++  )
	    {
	        std::vector< Float_t > vcc( NSnConstants::kNsamps );
	        const Float_t* adc = fData->GetData( ch );

//	        TH2F* hp    = dynamic_cast<TH2F*>( hPrCC->At(ch) );
//            TH2F* hpr   = dynamic_cast<TH2F*>( hPwr->At(ch) );

	        for( UShort_t sm = 0; sm < NSnConstants::kNsamps; sm++, adc++ )
	        {
	            wvdt[sm] = *adc; 

	        }    
    
	        TSnSpectral::PearsonSmpCorrCoefCirc( wvdt, wvtp, vcc );

	        for( UShort_t sm = 0; sm < NSnConstants::kNsamps; sm++ )
	        {
	            if( vcc[sm] > maxcc[ch] )
		        {
		            maxcc[ch] = vcc[sm];
		        }
	        } 

	        fCC->corrcoef.AddAt( maxcc[ch], ch );

            for( UChar_t p = 0; p < NSnConstants::kNsamps; p++ )
            {
                if( fft->GetFrequency(p) > 0.17 ) 
                {
                    if( fft->GetFrequency(p) < 0.3 )
                    {
                        vsum[ch] += fft->GetFFTMag( ch, p );
                    }
                    else
                    {
                        break;
                    }
                }
            }

//            if( (ch == 1 && vsum[ch] > 7837) || (ch == 1 && vsum[ch] > 7903)
//                || (ch == 2 && vsum[ch] > 7085) || (ch == 3 && vsum[ch] > 7002) )
/*            if( !(ch == 0 && vsum[ch] > 7837) && !(ch == 1 && vsum[ch] > 7903)
                && !(ch == 2 && vsum[ch] > 7085) && !(ch == 3 && vsum[ch] > 7002) )
            {
                hpr         ->Fill( fHdr->GetUnixTime(), vsum[ch] );    
	            hp          ->Fill( fHdr->GetUnixTime(), maxcc[ch] );
            }
*/
	    }

        for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
        {
            TH2F* hp    = dynamic_cast<TH2F*>( hPrCC->At(ch) );
            TH2F* hpr   = dynamic_cast<TH2F*>( hPwr->At(ch) );

//            if( (ch == 0 && maxcc[ch] > 0.5) || (ch == 1 && maxcc[ch] > 0.5)
//                || (ch == 2 && maxcc[ch] > 0.5) || (ch == 3 && maxcc[ch] > 0.5) )

            if( (maxcc[0]+maxcc[1]+maxcc[2]+maxcc[3])/4 > 0.4 )
//            if( (ch == 0 && vsum[ch] > 7837) || (ch == 1 && vsum[ch] > 7903)
//                || (ch == 2 && vsum[ch] > 7085) || (ch == 3 && vsum[ch] > 7002) )
//            if( !(ch == 0 && vsum[ch] > 7837) && !(ch == 1 && vsum[ch] > 7903)
//                && !(ch == 2 && vsum[ch] > 7085) && !(ch == 3 && vsum[ch] > 7002) )
            {
                if( (fHdr->GetUnixTime() - tmlastevt[ch]) < maxsep )
                {
                    fClust->cluster.AddAt( cnum[ch], ch );

                    hCC->Fill( maxcc[1], maxcc[2] );
                    hCC->Fill( maxcc[1], maxcc[3] );
    
                    hpcNS->Fill( maxcc[0], maxcc[2] );
                    hpcEW->Fill( maxcc[1], maxcc[3] );

                    hCCAvg->Fill( (maxcc[0] + maxcc[2])/2., 
                                  (maxcc[1] + maxcc[3])/2. );

                    hPwrvsCC    ->Fill( vsum[ch], maxcc[ch] );         
                    hPwrvsWnd   ->Fill( vsum[ch], fWind->GetWind() );
	                hVvsCC      ->Fill( fPwr->GetAveV1()*6.0649e-4, maxcc[ch] );
	                hWndvsCC    ->Fill( fWind->GetWind(), maxcc[ch] );

                    hpr         ->Fill( fHdr->GetUnixTime(), vsum[ch] );    
	                hp          ->Fill( fHdr->GetUnixTime(), maxcc[ch] );
                    hAvgCCvT    ->Fill( fHdr->GetUnixTime(), (maxcc[0]+maxcc[1]+maxcc[2]+maxcc[3])/4 );

                }
                else
                {
                    cnum[ch]++;
                    fClust->cluster.AddAt( cnum[ch], ch );
/*
                    hCC->Fill( maxcc[1], maxcc[2] );
                    hCC->Fill( maxcc[1], maxcc[3] );
    
                    hpcNS->Fill( maxcc[0], maxcc[2] );
                    hpcEW->Fill( maxcc[1], maxcc[3] );

                    hCCAvg->Fill( (maxcc[0] + maxcc[2])/2., 
                                  (maxcc[1] + maxcc[3])/2. );

                    hPwrvsCC    ->Fill( vsum[ch], maxcc[ch] );         
                    hPwrvsWnd   ->Fill( vsum[ch], fWind->GetWind() );
	                hVvsCC      ->Fill( fPwr->GetAveV1()*6.0649e-4, maxcc[ch] );
	                hWndvsCC    ->Fill( fWind->GetWind(), maxcc[ch] );

                    hpr         ->Fill( fHdr->GetUnixTime(), vsum[ch] );    
	                hp          ->Fill( fHdr->GetUnixTime(), maxcc[ch] );
                    hAvgCCvT    ->Fill( fHdr->GetUnixTime(), (maxcc[0]+maxcc[1]+maxcc[2]+maxcc[3])/4 );
*/ 
                }
    
                tmlastevt[ch] = fHdr->GetUnixTime( );
/*
                hCC->Fill( maxcc[0], maxcc[2] );
                hCC->Fill( maxcc[1], maxcc[3] );

                hpcNS->Fill( maxcc[0], maxcc[2] );
                hpcEW->Fill( maxcc[1], maxcc[3] );

                hCCAvg->Fill( (maxcc[0] + maxcc[2])/2., 
                              (maxcc[1] + maxcc[3])/2. );

                hPwrvsCC    ->Fill( vsum[ch], maxcc[ch] );         
                hPwrvsWnd   ->Fill( vsum[ch], fWind->GetWind() );
	            hVvsCC      ->Fill( fPwr->GetAveV1()*6.0649e-4, maxcc[ch] );
	            hWndvsCC    ->Fill( fWind->GetWind(), maxcc[ch] );

                hpr         ->Fill( fHdr->GetUnixTime(), vsum[ch] );    
	            hp          ->Fill( fHdr->GetUnixTime(), maxcc[ch] );
*/
            }
            else
            {

                if( (fHdr->GetUnixTime() - tmlastevt[ch]) < maxsep )
                {
                    fClust->cluster.AddAt( cnum[ch], ch );

                    hCC->Fill( maxcc[1], maxcc[2] );
                    hCC->Fill( maxcc[1], maxcc[3] );
    
                    hpcNS->Fill( maxcc[0], maxcc[2] );
                    hpcEW->Fill( maxcc[1], maxcc[3] );

                    hCCAvg->Fill( (maxcc[0] + maxcc[2])/2., 
                                  (maxcc[1] + maxcc[3])/2. );

                    hPwrvsCC    ->Fill( vsum[ch], maxcc[ch] );         
                    hPwrvsWnd   ->Fill( vsum[ch], fWind->GetWind() );
	                hVvsCC      ->Fill( fPwr->GetAveV1()*6.0649e-4, maxcc[ch] );
	                hWndvsCC    ->Fill( fWind->GetWind(), maxcc[ch] );

                    hpr         ->Fill( fHdr->GetUnixTime(), vsum[ch] );    
	                hp          ->Fill( fHdr->GetUnixTime(), maxcc[ch] );
                    hAvgCCvT    ->Fill( fHdr->GetUnixTime(), (maxcc[0]+maxcc[1]+maxcc[2]+maxcc[3])/4 );

                }
                else
                {
                    fClust->cluster.AddAt( 0, ch );
/*
                    hCC->Fill( maxcc[1], maxcc[2] );
                    hCC->Fill( maxcc[1], maxcc[3] );
    
                    hpcNS->Fill( maxcc[0], maxcc[2] );
                    hpcEW->Fill( maxcc[1], maxcc[3] );

                    hCCAvg->Fill( (maxcc[0] + maxcc[2])/2., 
                                  (maxcc[1] + maxcc[3])/2. );

                    hPwrvsCC    ->Fill( vsum[ch], maxcc[ch] );         
                    hPwrvsWnd   ->Fill( vsum[ch], fWind->GetWind() );
	                hVvsCC      ->Fill( fPwr->GetAveV1()*6.0649e-4, maxcc[ch] );
	                hWndvsCC    ->Fill( fWind->GetWind(), maxcc[ch] );

                    hpr         ->Fill( fHdr->GetUnixTime(), vsum[ch] );    
	                hp          ->Fill( fHdr->GetUnixTime(), maxcc[ch] );
                    hAvgCCvT    ->Fill( fHdr->GetUnixTime(), (maxcc[0]+maxcc[1]+maxcc[2]+maxcc[3])/4 );
*/
                }
            }
        }

        if( (maxcc[0] >= 0.6 && maxcc[2] >= 0.6) || 
	        (maxcc[1] >= 0.6 && maxcc[3] >= 0.6) )
	    {
	        Printf( "Time: %lu, Run: %i, Seq: %i, Evt: %i, "\
		        "cc0: %f cc1: %f cc2: %f cc3: %f fft: %f\n",
		        fHdr->GetUnixTime(), fMeta->GetRunNum(), fMeta->GetSeqNum(), 
		        fHdr->GetEvtNum(), maxcc[0], maxcc[1], maxcc[2], maxcc[3],
		        fft->GetFFTMag(0, 0) );
	    }
    
        AddObjThisEvt( fCC );
        AddObjThisEvt( fClust );
    } 
    else 
    {
        SendError( kAbortModule, "Process", "Could not get fft named [%s]",
		                                                            "fft" );
    }
}


void TSnCorrelateWvs::Terminate( )
{
    for( UChar_t c = 0; c < NSnConstants::kNchans; c++ )
    {
        TH2F* h = dynamic_cast<TH2F*>( hPwr->At(c) );
        Printf( "RMS: %f, Mean: %f", h->GetRMS(2), h->GetMean(2) );
    }
}

