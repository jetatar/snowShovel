#if !defined(__CINT__) || defined(__MAKECINT__)

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

#include <vector>

#include "TSnConfigTreeLoader.h"
#include "TSnBasicEvtSelMod.h"
#include "TAMSelector.h"
#include "TAMOutput.h"
#include "TSnRejectEarlyEvtsMod.h"
#include "TSnCalWvData.h"
#include "TSnCorrelateWavesMod.h"
#include "NSnConstants.h"
#include "TSnPlotCorrelations.h"

#endif

#include "loadtree.C"


static const Char_t* dataflnm =
            "/w2/arianna/DATA/2013_Bounce_Studies/StationData/CombinedCalibTrees/CalTree.RawTree.SnEvtsM0002F7F0C61Ar00041sALL.root";

static const Char_t* tpltflnm = 
            "/data/users/jtatar/Work/Cosmic/Templates/PCD_100MHzHPF.dat.root";

static const Char_t* outflnm  = "bounce41.root";


TChain* nt( 0 );


void CorrelateWaves( void )
{
    delete nt;

    const Bool_t bOk = tryLoadDataTree( dataflnm, nt );

    if( !bOk )
    {
        Printf( "! Could not load data from: %s.\n", dataflnm );

        return;
    }

    TFile* tpltfl   = new TFile( tpltflnm );
    TTree* tptr     = (TTree*)tpltfl->Get( "Templates" );
    
    std::vector< Float_t > vtpltwv( NSnConstants::kNsamps );
    TSnCalWvData* wv = new TSnCalWvData( "TempWave", "TSnCalWvDataTemplate" );

    tptr->SetBranchAddress( "wave.", &wv );

    tptr->GetEntry( 0 );

    for( UChar_t s = 0; s < NSnConstants::kNsamps; s++ )
    {
        vtpltwv[s] = static_cast< Float_t >( *(wv->GetData(0) + s) );
    }

    
    TAMSelector* sel            = new TAMSelector;

    TSnBasicEvtSelMod* besmod   = new TSnBasicEvtSelMod( "TSnBasicEvtSelMod" );
    besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    besmod->SetCheckCRC( kTRUE ); 

    TSnRejectEarlyEvtsMod* reemod   = 
                          new TSnRejectEarlyEvtsMod( "TSnRejectEarlyEvtsMod" );
    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    sel->AddLoader( confLoader );

    
    TSnCorrelateWavesMod* cwmod = 
        new TSnCorrelateWavesMod( "TSnCorrelateWavesMod", vtpltwv );

    TSnPlotCorrelations* ccplot =
        new TSnPlotCorrelations( "TSnPlotCorrelations" );

    sel->AddInput( besmod );
    besmod->Add( reemod );
    reemod->Add( cwmod );
    cwmod->Add( ccplot );


    Printf( "Doing work..." );
    nt->Process( sel, "" );
    Printf( "Done working." );

    TAMOutput* modout = sel->GetModOutput( );
    
    if( modout != 0 )
    {
        TFile* fout = TFile::Open( outflnm, "recreate" );
        
        fout->cd( );

        modout->Write( );

        delete fout;
        fout = 0;
    }
    else
    {
        Printf( "Could not get the output list from selector." );
    }

    delete sel;    
}
