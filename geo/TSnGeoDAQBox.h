#ifndef SNS_TSnGeoDAQBox
#define SNS_TSnGeoDAQBox

#include <TGeoVolume.h>

class TSnGeoDAQBox : public TGeoVolumeAssembly {
 private:
   void Build();

 public:
   TSnGeoDAQBox() {}
   TSnGeoDAQBox(const Char_t* name) : TGeoVolumeAssembly(name) { Build(); }
   virtual ~TSnGeoDAQBox() {}

   ClassDef(TSnGeoDAQBox, 1); // a station DAQ box
};

#endif // SNS_TSnGeoDAQBox
