#ifndef SNS_TSnDbKey
#define SNS_TSnDbKey

#include <TObject.h>
#include <TString.h>
class TTimeStamp;

#include "NSnDatabase.h"
class TSnDatabase;
class TSnDbKeyWhisperer;

class TSnDbKey : public TObject {
 protected:
   // build obj from db query
   virtual TObject* GetFromDb(TSnDatabase& db)=0;
   // put info in the db
   virtual NSnDatabase::EWriteType
     WriteToDb(TSnDatabase& db, const TObject& obj)=0;

 public:
   TSnDbKey() {}
   virtual ~TSnDbKey() {}
   
   //-- functions so the key can be stored in the cache
   
   // check if the (elements of the) keys are equal
   virtual Bool_t        IsEqual(const TObject* obj) const=0;
   virtual ULong_t       Hash() const;
   
   virtual TString       AsString() const =0; // convert this key into a string
   
   // the Clone() function (with no arguments) is also used and may be
   // overridden (from TObject)
   // the format is:
   // virtual TObject*	Clone(const char* newname = "") const
   
   // a useful function to put in IsEqual before up-casting
   Bool_t IsSameClass(const TObject& obj) const {
     return obj.IsA() == IsA();
   }
   
   static const Char_t* GetSQLTimestampUTC(const TTimeStamp& ts);
   static Bool_t        GetTimestampFromStr(TTimeStamp& ts,
					    const Char_t* result);
   
   friend class TSnDbKeyWhisperer; // to preserve encapsulation
   
   ClassDef(TSnDbKey,1); // ABC for database keys
};

inline
ULong_t TSnDbKey::Hash() const {
   return AsString().Hash();
}


class TSnDbKeyWhisperer {
   // a simple interface to only allow TSnDatabase to call GetFromDb,
   // without giving TSnDatabase access to all of TSnDbKey's members
   
   static
   TObject* GetFromDb(TSnDbKey& key, TSnDatabase& db) {
      return key.GetFromDb(db);
   }
   
   static
   NSnDatabase::EWriteType WriteToDb(TSnDbKey& key,
                                     TSnDatabase& db,
                                     const TObject& obj) {
     return key.WriteToDb(db, obj);
   }
   
   friend class TSnDatabase; // only TSnDatabase is allowed access
};


#endif // SNS_TSnDbKey
