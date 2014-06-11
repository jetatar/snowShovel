#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TObjString.h>
#include <TClass.h>

#include <TSnDatabase.h>
#include <TSnPQStringKey.h>
#endif

/*
**
** A simple example to demonstrate the use of a TSnPQSimpleKey wrapper.
** In this case, a TSnPQStringKey.
**
** For an example of using a TSnPQSimpleKey without a wrapper, see
** $SNS/prog/dbExampleStringKeyHardwareModel.cxx which makes
** $SNS/bin/dbExampleStringKeyHardwareModel.exe
**
** Be sure to load the appropriate libraries before running this macro:
**    gSystem->Load("$SNS/lib/libdb.so"); gSystem->Load("$SNS/lib/libpqdb.so");
**
**
** This macro should work in compiled or interpreted mode.
**
*/

int dbExampleStringKeyHardwareModel() {
   
   // get the database inteface
   TSnDatabase& db = TSnDatabase::GetDB();
   
   // build our key.
   // we want to know the description of the hardware model "9602-N"
   const TString model("9602-N");
   TSnPQStringKey hmodelKey(
      model.Data(),     // the key by which to select the row
      "hardware_model", // the name of the table
      "label",          // the key's column (to select from)
      "description",    // the value's column (containing the info we want)
      NSnPQSimpleKey::kString);  // the type of the return value
   
   try {
      // fetch the description from the DB
      const TObject& result = db.Get(hmodelKey);
      
      // print the result
      
      // since kString was specified as the return type,
      // the 'result' object should be a TObjString
      // let's make sure
      if (result.IsA() == TObjString::Class()) {
         const TObjString& description = dynamic_cast<const TObjString&>(result);
         Printf("%s => %s", model.Data(), description.GetName());
      } else {
         Printf("ERROR: Got unexpected result type [%s]",
            result.IsA()->GetName());
         result.Print();
      }
   } catch (const std::exception& e) {
      Printf("ERROR: %s", e.what());
   }
   
   return 0;
}
