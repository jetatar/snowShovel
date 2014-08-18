#include <TGraph.h>

#include "TAModule.h"
#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnNumHighFFTBinsMod.h"

#include <stdexcept>

#include "TMath.h"

ClassImp( TSnNumHighFFTBinsMod );


void TSnNumHighFFTBinsMod::Calculate( )
{
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        Float_t magFFT[TSnCalFFTData::kFftPts];

        for( UChar_t b = 0; b < TSnCalFFTData::kFftPts; b++ )
        {
            magFFT[b] = fFFT->GetFFTMag( ch, b ); 
        }

        Float_t maxFFTVal = TMath::MaxElement( TSnCalFFTData::kFftPts, magFFT );
        UInt_t nhighbins  = 0;

        for( UChar_t bin = 0; bin < TSnCalFFTData::kFftPts; bin++ )
        {
            if( magFFT[bin] > maxFFTVal / 2. )
            {    
                nhighbins++;
            }
        }
        nHighBins[ch] = nhighbins;
    }
}


UInt_t TSnNumHighFFTBinsMod::GetNumHighBins( UChar_t ch )
{
    if ( ch > NSnConstants::kNchans )
    {
        throw std::out_of_range(
           Form("<TSnNumHighFFTBinsMod::GetNumHighBins>: "
                "Invalid channel number [%hhu]", ch));
    }
    return( nHighBins[ch] );
}

