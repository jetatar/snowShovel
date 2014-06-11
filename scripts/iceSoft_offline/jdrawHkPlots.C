TFile* inf;
// Currents
TGraph *gWind, *gSol, *gWindSol, *gStation, *gTemp1, *gTemp2;
// Higher Voltages
TGraph* gWindV, *gSolV, *gWindSolV, *gStationVp;
// Lower Voltages 
TGraph* gWindSolGND, *gStationVn;
// Measured Voltages
TGraph* gWinddV, *gSoldV, *gWindSoldV, *gStationdV;
// Anemometer
TGraph* gAnem;

TCanvas *c1, *c2, *c3, *c4, *c5;

void getGraph(TGraph*& gr, const Char_t* gname) {
   if (inf!=0) {
      gr = dynamic_cast<TGraph*>(inf->Get(gname));
   }
}

drawGraph( TGraph* gr, const Char_t* ytitle = 0 ) 
{
	gr->Draw( "AP*" );
	TH1* h = gr->GetHistogram( );

	std::cout << gr->GetMean( ) << std::endl;

	gr->Fit( "pol0", "Q", "SAME" );
	TF1* fit = gr->GetFunction( "pol0" );	
	Double_t chi2 	= fit->GetChisquare( );
	Double_t par0 	= fit->GetParameter( 0 );
	Double_t err 	= fit->GetParError( 0 );	

	TLatex* l = new TLatex;
	l->SetNDC( );
	l->DrawLatex( 0.85, 0.80, Form( "#splitline{Entries = %i}{RMS = %2.3f}",
		gr->GetN(), gr->GetMean(2)) );

	TLatex* l2 = new TLatex;
	l2->SetNDC( );
	l2->DrawLatex( 0.85, 0.7, Form("#splitline{fit = %2.3f#pm%2.3f}{#chi^{2} = %2.3f}", 
		par0, chi2, err) );
		
//	TLegend* leg = new TLegend( 0.8, 0.9, 0.95, 0.95 );
//	leg->SetFillColor( kWhite );
//	leg->SetBorderSize( 0 );
//	leg->AddEntry( "gr", "CPU+AFAR+MBED+ATWD+AMPS", "P" );
//	leg->Draw( );

	if( h != 0 ) 
	{
		h->GetXaxis()->SetTimeFormat("%m/%d %H:%M");
		h->GetXaxis()->SetNdivisions( 10, 10, 0 );
		h->GetXaxis()->SetTimeDisplay( 1 );
		h->GetXaxis()->SetTitle("Date / Time (UTC)");
		h->GetXaxis()->SetLabelSize( 0.06 );
		h->GetYaxis()->SetLabelSize(0.07);
		h->GetYaxis()->SetTitleSize(0.06);
		h->GetXaxis()->SetTitleSize(0.055);
		h->GetYaxis()->SetTitleOffset( 0.9 );
		h->GetXaxis()->SetTitleOffset( 0.9 );
		h->SetMarkerSize( 5 );

		if( ytitle != 0 ) 
		{
			h->GetYaxis()->SetTitle(ytitle);
      	}
	}
   
	if( gPad != 0 ) 
	{
		gPad->Update();
	}
}


void LineFit( TGraph* gr )
{
}


void jdrawHkPlots( const Char_t* infn, const Char_t* outfn,
					std::string skipEvts, const Int_t cwid=1400, 
					const Int_t chit=1000 ) 
{
	std::string sfin;
	std::string evtNum;
	size_t loc;
	sfin = infn;

	loc = sfin.find_last_of( "/" );

	if( loc >= 0 )
	{
		sfin.erase( sfin.begin(), sfin.begin() + loc + 1 );

		for( int i = 0; i < sfin.size(); i++ )
			if( isdigit(sfin[i]) )
				 evtNum += sfin[i];
	}
	else
	{
		evtNum = "1";
	}

	gSystem->Setenv("TZ","UTC");
	gStyle->SetOptStat( 1101 );
	gStyle->SetTimeOffset( 0 );   
	gStyle->SetPadTickY( 1 );
	gStyle->SetPadTickX( 1 );
	gStyle->SetPadLeftMargin( 0.11 );
	gStyle->SetPadRightMargin( 0.03 );
//	gStyle->SetPadBottomMargin( 0.2 );

   inf = TFile::Open(infn);
   if ((inf==0) || (inf->IsZombie())) {
      Printf("Could not open [%s]",infn);
      return;
   }
   

   		getGraph( gWind,     	"gWind" );
   		getGraph( gSol,      	"gSol" );
   		getGraph( gWindSol,		"gWindSol" );
   		getGraph( gStation,   	"gStation" );
   		getGraph( gTemp1,    	"gTemp1" );
   		getGraph( gTemp2,    	"gTemp2" );

		getGraph( gWindV,     	"gWindV" );
		getGraph( gSolV,       	"gSolV" );
		getGraph( gWindSolV,  	"gWindSolV" );
		getGraph( gStationVp,  	"gStationVp" );

		getGraph( gWindSolGND,	"gWindSolGND" );
		getGraph( gStationVn,  	"gStationVn" );

		getGraph( gWinddV,     	"gWinddV" );
		getGraph( gSoldV,       "gSoldV" );
		getGraph( gWindSoldV,  	"gWindSoldV" );
		getGraph( gStationdV,  	"gStationdV" );

		getGraph( gAnem, "gAnem" );
	

   	c1 = new TCanvas("c1","c1 - currents",cwid,chit);
   	c1->Divide(1,3);
   	c1->cd( 1 );
   	drawGraph(gSol,  "Solar Current (A)");
   	c1->cd( 2 );
	drawGraph( gSoldV, "Solar Voltage Divider Drop (V)" ); 
   	c1->cd( 3 );
	drawGraph( gSolV, "Solar Voltage High (V)" );
   	c1->Update();

   	c2 = new TCanvas("c2","c2 - others",cwid,chit);
   	c2->Divide(1,4);
   	c2->cd( 1 );
   	drawGraph(gWindSol, "Wind & Solar Current (A)");
   	c2->cd( 2 );
   	drawGraph(gWindSoldV, "Wind & Solar Voltage Drop (V)");
	c2->cd( 3 );
   	drawGraph(gWindSolV, "Wind & Solar Voltage High (V)");
	c2->cd( 4 );
	drawGraph( gWindSolGND, "Voltage Divider Common Ground (V)" );
   	c2->Update();

	c3 = new TCanvas( "c3", "c3", cwid, chit );
	c3->Divide( 1, 3 );
	c3->cd( 1 );
	drawGraph(gWind, "Wind Current (A)");
	c3->cd( 2 );
	drawGraph( gWinddV, "Wind Voltage Divider Drop (V)" );
	c3->cd( 3 );
	drawGraph( gWindV, "Wind Voltage High (V)" );
   	c3->Update();

	c4 = new TCanvas( "c4", "c4", cwid, chit );
	c4->Divide( 1, 3 );
	c4->cd( 1 );
   	drawGraph( gStation, "Station Current (A)" );
	c4->cd( 2 );
   	drawGraph( gStationVn, "Station Voltage / 2 (V)" );
	c4->cd( 3 );
   	drawGraph( gStationdV, "Station Voltage Drop (V)" );		
   	c4->Update();

	c5 = new TCanvas( "c5", "c5", cwid, chit );
	c5->Divide( 1, 3 );
	c5->cd( 1 );
   	drawGraph(gTemp1, "Temp 1 - Solar (C)");
	c5->cd( 2 );
   	drawGraph(gTemp2, "Temp 2 - Battery (C)");
	c5->cd( 3 );
	drawGraph( gAnem, "Wind Speed in Knots" );
   	c5->Update();

//	c6 = new TCanvas( "c6", "c6", cwid, chit );
		

    TFile* fCommon = new TFile( outfn, "UPDATE" );

	fCommon->cd( );

	gWind->Write( "", TObject::kOverwrite );
	gWindV->Write( "", TObject::kOverwrite );
	gWinddV->Write( "", TObject::kOverwrite );
	gSol->Write( "", TObject::kOverwrite );
	gSolV->Write( "", TObject::kOverwrite );
	gSoldV->Write( "", TObject::kOverwrite );
	gWindSol->Write( "", TObject::kOverwrite );	
	gWindSolV->Write( "", TObject::kOverwrite );	
	gWindSoldV->Write( "", TObject::kOverwrite );	
	gWindSolGND->Write( "", TObject::kOverwrite );	
	gStation->Write( "", TObject::kOverwrite );
	gStationVn->Write( "", TObject::kOverwrite );
	gStationdV->Write( "", TObject::kOverwrite );
	gTemp1->Write( "", TObject::kOverwrite );
	gTemp2->Write( "", TObject::kOverwrite );
	gAnem->Write( "", TObject::kOverwrite );

	fCommon->Write( );
	fCommon->Close( );

	Char_t* dir = "CPU+AFAR+MBED+ATWD+AMPS";
	c1->SaveAs( Form("~/%s/c1.pdf", dir) );
	c2->SaveAs( Form("~/%s/c2.pdf", dir) );
	c3->SaveAs( Form("~/%s/c3.pdf", dir) );
	c4->SaveAs( Form("~/%s/c4.pdf", dir) );
	c5->SaveAs( Form("~/%s/c5.pdf", dir) );
//	c6->SaveAs( Form("~/%s/c6.pdf", dir) );

/*
	if( evtNum.size() != 0 )
	{
		c2->SaveAs( Form("voltage_%s.pdf", evtNum.c_str()) );
	}
*/
}
