#ifndef SNS_TSnNumHighFFTBins
#define SNS_TSnNumHighFFTBins

#include "NSnConstants.h"
#include "TNamed.h"

class TSnCalWvData;
class TSnCalFFTData;


class TSnNumHighFFTBins : public TNamed 
{
    public:
        TSnNumHighFFTBins( ){ }
        TSnNumHighFFTBins( const Char_t* name, TSnCalFFTData* fft ) : 
            TNamed(name, "Find number of high FFT bins"), fFFT(0)
        {
            fFFT = fft;
        }

        virtual ~TSnNumHighFFTBins( ){ }

        UInt_t GetNumHighBins( UChar_t ch );

        void Calculate( void );

    private:
        TSnCalFFTData*  fFFT;
        UInt_t          nHighBins[NSnConstants::kNchans];
    
    ClassDef( TSnNumHighFFTBins, 1 );
};


#endif // SNS_TSnNumHighFFTBins
