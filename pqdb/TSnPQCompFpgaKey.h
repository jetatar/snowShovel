#ifndef SNS_TSnPQCompFpgaKey
#define SNS_TSnPQCompFpgaKey

#include "TSnPQComponentKey.h"

class TSnPQCompFpgaKey : public TSnPQComponentKey {
 private:
   Int_t       fNum;
   
   static const Char_t* kIdCol;
   static const Char_t* kNumCol;
   static const Char_t* kModelCol;
   static const Char_t* kFpgaTbl;
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
   TSnPQCompFpgaKey(const Int_t n=-1) : fNum(n) {}
   virtual ~TSnPQCompFpgaKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   virtual Bool_t        IsPKvalid() const { return fNum>-1; }

   virtual const Char_t* GetCompType() const { return kCompType; }
   
   friend class TSnPQCompFpgaKeyWhisperer;
   
   ClassDef(TSnPQCompFpgaKey, 1); // key to an fpga component in the db
};



class TSnPQCompFpgaKeyWhisperer {
   // TSnPQCompDaughtercardKey and TSnPQCompMotherboardKey need
   // more access in order to build and write the linked components
   
   static
   NSnDatabase::EWriteType WriteToDb(TSnPQCompFpgaKey& key,
                                     TSnDatabase& db, const TObject& obj) {
      return key.WriteToDb(db, obj);
   }
   
   static
   TObject* GetFromDb(TSnPQCompFpgaKey& key, TSnDatabase& db) {
      return key.GetFromDb(db);
   }
   
   static
   void SetCompID(TSnPQCompFpgaKey& key, const Int_t id) {
      key.SetCompID(id);
   }
   
   static
   Int_t GetCompID(const TSnPQCompFpgaKey& key) {
      return key.GetCompID();
   }
   
   friend class TSnPQCompMotherboardKey;
   friend class TSnPQCompDaughtercardKey;
};


#endif // SNS_TSnPQCompFpgaKey
