#ifndef SNS_TSnSelEvtsOnCC
#define SNS_TSnSelEvtsOnCC

#include "TAModule.h"

class TSnEventHeader;
class TSnCalWvData;
class TH2F;
class TH1F;
class TSnEventMetadata;

class TSnSelEvtsOnCC : public TAModule
{
    private:
        TSnCalWvData*           fData;
        std::vector< Float_t >  tmpltwv;
        TSnEventHeader*         fHdr;
        TH2F*                   hCCvT;
        TH2F*                   hLrgCCvT;
        TH1F*                   hCC;
        TSnEventMetadata*       fMeta;        
        TObjArray*              hCCbyCh;

    public:
        TSnSelEvtsOnCC( ) : fData(0), fHdr(0), fMeta(0) { }
        TSnSelEvtsOnCC( const Char_t* name, std::vector< Float_t > tmpltsig );   
        virtual ~TSnSelEvtsOnCC( ){ }

    protected:
        virtual void SlaveBegin( );
        virtual void Process( );

    ClassDef( TSnSelEvtsOnCC, 1 );
};

#endif // SNS_TSnSelEvtsOnCC
