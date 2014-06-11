#include "TSnPQPlaSetKey.h"

#include <TString.h>
#include <TClass.h>

#include "TSnPlaSet.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <sstream>
#include <stdexcept>


ClassImp(TSnPQPlaSetKey);

// the pla table
const Char_t* TSnPQPlaSetKey::kPTbl        = "pla";
const Char_t* TSnPQPlaSetKey::kPidCol      = "pla_set_id";
const Char_t* TSnPQPlaSetKey::kPnumCol     = "pla_num";
const Char_t* TSnPQPlaSetKey::kPvalCol     = "val";

// the pla_num table
const Char_t* TSnPQPlaSetKey::kPNTbl       = "pla_num";
const Char_t* TSnPQPlaSetKey::kPNnumCol    = "num";

// the pla_set table
const Char_t* TSnPQPlaSetKey::kPSTbl       = "pla_set";
const Char_t* TSnPQPlaSetKey::kPSidCol     = "id";

#if PQXX_VERSION_MAJOR<4
void TSnPQPlaSetKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQPlaSetKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   pi(fId);
}

Bool_t TSnPQPlaSetKey::IsEqual(const TObject* obj) const {
   if (obj!=0 && IsSameClass(*obj)) {
      const TSnPQPlaSetKey* ck = dynamic_cast<const TSnPQPlaSetKey*>(obj);
      if ( ck->fId == fId ) {
         return kTRUE;
      }
   }
   return kFALSE;
}

TString TSnPQPlaSetKey::AsString() const {
   TString s("pla set, id=");
   s += fId;
   return s;
}

void TSnPQPlaSetKey::GetFromPQDb(pqxx::dbtransaction& trans,
                                 pqxx::result& res) {
   
   TString sql(Form("SELECT %s, %s FROM %s WHERE %s=",
      kPnumCol, kPvalCol, kPTbl, kPidCol));
   AddVarToSQL(trans, sql, fId);
   res = trans.exec(sql.Data(), sql.Data());
}

TObject* TSnPQPlaSetKey::BuildObjFromResult(const pqxx::result& res) {
   TSnPlaSet* plas = new TSnPlaSet;
   UShort_t pn=0, pv=0;
   pqxx::result::const_iterator row, end=res.end();
   for (row=res.begin(); row!=end; row++) {
      pn = row->at(kPnumCol).as<UShort_t>();
      pv = row->at(kPvalCol).as<UShort_t>();
      plas->SetPla(pn, pv);
   }
   return plas;
}

Bool_t TSnPQPlaSetKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   return CheckUniquePrimKey(trans, kPSidCol, kPSTbl);
}

Bool_t TSnPQPlaSetKey::IsAlreadyInDb(pqxx::dbtransaction& trans,
                                     const TSnPlaSet& plaset) {
   // check if the pla set is already in the DB, and set fId if so
   // if not, the value of fId is unchanged
   
   // see TSnPQDacSetKey::IsAlreadyInDb for comments on the algorithm
   
   TString sql, csub;
   Bool_t notfirst=kFALSE;
   const UInt_t nplas = plaset.GetNplas();
   for (UInt_t pn=0; pn<nplas; pn++) {
      csub = Form("SELECT %s FROM %s WHERE %s=%u and %s=%hu)",
                  kPidCol, kPTbl, kPnumCol, pn, kPvalCol, plaset.GetPla(pn));
      if (notfirst) {
         sql += " INTERSECT ";
      }
      sql += csub;
      notfirst=kTRUE;
   }
   
   pqxx::result res = trans.exec(sql.Data(), sql.Data());
   
   const UInt_t nres = res.size();
   if (nres==1) {
      fId = res.begin()->at(kPidCol).as<Int_t>();
      SetPKcached(kTRUE);
      return kTRUE;
   } else if (nres>1) {
      throw std::runtime_error(Form("<TSnPQPlaSetKey::IsAlreadyInDb>: "
         "Got [%u] duplicate pla sets. DB table needs fixing.", nres));
   }
   return kFALSE;
}

NSnDatabase::EWriteType TSnPQPlaSetKey::WriteToPQDb(pqxx::dbtransaction& trans,
                                                    const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnPlaSet::Class())) {
      const TSnPlaSet& plaset = dynamic_cast<const TSnPlaSet&>(obj);
      
      if (plaset.IsValidForDb()) {
         
         if (FetchPrimKey(trans)) {
            throw std::runtime_error(Form("<TSnPQPlaSetKey::WriteToPQDb>: "
               "This key must not be used to update an existing pla set. "
               "If new values are needed, simply create a new pla set."));
         } else {
            // insert
            
            // check if set already exists. if so, don't write
            if (IsAlreadyInDb(trans, plaset)) {
               wt = NSnDatabase::kNotWritten;
            } else {
               TString sql;

               // first make the set id
               sql = Form("INSERT INTO %s (%s) VALUES (DEFAULT) RETURNING %s",
                  kPSTbl, kPSidCol, kPSidCol);
               pqxx::result res = trans.exec(sql.Data(), sql.Data());
               const Int_t id = res.begin()->at(kPidCol).as<Int_t>();
               // don't cache id in case some part of this procedure fails

               // now insert the values
               sql = Form("INSERT INTO %s (%s, %s, %s) VALUES ",
                  kPTbl, kPidCol, kPnumCol, kPvalCol);
               Bool_t notfirst=kFALSE;
               const UInt_t nplas = plaset.GetNplas();
               for (UInt_t pn=0; pn<nplas; pn++) {
                  if (notfirst) {
                     sql += ", ";
                  }

                  sql += Form("(%d, %u, %hu)", id, pn, plaset.GetPla(pn));
                  notfirst=kTRUE;
               }

               trans.exec(sql.Data(), sql.Data());
               trans.commit();
               fId = id;
               wt = NSnDatabase::kInsert;
            }
         }
         
      } else {
         throw std::runtime_error(Form("<TSnPQDacSetKey::WriteToPQDb>: "
            "Specified pla set is not valid for writing to DB."));
      } 
   } else {
      throw std::runtime_error(Form("<TSnPQDacSetKey::WriteToPQDb>: "
         "Cannot write object of type [%s].",
         obj.IsA()->GetName()));
   }
   
   return wt;
}

   
