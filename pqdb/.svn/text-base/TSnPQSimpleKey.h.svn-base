#ifndef SNS_TSnPQSimpleKey
#define SNS_TSnPQSimpleKey

#include <TString.h>
#include <TParameter.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TClass.h>

#include "TSnPQDbKey.h"
#include "NSnPQSimpleKey.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
//
// TSnPQSimpleKey<TKEY>
//
// Generic key for a simple table with one column that serves as
// the primary key and one column that serves as the value.
// Note that the columns may be the same.
// 
// Warning: do not use string literals (char* or const char*) as a template
// parameter. Use one of the TString typedefs instead.
// 
// Note: to insert data into a table that uses an auto-incrementing index
// as the primary key, use TSnPQStringKey, with "DEFAULT" as the key.
// (DO NOT use a TSnPQIntKey for this purpose.)
//
// "Common" uses of this class have been defined:
//    TSnPQIntKey
//    TSnPQShortKey
//    TSnPQStringKey
//
//////////////////////////////////////////////////////////////////////////

template<class TKEY>
class TSnPQSimpleKey : public TSnPQDbKey {
 private:
   TKEY                       fPK;     // the primary key
   TString                    fTable;  // the db table
   TString                    fKcol;   // the primary key column name
   TString                    fVcol;   // the value colulmn name
   NSnPQSimpleKey::EValType   fVtype;  // the type of the value
   
   Bool_t IsPkEqual(const TKEY& p) const;
   void   AddValToSQL(pqxx::dbtransaction& trans, TString& sql,
                      const TObject& obj) const;
   
   TObject*  NewTParamFrom(pqxx::result::const_iterator row) const;
   
 protected:
   virtual void     GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res);
   virtual NSnDatabase::EWriteType 
            WriteToPQDb(pqxx::dbtransaction& trans, const TObject& obj);
   virtual TObject* BuildObjFromResult(const pqxx::result& res);
   virtual Bool_t   FetchPrimKey(pqxx::dbtransaction& trans);

#if PQXX_VERSION_MAJOR<4
   virtual void AddPKToPrepSQL(pqxx::prepare::declaration& pd) const;
#endif
   virtual void AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const;
   
   
 public:
   TSnPQSimpleKey() : fVtype(NSnPQSimpleKey::kUndef) {}
   TSnPQSimpleKey(const TKEY k, const Char_t* table,
                  const Char_t* kcol, const Char_t* vcol,
                  const NSnPQSimpleKey::EValType vtype) :
      fPK(k), fTable(table), fKcol(kcol), fVcol(vcol), fVtype(vtype) {}
   virtual ~TSnPQSimpleKey() {}
   
   static const Char_t* GetSQLtype();

   const TKEY&                    GetPK()    const { return fPK; }
   const Char_t*                  GetTable() const { return fTable.Data(); }
   const Char_t*                  GetKcol()  const { return fKcol.Data(); }
   const Char_t*                  GetVcol()  const { return fVcol.Data(); }
   const NSnPQSimpleKey::EValType GetVtype() const { return fVtype; }
   
   virtual Bool_t        IsEqual(const TObject* obj) const {
      if (obj!=0 && IsSameClass(*obj)) {
         const TSnPQSimpleKey<TKEY>* o = 
            dynamic_cast< const TSnPQSimpleKey<TKEY>* >(obj);
         if ( (fTable.CompareTo(o->fTable)==0) &&
              (fKcol.CompareTo(o->fKcol)==0) &&
              (fVcol.CompareTo(o->fVcol)==0) ) {
            return IsPkEqual(o->fPK);
         }
      }
      return kFALSE;
   }

   virtual TString       AsString() const;

   virtual Bool_t        IsPKvalid() const;

   TObject*      Clone(const char* newname = "") const;
   
   //ClassDef(TSnPQSimpleKey, 1); // key to a simple table with one primary key column and one result column
   
   friend class TSnPQSimpleKeyWhisperer;
};

// whisperer for the wrapper classes
class TSnPQSimpleKeyWhisperer {
   
   template<class T>
   static
   void GetFromPQDb(TSnPQSimpleKey<T>* key,
                    pqxx::dbtransaction& trans, pqxx::result& res) {
      return key->GetFromPQDb(trans, res);
   }
   
   template<class T>
   static
   TObject* BuildObjFromResult(TSnPQSimpleKey<T>* key,
                               const pqxx::result& res) {
      return key->BuildObjFromResult(res);
   }
   
   template<class T>
   static
   NSnDatabase::EWriteType WriteToPQDb(TSnPQSimpleKey<T>* key,
                    pqxx::dbtransaction& trans, const TObject& obj) {
      return key->WriteToPQDb(trans, obj);
   }
   
   template<class T>
   static
   Bool_t FetchPrimKey(TSnPQSimpleKey<T>* key,
		       pqxx::dbtransaction& trans) {
     return key->FetchPrimKey(trans);
   }

#if PQXX_VERSION_MAJOR<4
   template<class T>
   static
   void AddPKToPrepSQL(TSnPQSimpleKey<T>* key,
                       pqxx::prepare::declaration& pd) {
      key->AddPKToPrepSQL(pd);
   }
#endif

   template<class T>
   static
   void AddPKValToPrepSQL(TSnPQSimpleKey<T>* key,
                          pqxx::prepare::invocation& pi) {
      key->AddPKValToPrepSQL(pi);
   }

   // these classes can make the calls
   friend class TSnPQIntKey;
   friend class TSnPQShortKey;
   friend class TSnPQStringKey;
};


//-- "implementation" of template functions

template<class TKEY>
TObject* TSnPQSimpleKey<TKEY>::Clone(const char* newname) const {
   // need to define our own Clone function.
   // normally could use TObject::Clone, but this uses the ROOT streamer
   // facility, and because rootcint can't parse the pqxx headers,
   // we have no streamers for TSnPQSimpleKey<>'s
   
   return new TSnPQSimpleKey<TKEY>(*this);
}

template<class TKEY>
inline
Bool_t TSnPQSimpleKey<TKEY>::IsPkEqual(const TKEY& p) const {
   // simple check that the fPK's are the same using the == operator
   return fPK == p;
}

template<>
inline
Bool_t TSnPQSimpleKey<TString>::IsPkEqual(const TString& p) const {
   // specialize for TString
   return ( fPK.CompareTo(p) == 0 );
}

template<class TKEY>
TString TSnPQSimpleKey<TKEY>::AsString() const {
   // format the key to a string
   TString s(fKcol);
   s += "=[";
   s += fPK;
   s += "] to select ";
   s += fVcol;
   s += " from table [";
   s += fTable.Data();
   s += "]";
   return s;
}

template<class TKEY>
Bool_t TSnPQSimpleKey<TKEY>::IsPKvalid() const {
   // default is for integers
   return (GetPK() > -1);
}

template<>
inline
Bool_t TSnPQSimpleKey<Float_t>::IsPKvalid() const {
   return kTRUE;
}

template<>
inline
Bool_t TSnPQSimpleKey<Double_t>::IsPKvalid() const {
   return kTRUE;
}

template<>
inline
Bool_t TSnPQSimpleKey<TString>::IsPKvalid() const {
   return !(GetPK().IsNull());
}

template<class TKEY>
Bool_t TSnPQSimpleKey<TKEY>::FetchPrimKey(pqxx::dbtransaction& trans) {
  // simple key: just check that GetFromPQDb returns 1 row
  pqxx::result res;
  GetFromPQDb(trans, res);
  return res.size()==1;
}

template<class TKEY>
void TSnPQSimpleKey<TKEY>::GetFromPQDb(pqxx::dbtransaction& trans,
                                       pqxx::result& res) {
   TString sql("SELECT ");
   AddVarToSQL(trans, sql, fVcol.Data(), kFALSE);
   sql += " FROM ";
   AddVarToSQL(trans, sql, fTable.Data(), kFALSE);
   sql += " WHERE ";
   AddVarToSQL(trans, sql, fKcol.Data(), kFALSE);
   sql += "=";
   // might be different depending on the type (e.g. esc a str)
   AddVarToSQL(trans, sql, fPK);
   
   res = trans.exec(sql.Data(), sql.Data());
}

template<class TKEY>
TObject*  TSnPQSimpleKey<TKEY>::NewTParamFrom(
                                       pqxx::result::const_iterator row) const {
   if (row->size()==1) {
      // for a "simple" key, we cam have only one column
      switch (fVtype) {
         case NSnPQSimpleKey::kBool:
            return new TParameter<Bool_t>(fVcol,row->front().as<Bool_t>());
         case NSnPQSimpleKey::kShort:
            return new TParameter<Short_t>(fVcol,row->front().as<Short_t>());
         case NSnPQSimpleKey::kUShort:
            return new TParameter<UShort_t>(fVcol,row->front().as<UShort_t>());
         case NSnPQSimpleKey::kInt:
            return new TParameter<Int_t>(fVcol,row->front().as<Int_t>());
         case NSnPQSimpleKey::kUInt:
            return new TParameter<UInt_t>(fVcol,row->front().as<UInt_t>());
         case NSnPQSimpleKey::kFloat:
            return new TParameter<Float_t>(fVcol,row->front().as<Float_t>());
         case NSnPQSimpleKey::kDouble:
            return new TParameter<Double_t>(fVcol,row->front().as<Double_t>());
         case NSnPQSimpleKey::kLong64:
            return new TParameter<Long64_t>(fVcol,row->front().as<Long64_t>());
         case NSnPQSimpleKey::kULong64:
            return new TParameter<ULong64_t>(fVcol,
                                             row->front().as<ULong64_t>());
         case NSnPQSimpleKey::kString:
            return new TObjString(row->front().as<const char*>());
         default:
            throw std::runtime_error(Form("<TSnPQSimpleKey::NewTParamFrom>: "
               "Unknown Vtype [%d].",
               static_cast<Int_t>(fVtype)));
      };
   } else {
      throw std::runtime_error(Form("<TSnPQSimpleKey::NewTParamFrom>: "
         "Row has %u columns. Require 1 for simple key.",
         row->size()));
   }
   return 0;
}

template<class TKEY>
TObject* 
TSnPQSimpleKey<TKEY>::BuildObjFromResult(const pqxx::result& res) {
   if (res.size()==1) {
      // single row selected
      pqxx::result::const_iterator row = res.begin();
      return NewTParamFrom(row);
   } else {
      // multiple rows
      TObjArray* vals = new TObjArray(res.size());
      vals->SetOwner();
      pqxx::result::const_iterator row, end=res.end();
      for (row=res.begin(); row!=end; row++) {
         vals->Add(NewTParamFrom(row));
      }
      return vals;
   }
}

template<class TKEY>
NSnDatabase::EWriteType
TSnPQSimpleKey<TKEY>::WriteToPQDb(pqxx::dbtransaction& trans,
				  const TObject& obj) {
  NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
  if (FetchPrimKey(trans)) {
    TString sql("UPDATE ");
    AddVarToSQL(trans, sql, fTable, kFALSE);
    sql += " SET ";
    AddVarToSQL(trans, sql, fVcol, kFALSE);
    sql += "=";
    AddValToSQL(trans, sql, obj);
    sql += " WHERE ";
    AddVarToSQL(trans, sql, fKcol, kFALSE);
    sql += "=";
    AddVarToSQL(trans, sql, fPK);
    
    trans.exec(sql.Data(), sql.Data());
    trans.commit();
    
    wt= NSnDatabase::kUpdate;
  } else {
    TString sql("INSERT INTO ");
    AddVarToSQL(trans, sql, fTable, kFALSE);
    sql += " (";
    AddVarToSQL(trans, sql, fKcol, kFALSE);
    sql += ", ";
    AddVarToSQL(trans, sql, fVcol, kFALSE);
    sql += ") VALUES (";
    AddVarToSQL(trans, sql, fPK);
    sql += ", ";
    AddValToSQL(trans, sql, obj);
    sql += ")";
    
    trans.exec(sql.Data(), sql.Data());
    trans.commit();
    
    wt= NSnDatabase::kInsert;
  }
  
  return wt;
}

template<class TKEY>
void TSnPQSimpleKey<TKEY>::AddValToSQL(pqxx::dbtransaction& trans,
                                       TString& sql,
                                       const TObject& obj) const {
   if (fVtype==NSnPQSimpleKey::kString) {
      if (obj.IsA()->InheritsFrom(TObjString::Class())) {
         AddVarToSQL(trans, sql, obj.GetName());
      } else {
         throw std::runtime_error(
            Form("<TSnPQSimpleKey<K, TString>::AddValToSQL>: "
               "Require TObject (to be inserted) to be a TObjString,"
               " not a [%s].",
               obj.IsA()->GetName()));
      }
   } else {
      const TString cn(obj.IsA()->GetName());
      if (cn.BeginsWith("TParameter")) {
         switch (fVtype) {
            case NSnPQSimpleKey::kBool:
               sql += (dynamic_cast< const TParameter<Bool_t>& >(obj)).GetVal();
               break;
            case NSnPQSimpleKey::kShort:
               sql += (dynamic_cast< const TParameter<Short_t>& >(obj))
                  .GetVal();
               break;
            case NSnPQSimpleKey::kUShort:
               sql += (dynamic_cast< const TParameter<UShort_t>& >(obj))
                  .GetVal();
               break;
            case NSnPQSimpleKey::kInt:
               sql += (dynamic_cast< const TParameter<Int_t>& >(obj)).GetVal();
               break;
            case NSnPQSimpleKey::kUInt:
               sql += (dynamic_cast< const TParameter<UInt_t>& >(obj))
                  .GetVal();
               break;
            case NSnPQSimpleKey::kFloat:
               sql += (dynamic_cast< const TParameter<Float_t>& >(obj))
                  .GetVal();
               break;
            case NSnPQSimpleKey::kDouble:
               sql += (dynamic_cast< const TParameter<Double_t>& >(obj))
                  .GetVal();
               break;
            case NSnPQSimpleKey::kLong64:
               sql += (dynamic_cast< const TParameter<Long64_t>& >(obj))
                  .GetVal();
               break;
            case NSnPQSimpleKey::kULong64:
               sql += (dynamic_cast< const TParameter<ULong64_t>& >(obj))
                  .GetVal();
               break;
            default:
               throw std::runtime_error(Form("<TSnPQSimpleKey::NewTParamFrom>: "
                  "Unknown Vtype [%d].",
                  static_cast<Int_t>(fVtype)));
         };
      } else {
         throw std::runtime_error(Form("<TSnPQSimpleKey::AddValToSQL>: "
            "Require TObject (to be inserted) to be a TParameter, not a [%s].",
            obj.IsA()->GetName()));
      }
   }
}

template<>
inline
const Char_t* TSnPQSimpleKey<Char_t>::GetSQLtype() { return "smallint"; }
template<>
inline
const Char_t* TSnPQSimpleKey<UChar_t>::GetSQLtype() { return "smallint"; }
template<>
inline
const Char_t* TSnPQSimpleKey<Short_t>::GetSQLtype() { return "smallint"; }
template<>
inline
const Char_t* TSnPQSimpleKey<UShort_t>::GetSQLtype() { return "smallint"; }
template<>
inline
const Char_t* TSnPQSimpleKey<Int_t>::GetSQLtype() { return "integer"; }
template<>
inline
const Char_t* TSnPQSimpleKey<UInt_t>::GetSQLtype() { return "integer"; }
template<>
inline
const Char_t* TSnPQSimpleKey<Long64_t>::GetSQLtype() { return "bigint"; }
template<>
inline
const Char_t* TSnPQSimpleKey<ULong64_t>::GetSQLtype() { return "bigint"; }
template<>
inline
const Char_t* TSnPQSimpleKey<Float_t>::GetSQLtype() { return "real"; }
template<>
inline
const Char_t* TSnPQSimpleKey<Double_t>::GetSQLtype() { return "double"; }
template<>
inline
const Char_t* TSnPQSimpleKey<const Char_t*>::GetSQLtype() { return "varchar"; }
template<>
inline
const Char_t* TSnPQSimpleKey<TString>::GetSQLtype() { return "varchar"; }

#if PQXX_VERSION_MAJOR<4
template<class TKEY>
inline
void TSnPQSimpleKey<TKEY>::AddPKToPrepSQL(
                                  pqxx::prepare::declaration& pd) const {
   pd(GetSQLtype());
}

template<>
inline
void TSnPQSimpleKey<TString>::AddPKToPrepSQL(
                                  pqxx::prepare::declaration& pd) const {
   pd(GetSQLtype(), pqxx::prepare::treat_string);
}

template<>
inline
void TSnPQSimpleKey<const Char_t*>::AddPKToPrepSQL(
                                  pqxx::prepare::declaration& pd) const {
   pd(GetSQLtype(), pqxx::prepare::treat_string);
}
#endif // PQXX_VERSION_MAJOR<4

template<class TKEY>
inline
void TSnPQSimpleKey<TKEY>::AddPKValToPrepSQL(
                                  pqxx::prepare::invocation& pi) const {
   pi(fPK);
}

template<>
inline
void TSnPQSimpleKey<TString>::AddPKValToPrepSQL(
                                  pqxx::prepare::invocation& pi) const {
   pi(fPK.Data());
}


#endif // SNS_TSnPQSimpleKey
