#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <TError.h>
#include <TObjArray.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TTimeStamp.h>
#include <TDataType.h>
#endif

#include <iostream>

static const UShort_t kNumAdcs     	= 16;
static const Float_t  kRcBatBox    	= 0.1;  // Ohms
static const Float_t  kRcInstrBox  	= 0.05;   // Ohms
static const Float_t  kVoltPerAdc  	= 10.0 / TMath::Power(2.,16.); // V/chan
static const Float_t  kVoltPerAdcBi	= 20.0 / TMath::Power(2.,16.); // V/chan
static const Float_t  kKperVolt    	= 100.; // Kelvin / volt
static const Float_t  kKnotPerVolt 	= 1000./16.67; // knots / volt

Float_t mean = 0;

enum EADCDef_t
{
	k0 	= 0,	//	Current 1+ (PS1- input)	// Solar ground
	k1 	= 1,	//	Current 2+ (PS2- input)	// Wind ground
	k2 	= 2,	//	Current 3+ ()
	k3 	= 3,	//	Wind +
	k4 	= 4,	//	Station Current +
	k5 	= 5,	//	Backup Battery +
	k6 	= 6,	//	HK board GND (Vicor)
	k7 	= 7,	//	T1
	k8 	= 8,	//	Current 1- (Battery GND)	// Common Ground
	k9 	= 9,	//	Current 2- (PS1- and PS2- common out)	// Wind + Sol ground
	k10 = 10,	//	Current 3-
	k11 = 11,	//	Wind - (HK GND)
	k12 = 12,	//	Station Current -
	k13 = 13,	//	HK board GND (Batt)
	k14 = 14,	//	3.3 DC-DC Vicor
	k15 = 15	//	T2
};


TChain* nt;
TFile* outf;
// Currents
TGraph* gWind, *gSol, *gWindSol, *gStation, *gTemp1, *gTemp2;
// Higher Voltages 
TGraph* gWindV, *gSolV, *gWindSolV, *gStationVp;
// Lower Voltages 
TGraph* gWindSolGND, *gStationVn;
// Measured Voltages
TGraph* gWinddV, *gSoldV, *gWindSoldV, *gStationdV;
// Anemometer
TGraph* gAnem;

void makeGraph( TGraph*& g, const Char_t* name )
{
	g = new TGraph;
	g->SetName( name );
}


void writeGraph( TGraph* g )
{
	g->Write( g->GetName() );
}


template<typename NumType>
Float_t calAnem( const NumType adc )
{	
	static const Float_t knotsPerAdc = kKnotPerVolt * kVoltPerAdc; 

	return( (adc*knotsPerAdc) );
}

template<typename NumType>
Float_t calAnemBi( const NumType adc )
{	
	static const Float_t knotsPerAdc = kKnotPerVolt * kVoltPerAdcBi; 

	return( (adc*knotsPerAdc) );
}


template<typename NumType>
Float_t calTemp( const NumType adc )
{
	static const Float_t kPerAdc = kKperVolt * kVoltPerAdc;

	return( (adc*kPerAdc) - 273. );
}


template<typename NumType>
Float_t calTempBi( const NumType adc )
{
	static const Float_t kPerAdc = kKperVolt * kVoltPerAdcBi;

	return( (adc*kPerAdc) - 273. );
}


template<typename NumType>
Float_t calCurBipol( const NumType ap, const NumType an, const Float_t rc )
{
	if( rc > 0.0 )
	{
		const Float_t vp = ap * kVoltPerAdcBi;
		const Float_t vn = an * kVoltPerAdcBi;

		return( (vp-vn) / rc );
	}

	return 0;
}

template<typename NumType>
Float_t calCur( const NumType ap, const NumType an, const Float_t rc )
{
	if( rc > 0.0 )
	{
		const Float_t vp = ap * kVoltPerAdc;
		const Float_t vn = an * kVoltPerAdc;

		return( 2.0*(vp-vn) / rc );
	}

	return 0;
}


void rhkPlot( const Char_t* infn, const Char_t* outfn )
{
	nt = new TChain( "hk" );
	nt->Add( infn );

	const Long64_t nents = nt->GetEntries();

	if( nents < 1 )
	{
		Error( "hkPlot", "No entries in chain." );

		return;
	}

	Printf( "Entries in tree = %lld", nents );

	outf = TFile::Open( outfn, "recreate" );

	UShort_t adc[kNumAdcs]	= { 0 };
	Short_t adcBi[kNumAdcs] = { 0 };

	UInt_t utime;
	TClass* c;
	EDataType dtype;

	nt->GetBranch("adc00")->GetExpectedType( c, dtype );

//	std::cout << dtype << std::endl;

	nt->SetBranchAddress( "unixTime", &utime );
	
	for( UShort_t a = 0; a < kNumAdcs; a++ )
	{
		if( dtype == 12 )
			nt->SetBranchAddress( Form("adc%02hu", a), &(adc[a]) );
		if( dtype == 2 )
			nt->SetBranchAddress( Form("adc%02hi", a), &(adcBi[a]) );
	}

	makeGraph( gWind, 		"gWind" );
	makeGraph( gSol, 		"gSol" );
	makeGraph( gWindSol,	"gWindSol" );
	makeGraph( gStation,	"gStation" );
	makeGraph( gTemp1, 		"gTemp1" );
	makeGraph( gTemp2, 		"gTemp2" );

	makeGraph( gWindV, 		"gWindV" );
	makeGraph( gSolV, 		"gSolV" );
	makeGraph( gWindSolV,	"gWindSolV" );
	makeGraph( gStationVp,	"gStationVp" );

	makeGraph( gWindSolGND,"gWindSolGND" );
	makeGraph( gStationVn,	"gStationVn" );

	makeGraph( gWinddV, 	"gWinddV" );
	makeGraph( gSoldV, 		"gSoldV" );
	makeGraph( gWindSoldV,	"gWindSoldV" );
	makeGraph( gStationdV,	"gStationdV" );

	makeGraph( gAnem, "gAnem" );

	Long64_t pts[kNumAdcs] = { 0 };

	Printf( "Processing tree ... " );

	for( Long64_t e = 0; e < nents; e++ )
	{
		nt->GetEntry( e );

		// PS1
		if( adcBi[k1] != 0 )
		{
			gWind->SetPoint( pts[k1], utime, 
				fabs(calCurBipol(adcBi[k9], adcBi[k1], kRcBatBox)) );

			gWinddV->SetPoint( pts[k1], utime,
				fabs((adcBi[k9] - adcBi[k1])*kVoltPerAdcBi) );

			gWindV->SetPoint( pts[k1], utime, fabs(adcBi[k1]*kVoltPerAdcBi) );

			pts[k1]++;
		}


		// PS2
		if( adcBi[k0] != 0 )
		{
			gSol->SetPoint( pts[k0], utime, 
				fabs(calCurBipol(adcBi[k9], adcBi[k0], kRcBatBox)) );

			gSoldV->SetPoint( pts[k0], utime, 
				fabs((adcBi[k9] - adcBi[k0])*kVoltPerAdcBi) );

			gSolV->SetPoint( pts[k0], utime, fabs(adcBi[k0]*kVoltPerAdcBi) );
	
			pts[k0]++;
		}

		// Total Current Used
		if( adcBi[k4] != 0 )
			// 2*adc values because we are using a voltage divider
		{
			gStation->SetPoint( pts[k4], utime, 
				fabs(calCurBipol(2*adcBi[k4], 2*adcBi[k12], kRcInstrBox)) );

			gStationdV->SetPoint( pts[k4], utime, 
				fabs((adcBi[k4] - adcBi[k12])*kVoltPerAdcBi) );

			gStationVp->SetPoint( pts[k4], utime, 
				fabs(adcBi[k4]*kVoltPerAdcBi) );
			gStationVn->SetPoint( pts[k4], utime, 
				fabs(adcBi[k12]*kVoltPerAdcBi) );

			pts[k4]++;
		}
		
		// Total Current IN
		if( adcBi[k9] != 0 )
		{
			gWindSol->SetPoint( pts[k9], utime, 
				fabs(calCurBipol(adcBi[k9], adcBi[k8], kRcBatBox)) );

			gWindSoldV->SetPoint( pts[k9], utime, 
				fabs((adcBi[k8] - adcBi[k9])*kVoltPerAdcBi) );

			gWindSolV->SetPoint( pts[k9], utime, 
				fabs(adcBi[k8]*kVoltPerAdcBi) );

			gWindSolGND->SetPoint( pts[k9], utime, 
				fabs(adcBi[k9]*kVoltPerAdcBi) );

			pts[k9]++;
		}

		// T1
		if( adc[k7] != 0 )
			gTemp1->SetPoint( pts[k7]++, utime, calTemp(adc[k7]) );
		if( adcBi[k7] != 0 )
			gTemp1->SetPoint( pts[k7]++, utime, calTempBi(adcBi[k7]) );

		// T2
		if( adc[k15] != 0 )
			gTemp2->SetPoint( pts[k15]++, utime, calTemp(adc[k15]) );
		if( adcBi[k15] != 0 )
			gTemp2->SetPoint( pts[k15]++, utime, calTempBi(adcBi[k15]) );

		// Anemometer
		if( adc[k3] != 0 )
			gAnem->SetPoint( pts[k3]++, utime, calAnem(adc[k3]-adc[k11]) );
		if( adcBi[k3] != 0 )
			gAnem->SetPoint( pts[k3]++, utime, calAnemBi(adcBi[k3]-adcBi[k11]) );
	}

	outf->cd();
	writeGraph( gWind );
	writeGraph( gSol );
	writeGraph( gWindSol );
	writeGraph( gStation );

	writeGraph( gWinddV );
	writeGraph( gSoldV );
	writeGraph( gWindSoldV );
	writeGraph( gStationdV);	
	
	writeGraph( gWindV );
	writeGraph( gSolV );
	writeGraph( gWindSolV );
	writeGraph( gStationVp );

	writeGraph( gWindSolGND );
	writeGraph( gStationVn );

	writeGraph( gTemp1 );
	writeGraph( gTemp2 );

	writeGraph( gAnem );

	outf->Close();
}
