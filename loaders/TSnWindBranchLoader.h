#ifndef SNS_TSnWindBranchLoader
#define SNS_TSnWindBranchLoader

#include "TSnTimeLinkBranchLoader.h"
#include "TSnWindData.h"
#include "TSnEventHeader.h"

class TSnWindBranchLoader : public TSnTimeLinkBranchLoader
{
    private:
        TSnWindData* fWnd;  
        TSnEventHeader* fHdr;    

    public:
        TSnWindBranchLoader( ) : fWnd(0), fHdr(0) { }
        TSnWindBranchLoader( TAMBranchInfo* binfo,
                            const Char_t* linkedTreeName,
                            const Char_t* linkedTimeBranchName,
                            const Char_t* mainTimeBranchName,
                            const TSnTimeLinkBranchLoader::EMethod_t method ) :
            TSnTimeLinkBranchLoader( binfo, linkedTreeName, linkedTimeBranchName,
                mainTimeBranchName, method ), fWnd(0), fHdr(0)
        { }
        
        virtual ~TSnWindBranchLoader( ) { }

        inline virtual TTimeStamp* GetMyTime( ) const 
        {
            return( fWnd != 0 ) ? &( fWnd->GetTime() ) : 0;
        }
        inline virtual TTimeStamp* GetMainTime( ) const
        {
            return( fHdr != 0 ) ? &( fHdr->GetTime() ) : 0;
        }
        
        virtual void SetPointerToMyLinkData( )
        {
            SetPointerToLinkDataIn( fWnd, GetMyBranch() );
        }
        virtual void SetPointerToMainLinkData( )
        {
            SetPointerToLinkDataIn( fHdr, GetMainBranch() );
        }

        ClassDef( TSnWindBranchLoader, 1 );
};

#endif // SNS_TSnWindBranchLoader
