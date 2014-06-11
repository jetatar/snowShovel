#ifndef SNS_TSnWindTreeLoader
#define SNS_TSnWindTreeLoader

#include "TAMVirtualLoader.h"
#include "TString.h"
#include "TSnTimeLinkBranchLoader.h"


class TAMVirtualBranchLoader;


class TSnWindTreeLoader : public TAMVirtualLoader
{
    private:
        TSnTimeLinkBranchLoader::EMethod_t fMeth;
        TString fTName;
        TString fBrName;

    public:
        TSnWindTreeLoader( ) : fMeth( TSnTimeLinkBranchLoader::kAbsoluteClosest )
            { }
        TSnWindTreeLoader( const Char_t* tree, const Char_t* br, 
                            const TSnTimeLinkBranchLoader::EMethod_t m ) : 
            fMeth( m ), fTName(tree), fBrName(br) { }

        
        virtual ~TSnWindTreeLoader( ) { } 

        TAMVirtualBranchLoader* CreateBranchLoader( TTree* tree,
                                                    TAMBranchInfo* brInfo );
        const Char_t* GetTreeName( ) const  { return fTName; }
        const Char_t* GetBrName( ) const    { return fBrName; }

        void SetTreeName( const Char_t* tree )   { fTName    = tree; }
        void SetBrName( const Char_t* br )       { fBrName   = br; }

        ClassDef( TSnWindTreeLoader, 2 );
};

#endif // SNS_TSnWindTreeLoader
