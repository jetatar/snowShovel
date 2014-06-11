#ifndef SNS_TSnPQCompMotherboardKey
#define SNS_TSnPQCompMotherboardKey

#include "TSnPQComponentKey.h"

class TSnPQCompMotherboardKey : public TSnPQComponentKey {
 private:
   Int_t       fNum;
   
   static const Char_t* kIdCol;
   static const Char_t* kNumCol;
   static const Char_t* kFpgaCol;
   static const Char_t* kDateCnstrCol;
   static const Char_t* kModelCol;
   static const Char_t* kMbdTbl;
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
   TSnPQCompMotherboardKey(const Int_t n=-1) : fNum(n) {}
   virtual ~TSnPQCompMotherboardKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   
   virtual Bool_t        IsPKvalid() const { return fNum>-1; }

   virtual const Char_t* GetCompType() const { return kCompType; }
   
   
   ClassDef(TSnPQCompMotherboardKey, 1); // key to a motherboard in the db
};

#endif // SNS_TSnPQCompMotherboardKey
