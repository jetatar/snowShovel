#ifndef SNS_NSnHashUtils
#define SNS_NSnHashUtils

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TString
#include <TString.h>
#endif

// only intended to be included in implementation files

namespace NSnHashUtils {
   inline
   void HashCombine(ULong_t& hash, const ULong_t& newHash) {
      // update the specified hash to a new value that combines
      // the old hash with the newHash
      hash ^= newHash + 0x9e3779b9 + (hash<<6) + (hash>>2);
   }

   inline
   void HashCombine(ULong_t& hash, const TObject& obj) {
      // update the specified hash to a new value that combines
      // the old hash with the new obj's Hash()
      HashCombine(hash, obj.Hash());
   }
   
   inline
   void HashCombine(ULong_t& hash, const TString& str) {
      HashCombine(hash, str.Hash());
   }
   
   
};

#endif // SNS_NSnHashUtils
