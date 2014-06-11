#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TChain.h>
#include <TFile.h>

#include "TAMSelector.h"
#include "TAMOutput.h"

#include "TSnRawTreeMaker.h"
#include "TSnBasicEvtSelMod.h"
#include "TSnCorrelateWvs.h"
#include "NSnConstants.h"
#include "TSnCorrelateWvs.h"
#include "TSnSaveCalibDataMod.h"
#include "TSnRejectEarlyEvtsMod.h"
#include "TSnConfigTreeLoader.h"
#include "TSnRejectBadClocksMod.h"
#include "TSnSaveRecoResultsMod.h"
#include "TSnVoltageTreeLoader.h"
#include "TSnWindTreeLoader.h"
#include "TSnTimeLinkBranchLoader.h"
#include "TSnFFTCalcMod.h"
#include "TSnCalWvData.h"

#endif

TChain* nt( 0 );
TFile* fout;

void CorrelateWvs(  const Char_t* indtfl, const Char_t* intmfl,
                    const Char_t* outfl,
                    const Bool_t useThermals    = kTRUE,
                    const Bool_t useForced      = kFALSE )
{
    delete nt;

    nt = new TChain( TSnSaveCalibDataMod::kFpnEvtTreeNm );
    nt->Add( indtfl );

    TAMSelector* sel                = new TAMSelector;
    TSnBasicEvtSelMod* besmod       = new TSnBasicEvtSelMod( "BasicEvtSelMod" );

    TSnConfigTreeLoader* confLoader = new TSnConfigTreeLoader;
    TSnVoltageTreeLoader* vLoader   = new TSnVoltageTreeLoader;
//    TSnWindTreeLoader* windLoader   = new TSnWindTreeLoader( 
//        "WindTree", "WindData", TSnTimeLinkBranchLoader::kAbsoluteClosest );

    sel->AddLoader( confLoader );
    sel->AddLoader( vLoader );
//    sel->AddLoader( windLoader );
    
    if( useThermals )
    {
        besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kThermal );
    }
    if( useForced )
    {
        besmod->GetTrgBits().EnableTrig( TSnTrgInfo::kForced );
    }

    TSnRejectEarlyEvtsMod* reemod = 
                            new TSnRejectEarlyEvtsMod( "RejectEarlyEvtsMod" );

//    TSnRejectBadClocksMod* rbcmod = 
//                            new TSnRejectBadClocksMod( "RejectBadClocksMod" );
//    rbcmod->SetInclude( 0 );

//    rbcmod->SetMinTime( 1391642000 );
//    rbcmod->SetMaxTime( 1392074400 );


    besmod->SetCheckCRC( kTRUE );

    TFile* tmfl = new TFile( intmfl );
    TTree* tr = (TTree*)tmfl->Get( "Templates" );

    std::vector< Float_t > tpltwv( NSnConstants::kNsamps );
    TSnCalWvData* wv = new TSnCalWvData( "templatewave", "TSnCalWvData" );

    tr->SetBranchAddress( "wave.", &wv );
    
    tr->GetEntry( 0 );

    for( UChar_t s = 0; s < NSnConstants::kNsamps; s++ )
    {
        tpltwv[s] = static_cast< Float_t >( *(wv->GetData(0) + s) );
    }

    fout = TFile::Open( outfl, "recreate" );
    fout->SetCompressionLevel( 2 );
    fout->cd( );
    
    TSnSaveRecoResultsMod* scc = new TSnSaveRecoResultsMod( "SaveCorrCoeff",
                                "AnaVars", "Various Analysis Variables" );

    scc->SaveRecoResult( "corrcoef." );
    
    TSnFFTCalcMod* fftmod   = new TSnFFTCalcMod( "CalcFFTs", 
                                  TSnSaveCalibDataMod::kFPNSubDatBrNm, "fft" );
    TSnCorrelateWvs* cwmod  = new TSnCorrelateWvs( "corrcoef", tpltwv, "fft" );
    
//    besmod->Add( rbcmod );
//    rbcmod->Add( reemod );
/*    besmod->Add( reemod );
    reemod->Add( fftmod );
    fftmod->Add( cwmod );
    cwmod->Add( scc );
*/
    besmod->Add( reemod );
    reemod->Add( cwmod );

    sel->AddInput( besmod );
     
    Printf( "Correlating waveforms..." );

    nt->Process( sel, "" );

    Printf( "Finished waveform correlation." );

    TAMOutput* mout = sel->GetModOutput( );

    if( mout != 0 )
    {
//        fout = TFile::Open( outfl, "recreate" );
        fout->cd( );
        mout->Write( );

        delete fout;
        fout = 0;
    }
    else
    {
        Printf( "Could not get the output list from selector." );
    }

    delete sel;
}
