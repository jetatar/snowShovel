#ifndef SNS_TSnPruneBadEvtsMod
#define SNS_TSnPruneBadEvtsMod

#include "TAModule.h"

// Forward declarations
class TSnEventHeader;
class TSnCalWvData;
class TObjArray;
class TSnRunInfo;
class TH1F;
class TSnClockSetInfo;

class TSnPruneBadEvtsMod : public TAModule
{
    private:
        TObjArray* hAdc;            // [chan] fpn sub adc dist
        TH1F* hdT;
        TObjArray* hdTvsAdc;

        TSnCalWvData*       fFpnSub;
        TSnEventHeader*     fHdr;     
        TSnRunInfo*         fRunInfo;
        TSnClockSetInfo*    fTrigSC;


    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );

    public:
        TSnPruneBadEvtsMod( );
        TSnPruneBadEvtsMod( const Char_t* name );
        virtual ~TSnPruneBadEvtsMod( );

    ClassDef( TSnPruneBadEvtsMod, 1 );
};


#endif // SNS_TSnPruneBadEvtsMod


