#include "TSnCorrelateWaves.h"

#include "TAModule.h"
#include "NSnConstants.h"
#include "TSnSpectral.h"
#include "TSnCalWvData.h"
#include "TSnSaveCalibDataMod.h"

#include <algorithm>
#include <vector>

ClassImp( TSnCorrelateWaves );


TSnCorrelateWaves::TSnCorrelateWaves( ) 
{
}


TSnCorrelateWaves::TSnCorrelateWaves( const Char_t* name, TSnCalWvData* dt, 
    std::vector< Float_t > wv ) : 
    TNamed( name, "Calculate Correlation Coefficients against a template" ),
    fData(dt)
{
    tpwave  = wv;    // pass template waveform for CC calculations.
}


void TSnCorrelateWaves::Correlate( )
{
    // Store event waveform for each channel in a vector for CC func
    std::vector< Float_t >  evtwv( NSnConstants::kNsamps );
    // Store inverse event waveform since we don't know the relative phases
    std::vector< Float_t >  invevtwv( NSnConstants::kNsamps );

    // Storage for all correlation coefficients (one for each shift)
    std::vector< Float_t > cc( NSnConstants::kNsamps ); 
    // Store all CC for inverse waveform.
    std::vector< Float_t > invcc( NSnConstants::kNsamps );

    for( UChar_t ch = 0; ch < NSnConstants::kNchans; ch++ )
    {
        // Point to the first entry of the channel whose data we want.
        const Float_t* adc = fData->GetData( ch );

        for( UChar_t s = 0; s < NSnConstants::kNsamps; s++, adc++ )
        {
            evtwv[s]    = *adc;
            invevtwv[s] = *adc * -1;
        }

        // evtwv = input evt template wvfrm, tpwave = input tmplt wvfrm, 
        // cc = output correlation coefficients
        TSnSpectral::PearsonSmpCorrCoefCirc( evtwv, tpwave, cc );
        TSnSpectral::PearsonSmpCorrCoefCirc( invevtwv, tpwave, invcc );

        // Find the maximum correlation coefficient value and save it
        Float_t maxcc       = *(std::max_element(cc.begin(), cc.end()));
        Float_t invmaxcc    = *(std::max_element(invcc.begin(), invcc.end()));

        fCCWave[ch]     = maxcc;
        fCCWaveInv[ch]  = invmaxcc;
/*
        if( maxcc >= invmaxcc )
        {
            fCCWave[ch] = maxcc;
        }
        else
        {
            fCCWave[ch] = invmaxcc;
        }
*/
    }
}


Float_t TSnCorrelateWaves::GetCC( const UChar_t c ) const
{
    return( fCCWave[c] );
}


Float_t TSnCorrelateWaves::GetCCInv( const UChar_t c ) const
{
    return( fCCWaveInv[c] );
}



Float_t TSnCorrelateWaves::GetAvgCC( void ) const
{
    Float_t avg = 0;

    for( UChar_t c = 0; c < NSnConstants::kNchans; c++ )
    {
        avg += fCCWave[c];
    }
            
    return( avg/NSnConstants::kNchans );
}
