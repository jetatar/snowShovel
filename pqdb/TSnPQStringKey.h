#ifndef SNS_TSnPQStringKey
#define SNS_TSnPQStringKey

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
// TSnPQStringKey
//
//
// Wrapper for a TSnPQSimpleKey<TString>. This is useful because rootcint
// can't parse the pqxx headers, which are included by TSnPQSimpleKey.
// This wrapper allows the inclusion of TSnPQSimpleKey to be moved to
// the implementation, so that rootcint can build a dictionary for
// TSnPQStringKey.
//
//////////////////////////////////////////////////////////////////////////


class TSnPQStringKey : public TSnPQDbKey {
 private:
   TSnPQSimpleKey<TString>*  fSkey;

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
   TSnPQStringKey() : fSkey(0) {}
   TSnPQStringKey(const TString k, const Char_t* table,
                  const Char_t* kcol, const Char_t* vcol,
                  const NSnPQSimpleKey::EValType vtype);
   virtual ~TSnPQStringKey();
   
   virtual Bool_t        IsEqual(const TObject* obj) const;
   virtual TString       AsString() const;
   virtual TObject*      Clone(const char* newname = "") const;
   virtual Bool_t        IsPKvalid() const;
   
   
   
   ClassDef(TSnPQStringKey, 1); // a simple key to select one column using a str
};

#endif //SNS_TSnPQStringKey
