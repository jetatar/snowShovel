#ifndef SNS_TSnPQCompAntennaKey
#define SNS_TSnPQCompAntennaKey

#include "TSnPQComponentKey.h"


class TSnPQCompAntennaKey : public TSnPQComponentKey {
 private:
   Int_t                 fNum; // antenna number
  
   static const Char_t* kIdCol;
   static const Char_t* kNumCol;
   static const Char_t* kAntennaTbl;
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
   TSnPQCompAntennaKey(const Int_t num=-1) : fNum(num) {}
   virtual ~TSnPQCompAntennaKey() {}
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;

   virtual Bool_t        IsPKvalid() const { return fNum>-1; }

   virtual const Char_t* GetCompType() const { return kCompType; }
   
   ClassDef(TSnPQCompAntennaKey, 1); // key to an antenna component db entry
};

#endif // SNS_TSnPQCompAntennaKey
