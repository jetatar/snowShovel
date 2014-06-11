#include <algorithm>

#include "TAModule.h"
#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnNumHighFFTBins.h"
#include "TSnCalFFTData.h"

ClassImp( TSnNumHighFFTBins );


void TSnNumHighFFTBins::Calculate( )
{
    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        Float_t* pFFT    = fFFT->GetFFTData( ch );
        Float_t* pmaxFFT = std::max_element( pFFT, 
                                             pFFT + TSnCalFFTData::kFftPts );
            
        UInt_t nhighbins = 0;

        for( UChar_t bin = 0; bin < TSnCalFFTData::kFftPts; bin++ )
        {
            Float_t binVal = pFFT[bin];
    
            if( binVal > (*pmaxFFT) / 2 )
            {
                nhighbins++;
            }
        }
        
//        Printf( "NhighBins: %d", nhighbins );
        nHighBins[ch] = nhighbins;
    }
}


UInt_t TSnNumHighFFTBins::GetNumHighBins( UChar_t ch )


{
    return( nHighBins[ch] );
}
