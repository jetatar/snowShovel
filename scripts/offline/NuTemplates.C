#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TGraph.h>
#include <TRandom3.h>
#include <TMath.h>

#include <algorithm>
#include <utility>
#include <iterator>

#include "TSnCalWvData.h"
#include "TSnCalFFTData.h"
#include "TSnNumHighFFTBinsMod.h"

#endif

static const Char_t* fin = 
                "/data/users/jtatar/Work/Cosmic/Templates/nt.sigtemps.root";
//             "/data/users/jtatar/Work/Cosmic/Templates/PCD_100MHzHPF.dat.root";
//            "/w1/jtatar/Analysis/Templates/nt.onlytemplates.root";
//static const Char_t* foutnm = "bouncenonoise.root";
//static const Char_t* foutnm = "nu100sigwithnoise.root";
static const Char_t* foutnm = "test2.root";

const Float_t kSmpRate   = 1.92; // GHz
const Float_t kDelta     = 1.0 / kSmpRate;
const Int_t   kHalfsamps = NSnConstants::kNsamps/2;
const Float_t kfincr     = 1.0 / (kDelta*NSnConstants::kNsamps);

void NuTemplates( void )
{
    TFile* fl = new TFile( fin );

    TTree* tr = (TTree*)fl->Get( "Templates" );
    //TTree* tr = (TTree*)fl->Get( "CalibTree" );

    TSnCalWvData* wv = new TSnCalWvData( "wave", "TSnCalWvData" );
    Float_t EAng;
    Float_t HAng;

    tr->SetBranchAddress( "wave.",  &wv );
    //tr->SetBranchAddress( "AmpOutData.",  &wv );
//    tr->SetBranchAddress( "EAng",   &EAng );
//    tr->SetBranchAddress( "HAng",   &HAng );

    TFile* fout = new TFile( foutnm, "recreate" );
    TTree* nutr = new TTree( "Templates", "" );

    TSnCalWvData* nuwv = new TSnCalWvData( "nuwave", "TSnCalWvData2" );
    Float_t nuEAng;
    Float_t nuHAng;

    nutr->Branch( "wave.",  nuwv, 262144, 1 );
//    nutr->Branch( "EAng",   &nuEAng );
//    nutr->Branch( "HAng",   &nuHAng );

    TRandom3* rand  = new TRandom3( );

//    Float_t avgfft[TSnCalFFTData::kFftPts] = { 0. };

    TH1F* hpeak     = new TH1F( "hpeak", "", kHalfsamps + 1, -0.5 * kfincr, 
                                kHalfsamps * kfincr + kfincr / 2. );
    TH1F* hNHighPks = new TH1F( "hNHighPks", "", 
                                kHalfsamps + 1, -0.5, kHalfsamps + 0.5 );

    TH1F* hNHighPksvsEAng[8];
    TH1F* hNHighPksvsHAng[8];
    
    for( UChar_t n = 0; n < 8; n++ )
    {
        hNHighPksvsEAng[n] = new TH1F( Form("hNHighPksVsEAng%d", n*10), "",
            9, 3.5, 12.5 );
        hNHighPksvsHAng[n] = new TH1F( Form("hNHighPksVsHAng%d", n*10), "",
            9, 3.5, 12.5 );
    }

    ULong64_t nevts = tr->GetEntries( );

    for( ULong64_t i = 0; i < nevts; i++ )
    {
        tr->GetEntry( i );

        Float_t* pNoisy = wv->GetData( 0 );
        Float_t maxElem;

        nuEAng = EAng;
        nuHAng = HAng;
        
//        for( UInt_t num = 0; num < 1000; num++ )
        {
            // Add noise to signal templates.
            for( UChar_t sm = 0; sm < NSnConstants::kNsamps; sm++ )
            {
                Float_t noisy = 0;
        
//                noisy = *(pNoisy + sm) * 100. + rand->Gaus( 0, 20 );
                noisy = *(pNoisy + sm);

                nuwv->SetData( 0, sm, noisy );
            }

            TSnCalFFTData* nufft = new TSnCalFFTData( "", "", *nuwv );
            TSnCalFFTData* fft  = new TSnCalFFTData( "", "", *nuwv );
            //TSnCalFFTData* fft  = new TSnCalFFTData( "", "", *wv );

//            Float_t* pfft       = fft->GetFFTData( 0 );
            Float_t fftmag[TSnCalFFTData::kFftPts];

            for( UChar_t b = 0; b < TSnCalFFTData::kFftPts; b++ )
            {
               fftmag[b] = fft->GetFFTMag( 0, b ); 
            }

            Float_t maxfftval   = TMath::MaxElement( TSnCalFFTData::kFftPts, 
                                                                    fftmag );
            UInt_t nhighbins    = 0;

            for( UChar_t pt = 0; pt < TSnCalFFTData::kFftPts; pt++ )
            {
                if( fftmag[pt] > maxfftval / 2 )
                {
                    nhighbins++;
                }
            }

            //maxElem = fft->GetFrequency( std::distance(pfft, peakValue) );
            //maxElem = fft->GetFreqData( std::distance(pfft, peakValue) );

//            hpeak->Fill( maxElem );
            hNHighPks->Fill( nhighbins );

//            hNHighPksvsEAng[int(EAng/10)]->Fill( nhighbins );

//            if( HAng == 59 )
//            {
//                hNHighPksvsHAng[6]->Fill( nhighbins );
//            }
//            else
//            {
//                hNHighPksvsHAng[int(HAng/10)]->Fill( nhighbins );
//            }
            
            nutr->Fill( );  
 
            delete fft;
        }
    }

    hNHighPks->Write( "hNHighPks" );

    fl->Close( );

    fout->cd( );
//    nutr->Write( );
//    fout->Write( );
    fout->Close( );
}
