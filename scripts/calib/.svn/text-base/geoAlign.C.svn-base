{
   
   const Int_t cols[4] = { kOrange+7, kAzure-2, kViolet-1, kGreen+2 };
   
   gSystem->Load("libGeom");
   gSystem->Load("$SNS/lib/libgeo");
   
   // build the station site geometry
   TString stnnm = "CampSite2013";
   TGeoManager* geom = new TGeoManager("CampSite2013", 
                                       "Arianna site at camp 2013");
   NSnGeoMaterials::DefineMaterials(*geom);
   TSnGeoStnSite* site = new TSnGeoStnSite(stnnm.Data());
   geom->SetTopVolume(site);
   
   // get the physical LPDA nodes
   TGeoPhysicalNode* lpda[4];
   TString path;
   for (Int_t ch=0; ch<4; ++ch) {
      path = Form("/%s_1/%s_LPDA_%d",stnnm.Data(),stnnm.Data(),ch+1);
      lpda[ch] = new TGeoPhysicalNode(path.Data());
   }
   
   // re-align them with some vertical offsets
   TGeoTranslation ch0("Ch0Trans", 0.0,  3.0, -0.5);
   TGeoTranslation ch1("Ch1Trans", 3.0,  0.0, -0.5 + 0.055);
   TGeoTranslation ch2("Ch2Trans", 0.0, -3.0, -0.5 - 0.09);
   TGeoTranslation ch3("Ch3Trans",-3.0,  0.0, -0.5 + 0.03);
   TGeoRotation ch1r("Ch1Rot", 90.0, 0.0, 0.0);
   lpda[0]->Align(new TGeoCombiTrans(ch0, *gGeoIdentity));
   lpda[1]->Align(new TGeoCombiTrans(ch1, ch1r));
   lpda[2]->Align(new TGeoCombiTrans(ch2, *gGeoIdentity));
   lpda[3]->Align(new TGeoCombiTrans(ch3, ch1r));
   
   geom->CloseGeometry();
   
   site->Draw();

}
