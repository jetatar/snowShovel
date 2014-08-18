#ifndef SNS_TSnNumHighFFTBinsMod
#define SNS_TSnNumHighFFTBinsMod

#include "NSnConstants.h"
#include "TNamed.h"

#include "TSnCalFFTData.h"
class TSnCalWvData;


class TSnNumHighFFTBinsMod : public TNamed 
{
    public:
        TSnNumHighFFTBinsMod( ){ }
        TSnNumHighFFTBinsMod( const Char_t* name, const TSnCalFFTData* fft ) : 
            TNamed(name, "Find number of high FFT bins"), fFFT(fft)
        {
        }

        virtual ~TSnNumHighFFTBinsMod( ){ }

        UInt_t GetNumHighBins( const UChar_t ch );
        UInt_t GetHighBinElem( const UChar_t ch );

        void Calculate( void );

    private:
        const TSnCalFFTData* fFFT;
        UInt_t               nHighBins[NSnConstants::kNchans];
        UInt_t               fHiBinLoc[TSnCalFFTData::kFftPts];
    
    ClassDef( TSnNumHighFFTBinsMod, 2 ); // calculate the NHM variable from an fft
};


#endif // SNS_TSnNumHighFFTBinsMod
