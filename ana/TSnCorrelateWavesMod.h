#ifndef SNS_TSnCorrelateWavesMod
#define SNS_TSnCorrelateWavesMod

#include "TAModule.h"

#include <vector>

class TSnCalWvData;


class TSnCorrelateWavesMod : public TAModule
{
    private:
        TSnCalWvData*           fData;
        std::vector< Float_t >  tmpltwv;
        TObjArray*              hCC;
        TObjArray*              hCCInv;

    public:
        TSnCorrelateWavesMod( ) : fData(0) { }
        TSnCorrelateWavesMod( const Char_t* name, 
                                            std::vector< Float_t > tmpltsig );

        virtual ~TSnCorrelateWavesMod( ) { }

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );

    ClassDef( TSnCorrelateWavesMod, 1 );
};

#endif // SNS_TSnCorrelateWavesMod
