#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TChain.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRawTreeMaker.h"
#include "TSnBasicEvtSelMod.h"
#include "NSnConstants.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnRejectEarlyEvtsMod.h"
#include "TSnVoltageTreeLoader.h"
#include "TSnRejectBadClocksMod.h"
#include "TSnHeartbeatTreeLoader.h"

#endif

#include "loadtree.C"

TChain* nt( 0 );
TFile* fout;


void CCvsVoltage( const Char_t* infl, const Char_t* outfl )
{
    delete nt;

    const Bool_t bOk = tryLoadDataTree( infl, nt );

    if( !bOk )
    {
        Printf( "! Could not load data from: %s.\n", infl );

        return;
    }

    TAMSelector* sel            = new TAMSelector;
    TSnBasicEvtSelMod* besmod   = new TSnBasicEvtSelMod( "BasicEvtSelMod" );
    TSnVoltageTreeLoader* vLoad = new TSnVoltageTreeLoader;

    sel->AddLoader( 
}
