TString inflnm  = "nt.relStnVoltNoNoise.root";
//TString outflnm = "ppl.relStnVolt.root";

static const UChar_t kNchans = 4;

void pplStationVoltages( void )
{
    gROOT->LoadMacro( "/data/users/jtatar/plotDefaults/plotDefaults.C" );
    //gROOT->LoadMacro( "plotDefaults.C" );
    smallHistDefaults( );
    gROOT->ForceStyle( );
    gStyle->SetOptFit( );

    TFile* fin = new TFile( inflnm );

    // Ratio of # number of events with parallel channels having largest
    // values vs all events.
    TH1D* hParallel     = new TH1D( "hParallel", "", 11, -0.05, 1.05 );
    TCanvas* cParallel  = new TCanvas( "cParallel", "", cwid, chit );

    TH1F*       hRatio[kNchans];
    TCanvas*    cRatio[kNchans];

    TH2F* hSlope = new TH2F( "hSlope", "", 101, -0.00005, 0.01005,                                                           101, -0.00005, 0.01005 );
    TCanvas* chSlope = new TCanvas( "chSlope", "", cwid, chit );

    // Voltages / Threshold
    TH1F* hrtt[kNchans];
    TCanvas* chrtt[kNchans];

    // Hist of the ratio of the highest two voltages.
    TH1F* hHiRatio[kNchans];
    TCanvas* chHiRatio[kNchans];

    for( UChar_t ch = 0; ch < kNchans; ch++ )
    {
        hRatio[ch] = new TH1F( Form("hRatio%d", ch), "", 51, -0.05, 5.05 );
        cRatio[ch] = new TCanvas( Form("c%d", ch), "", cwid, chit );

        hHiRatio[ch]    = new TH1F( Form("hHiRatio%d", ch), "", 11, -0.05, 1.05 );
        chHiRatio[ch]   = new TCanvas( Form("chHiRatio%d", ch), "", cwid, chit );

        hrtt[ch]    = dynamic_cast<TH1F*>( fin->Get(Form("hrtt%d", ch)) );
        chrtt[ch]   = new TCanvas( Form("chrtt%d", ch), "", cwid, chit );
    }

    Float_t voltRatio[kNchans];
    Float_t volt[kNchans];

    TTree* tr = dynamic_cast<TTree*>(fin->Get("VoltRatio"));

    tr->SetBranchAddress( "voltRatio",  &voltRatio );
    tr->SetBranchAddress( "volt",       &volt );

    ULong64_t nevts     = tr->GetEntries( );
    ULong64_t nhighpar    = 0; // number of highest two events in parallel chans

    TGraph* gSlope  = new TGraph( nevts );
    gSlope->SetName( "voltSlope" );
    TCanvas* cSlope = new TCanvas( "cSlope", "", cwid, chit );

    // Graph of highest two voltages
    TGraph* g2HiV   = new TGraph( nevts );
    g2HiV->SetName( "highestVs" );
    TCanvas* cHiV   = new TCanvas( "cHiV", "", cwid, chit );

    for( ULong64_t i = 0; i < nevts; i++ )
    {
        tr->GetEntry( i );

        // Plot highest two voltages.
        Int_t idx[kNchans];

        TMath::Sort( kNchans, volt, idx );

        // Plot ratios: V[i]/V[0], where V[0] is the highest voltage
        for( UChar_t ch = 0; ch < kNchans; ch++ )
        {
            hRatio[ch]->Fill( voltRatio[ch] );
            hHiRatio[ch]->Fill( volt[idx[ch]] / volt[idx[0]] );
        }        

        // Check if parallel channels have highest amplitues.
        if( TMath::Abs(idx[0] - idx[1]) == 2 )
        {
            nhighpar++;
        }

        // Graph voltage of two neighbouring antenna.
        if( volt[0] > volt[1] )
        {
            gSlope->SetPoint( i, volt[0], volt[1] );
            hSlope->Fill( volt[0], volt[1] );
        }
        else
        {
            gSlope->SetPoint( i, volt[1], volt[0] );
            hSlope->Fill( volt[1], volt[0] );
        }

        g2HiV->SetPoint( i, volt[idx[0]], volt[idx[1]] );
    }

    for( UChar_t ch = 0; ch < kNchans; ch++ )
    {
        cRatio[ch]->cd( );
        gPad->SetLogy( 1 );

        hRatio[ch]->Draw( );
        hRatio[ch]->SetTitle( Form(";V_{0}/V_{%d};Num. of Events", ch) );

        cRatio[ch]->Print( Form("c%d.pdf", ch) );

        // Ratio of voltages wrt highest voltage
        chHiRatio[ch]->cd( );
        hHiRatio[ch]->Draw( );
        hHiRatio[ch]->SetTitle( Form(";V_{%d}/V_{HIGHEST};Num. of Events", ch) );
        chHiRatio[ch]->Print( Form("chHiRatio%d.pdf", ch) );

        chrtt[ch]->cd( );
        hrtt[ch]->Draw( );
        hrtt[ch]->SetTitle( Form(";V_{%d}/Threshold;Num. of Events", ch) );
        chrtt[ch]->Print( Form("multiplyFactor%d.pdf", ch) );
    }    

    cSlope->cd( );
    gSlope->Draw( "AP*" );
    gSlope->SetTitle( ";V_{i} where V_{i} > V_{j};Num. of Events" );

    TF1* f1 = new TF1( "f1", "pol1", 0, 0.34 );
    gSlope->Fit( "f1" );
    cSlope->Print( "voltageSlopeNoNoise.pdf" );
    
    chSlope->cd( );
    hSlope->Draw( "colz" );
    chSlope->Print( "histVoltageSlopeNoNoise.pdf" );

    // Plot highest two voltages.
    cHiV->cd( );
    g2HiV->Draw( "AP*" );
    g2HiV->
        SetTitle( ";Highest Antenna Voltage;Second Highest Antenna Voltage;" );
    g2HiV->Print( "highest2AntVolt.pdf" );

    cParallel->cd( );
    Double_t rat = static_cast<Float_t>(nhighpar / nevts);
    hParallel->Fill( rat );
    Printf( "Number of pairs of parallel channels with highest voltages: %d, %f",
                                                    nhighpar/nevts, rat ); 
    hParallel->SetTitle( ";Ratio of Highest Evts in Parallel Chans vs All Evts" );
    cParallel->Print( "highestEvtsParallelChRatio.pdf" );

    

}
