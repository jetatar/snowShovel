#ifndef SNS_TSnLoadFromFileMod
#define SNS_TSnLoadFromFileMod

#include <TString.h>

#include "TAModule.h"

class TSnLoadFromFileMod : public TAModule {
 private:
   TString   fFilen; // name of file with the object
   TString   fObjnm; // name of the object in the file

 protected:
   virtual void Begin();
   virtual void Terminate();

 public:
   TSnLoadFromFileMod() {}
   TSnLoadFromFileMod(const Char_t* name,
                      const Char_t* filename,
                      const Char_t* objnm) :
      TAModule(name, "Load an object from file for use during processing"),
      fFilen(filename), fObjnm(objnm) {
   }
   virtual ~TSnLoadFromFileMod() {}
   
   const Char_t* GetFilename() const { return fFilen.Data(); }
   const Char_t* GetObjName() const { return fObjnm.Data(); }
   
   void          SetFilename(const Char_t* n) { fFilen = n; }
   void          SetObjName(const Char_t* n) { fObjnm = n; }
   
   ClassDef(TSnLoadFromFileMod, 1); // load an object from a file and publish it
};

#endif // SNS_TSnLoadFromFileMod
