#ifndef SNS_TSnGeoLPDA
#define SNS_TSnGeoLPDA

#include <TGeoVolume.h>

class TSnGeoLPDA : public TGeoVolumeAssembly {
 private:
   void Build();
   
 public:
   TSnGeoLPDA() {}
   TSnGeoLPDA(const Char_t* name) : TGeoVolumeAssembly(name) { Build(); }
   virtual ~TSnGeoLPDA() {}
   
   ClassDef(TSnGeoLPDA, 1); // an LPDA geometry
};

#endif // SNS_TSnGeoLPDA
