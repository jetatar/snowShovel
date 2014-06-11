#ifndef SNS_TSnGeoStnSite
#define SNS_TSnGeoStnSite

#include <TGeoVolume.h>
class TVector3;
class TGeoNode;

class TSnGeoStnSite : public TGeoVolumeAssembly {
 private:
   void Build();
   
 public:
   TSnGeoStnSite() {}
   TSnGeoStnSite(const Char_t* name) : TGeoVolumeAssembly(name) { Build(); }
   virtual ~TSnGeoStnSite() {}
   
   const TGeoNode* GetLPDANode(const UChar_t ch) const;
   
   void            SetLPDAPosition(const UChar_t ch, TVector3& pos) const;
   
   ClassDef(TSnGeoStnSite,1); // a station site geometry (DAQ box, antennas, etc)
};

#endif // SNS_TSnGeoStnSite
