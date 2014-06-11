#ifndef SNS_TSnCorrelateWaves
#define SNS_TSnCorrelateWaves

#include "NSnConstants.h"
#include "TNamed.h"

#include <vector>

class TSnCalWvData;

class TSnCorrelateWaves : public TNamed
{
    private:
        Float_t                 fCCWave[NSnConstants::kNchans];
        Float_t                 fCCWaveInv[NSnConstants::kNchans];
        TSnCalWvData*           fData;
        std::vector< Float_t >  tpwave; // template waveform

    public:
        TSnCorrelateWaves( );
        TSnCorrelateWaves( const Char_t* name, TSnCalWvData* dt, 
                                               std::vector< Float_t > wv );
        void Correlate( );
        Float_t GetCC( const UChar_t c ) const;
        Float_t GetCCInv( const UChar_t c ) const;
        Float_t GetAvgCC( void ) const;

        virtual ~TSnCorrelateWaves( ){ };

    protected:

    ClassDef( TSnCorrelateWaves, 1 );
};

#endif // SNS_TSnCorrelateWaves
