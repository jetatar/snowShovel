{

   gSystem->Load("libGeom");
   gSystem->Load("$SNS/lib/libgeo");
   
   TGeoManager* geom = new TGeoManager("test", "Arianna test");
   NSnGeoMaterials::DefineMaterials(*geom);
   TSnGeoStnSite* site = new TSnGeoStnSite("TestStation");
   geom->SetTopVolume(site);
   
   geom->CloseGeometry();
   
   site->Draw(/*"ogl"*/);

}
