#ifndef SNS_TSnPQIntKey
#define SNS_TSnPQIntKey

#include "NSnDatabase.h"
#include "NSnPQSimpleKey.h"
#include "TSnPQDbKey.h"

namespace pqxx {
   class dbtransaction;
   class result;
};

template<class T>
class TSnPQSimpleKey;


//////////////////////////////////////////////////////////////////////////
//
// TSnPQIntKey
//
//
// Wrapper for a TSnPQSimpleKey<Int_t>. This is useful because rootcint
// can't parse the pqxx headers, which are included by TSnPQSimpleKey.
// This wrapper allows the inclusion of TSnPQSimpleKey to be moved to
// the implementation, so that rootcint can build a dictionary for
// TSnPQIntKey.
//
//////////////////////////////////////////////////////////////////////////


class TSnPQIntKey : public TSnPQDbKey {
 private:
   TSnPQSimpleKey<Int_t>*  fSkey;
   
 protected:
   virtual void      GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res);
   virtual NSnDatabase::EWriteType
             WriteToPQDb(pqxx::dbtransaction& trans, const TObject& obj);
   virtual TObject*  BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t    FetchPrimKey(pqxx::dbtransaction& trans);
 
#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;

 public:
   TSnPQIntKey() : fSkey(0) {}
   TSnPQIntKey(const Int_t k, const Char_t* table,
               const Char_t* kcol, const Char_t* vcol,
               const NSnPQSimpleKey::EValType vtype);
   virtual ~TSnPQIntKey();
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;

   virtual Bool_t        IsPKvalid() const;

   virtual TObject*      Clone(const char* newname = "") const;
   
   
   ClassDef(TSnPQIntKey, 1); // a simple key to select one column using an int
};

#endif //SNS_TSnPQIntKey
