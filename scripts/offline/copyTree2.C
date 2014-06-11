#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

#include "TSnPlotCCMod.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

#include <vector>
#include <utility>

#include "Riostream.h"

static const Char_t* fremain = "ic3.fftcut.remain.txt";
//static const Char_t* fevtslist  = "clusters.root";
static const Char_t* fallevts   = 
                    "/w2/jtatar/Analysis/Stn3/FPNSubTree.nt.ic3.all.root";
static const Char_t* fout       = "nt.ic3.fftcut.remain.root";


void copyTree2( void )
{
    // Open file containing pairs of run and event #
//    TFile* fevts        = new TFile( fevtslist );
    ifstream in;
    in.open( fremain );
    std::vector< UInt_t > run;
    std::vector< UInt_t > evt;
    UInt_t r;
    UInt_t e;

    while( 1 ) 
    {
        in >> r >> e;
      
        if( !in.good() ) 
            break;

//        std::cout << r << ' ' << e << std::endl;
        run.push_back( r );
        evt.push_back( e );
   }

    TFile* fdata        = new TFile( fallevts );
    TChain* nt          = new TChain( "FpnSubTree" );
    TTree* confnt       = (TTree*)fdata->Get( "ConfigTree" );

    nt->Add( fallevts );
    TSnEventHeader* hdr     = new TSnEventHeader;
    TSnEventMetadata* met   = new TSnEventMetadata;

    nt->SetBranchAddress( "EventHeader.", &hdr );
    nt->SetBranchAddress( "EventMetadata.", &met );

    TFile* flo          = TFile::Open( fout, "recreate" );
    TTree* newnt        = nt->CloneTree( 0 );
    TTree* newconfnt    = (TTree*)confnt->CloneTree( );


    const ULong64_t nevts = nt->GetEntries( );

    {
        for( ULong64_t i = 0; i < nevts; i++ )
        {
            nt->GetEntry( i );

            for( UInt_t j = 0; j < evt.size(); j++ )
            {
//                Printf( "Run: %d, Evt: %d", tsreo->GetRunNum(j), tsreo->GetEvtNum(j) );
//                std::cout << run[j] << ' ' << evt[j] << std::endl;

                if( evt[j] == hdr->GetEvtNum( ) &&
                    run[j] == met->GetRunNum( ) )
                {
                    Printf( "%d %d", run[j], evt[j] );
                    newnt->Fill( );
//                    newrawnt->Fill( );

            //        break;
                }
            }
        }
            
        flo->cd( );
        newnt->AutoSave( );
        newconfnt->AutoSave( );
    } 

    delete fdata;
    delete flo;
}
