#ifndef SNS_TSnCountEventsMod
#define SNS_TSnCountEventsMod

#include "TAModule.h"


class TSnCountEventsMod : public TAModule
{
    public:
        TSnCountEventsMod( ) : fNumEvts(0)
        { }
        TSnCountEventsMod( const Char_t* name ) : 
            TAModule(name, "TSnCountEventsMod"), fNumEvts(0)
        { }

    private:
        ULong64_t fNumEvts;        

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );

    ClassDef( TSnCountEventsMod, 1 );
};

#endif // SNS_TSnCountEventsMod
