#ifndef SNS_TSnCorrelateChans
#define SNS_TSnCorrelateChans

#include "TAModule.h"
#include "NSnConstants.h"

#include <vector>

class TSnCalWvData;
class TH2F;
class TSnEventMetadata;
class TSnEventHeader;
class TObjArray;
class TH1F;

class TSnCorrelateChans : public TAModule
{
    public:
        TSnCorrelateChans( );
        TSnCorrelateChans( const Char_t* name );
        virtual ~TSnCorrelateChans( );
        
        static const UChar_t kNChCmbs;

    private:
        TSnCalWvData*       fData;      // event data
        TSnEventMetadata*   fMeta;
        TSnEventHeader*     fHdr;
//        TH1F*               hCC[NSnConstants::kNchans][NSnConstants::kNchans];
        TObjArray*          hCC;

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );
        virtual void Terminate( );
    
    ClassDef( TSnCorrelateChans, 1 );
};

#endif
