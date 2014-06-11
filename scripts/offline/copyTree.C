#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

#include "TSnPlotCCMod.h"
#include "TSnEventHeader.h"
#include "TSnEventMetadata.h"

#include <vector>
#include <utility>


stati const Char_t fremain = "remain.txt";
//static const Char_t* fevtslist  = "clusters.root";
static const Char_t* fallevts   = 
                    "/w2/jtatar/Analysis/Stn3/FPNSubTree.nt.ic3.all.ww.root";
static const Char_t* fout       = "nt.aftercuts.root";


void copyTree( void )
{
    // Open file containing pairs of run and event #
    TFile* fevts        = new TFile( fevtslist );
    TSnRunEvtOut* tsreo = (TSnRunEvtOut*)fevts->Get( "TSnRunEvtOut" );

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

    if( tsreo != 0 )
    {
        std::vector< std::pair<UInt_t, UInt_t> >& re = tsreo->Get( );

        for( ULong64_t i = 0; i < nevts; i++ )
        {
            nt->GetEntry( i );

            for( UInt_t j = 0; j < re.size(); j++ )
            {
//                Printf( "Run: %d, Evt: %d", tsreo->GetRunNum(j), tsreo->GetEvtNum(j) );

                if( tsreo->GetEvtNum(j) == hdr->GetEvtNum( ) &&
                    tsreo->GetRunNum(j) == met->GetRunNum( ) )
                {
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

    delete fevts;
    delete fdata;
    delete flo;
}
