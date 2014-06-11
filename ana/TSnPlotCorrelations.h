#ifndef SNS_TSnPlotCorrelations
#define SNS_TSnPlotCorrelations

#include "TAModule.h"
#include "NSnConstants.h"

class TObjArray;

class TSnPlotCorrelations : public TAModule
{
    public:
        TObjArray* hCC;

        TSnPlotCorrelations( ) : hCC(0)
        { }
        TSnPlotCorrelations( const Char_t* name ) : 
            TAModule(name, "TSnPlotCorrelations"), hCC(0)
        {
            hCC = new TObjArray( NSnConstants::kNchans );
        }

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );

    ClassDef( TSnPlotCorrelations, 1 );
};


#endif // SNS_TSnPlotCorrelations
