#ifndef SNS_TSnPQCompDigitizerKey
#define SNS_TSnPQCompDigitizerKey

#include "TSnPQComponentKey.h"

class TSnPQCompDigitizerKey : public TSnPQComponentKey {
 private:
   Int_t          fNum;
   
   
   static const Char_t* kIdCol;
   static const Char_t* kNumCol;
   static const Char_t* kModelCol;
   static const Char_t* kDigiTbl;
   static const Char_t* kCompType;
   
 protected:
   virtual void GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res);
   virtual NSnDatabase::EWriteType WriteToPQDb(pqxx::dbtransaction& trans, 
				       const TObject& obj);
   virtual TObject* BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t   FetchPrimKey(pqxx::dbtransaction& trans);
   
#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQCompDigitizerKey(const Int_t n=-1) : fNum(n) {}
   virtual ~TSnPQCompDigitizerKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   virtual Bool_t        IsPKvalid() const { return fNum>-1; }

   virtual const Char_t* GetCompType() const { return kCompType; }
   
   
   friend class TSnPQCompDigitizerKeyWhisperer;
   
   ClassDef(TSnPQCompDigitizerKey, 1); // key to a digitizer component in the db
};



class TSnPQCompDigitizerKeyWhisperer {
   // TSnPQCompDaughtercardKey needs more access in order to
   // build and write the linked components
   
   static
   NSnDatabase::EWriteType WriteToDb(TSnPQCompDigitizerKey& key,
                                     TSnDatabase& db, const TObject& obj) {
      return key.WriteToDb(db, obj);
   }
   
   static
   TObject* GetFromDb(TSnPQCompDigitizerKey& key, TSnDatabase& db) {
      return key.GetFromDb(db);
   }
   
   static
   void SetCompID(TSnPQCompDigitizerKey& key, const Int_t id) {
      key.SetCompID(id);
   }
   
   static
   Int_t GetCompID(const TSnPQCompDigitizerKey& key) {
      return key.GetCompID();
   }
   
   friend class TSnPQCompDaughtercardKey;
};

#endif // SNS_TSnPQCompDigitizerKey
