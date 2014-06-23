#ifndef SNS_TSnNumHighFFTBinsMod
#define SNS_TSnNumHighFFTBinsMod

#include "NSnConstants.h"
#include "TNamed.h"

class TSnCalWvData;
class TSnCalFFTData;


class TSnNumHighFFTBinsMod : public TNamed 
{
    public:
        TSnNumHighFFTBinsMod( ){ }
        TSnNumHighFFTBinsMod( const Char_t* name, TSnCalFFTData* fft ) : 
            TNamed(name, "Find number of high FFT bins"), fFFT(0)
        {
            fFFT = fft;
        }

        virtual ~TSnNumHighFFTBinsMod( ){ }

        UInt_t GetNumHighBins( UChar_t ch );

        void Calculate( void );

    private:
        TSnCalFFTData*  fFFT;
        UInt_t          nHighBins[NSnConstants::kNchans];
    
    ClassDef( TSnNumHighFFTBinsMod, 1 );
};


#endif // SNS_TSnNumHighFFTBinsMod
