#include "TSnPQAmpOnSetKey.h"

#include <TString.h>
#include <TClass.h>

#include "TSnAmpOnSet.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <sstream>
#include <stdexcept>


ClassImp(TSnPQAmpOnSetKey);


// the amp_on table
const Char_t* TSnPQAmpOnSetKey::kATbl     = "amp_on";
const Char_t* TSnPQAmpOnSetKey::kAidCol   = "set_id";
const Char_t* TSnPQAmpOnSetKey::kAchanCol = "chan";
const Char_t* TSnPQAmpOnSetKey::kAvalCol  = "val";

// the amp_on_set table
const Char_t* TSnPQAmpOnSetKey::kASTbl    = "amp_on_set";
const Char_t* TSnPQAmpOnSetKey::kASidCol  = "id";


#if PQXX_VERSION_MAJOR<4
void TSnPQAmpOnSetKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQAmpOnSetKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   pi(fId);
}

Bool_t TSnPQAmpOnSetKey::IsEqual(const TObject* obj) const {
   if (obj!=0 && IsSameClass(*obj)) {
      const TSnPQAmpOnSetKey* ck = dynamic_cast<const TSnPQAmpOnSetKey*>(obj);
      if ( ck->fId == fId ) {
         return kTRUE;
      }
   }
   return kFALSE;
}

TString TSnPQAmpOnSetKey::AsString() const {
   TString s("amp on set, id=");
   s += fId;
   return s;
}

void TSnPQAmpOnSetKey::GetFromPQDb(pqxx::dbtransaction& trans,
                                   pqxx::result& res) {
   TString sql(Form("SELECT %s, %s FROM %s WHERE %s=",
      kAchanCol, kAvalCol, kATbl, kAidCol));
   AddVarToSQL(trans, sql, fId);
   res = trans.exec(sql.Data(), sql.Data());
}

TObject* TSnPQAmpOnSetKey::BuildObjFromResult(const pqxx::result& res) {
   TSnAmpOnSet* aoset = new TSnAmpOnSet;
   pqxx::result::const_iterator row, end=res.end();
   for (row=res.begin(); row!=end; row++) {
      aoset->SetAmpOn( row->at(kAchanCol).as<UShort_t>(),
                       row->at(kAvalCol).as<Bool_t>() );
   }
   return aoset;
}

Bool_t TSnPQAmpOnSetKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   return CheckUniquePrimKey(trans, kASidCol, kASTbl);
}

Bool_t TSnPQAmpOnSetKey::IsAlreadyInDb(pqxx::dbtransaction& trans,
                                       const TSnAmpOnSet& aoset) {
   // check if the pla set is already in the DB, and set fId if so
   // if not, the value of fId is unchanged
   
   // see TSnPQDacSetKey::IsAlreadyInDb for comments on the algorithm
   
   TString sql, csub;
   Bool_t notfirst=kFALSE;
   const UInt_t namps = aoset.GetNchans();
   for (UInt_t ch=0; ch<namps; ch++) {
      
      csub = Form("SELECT %s FROM %s WHERE %s=%u and %s=",
         kAidCol, kATbl, kAchanCol, ch, kAvalCol);
      AddVarToSQL(trans, csub, GetBoolKeyword(aoset.IsAmpOn(ch)));
      
      if (notfirst) {
         sql += " INTERSECT ";
      }
      
      sql += csub;
      notfirst=kTRUE;
      
   }
   
   pqxx::result res = trans.exec(sql.Data(), sql.Data());
   
   const UInt_t nres = res.size();
   if (nres==1) {
      fId = res.begin()->at(kAidCol).as<Int_t>();
      SetPKcached(kTRUE);
      return kTRUE;
   } else if (nres>1) {
   throw std::runtime_error(Form("<TSnPQAmpOnSetKey::IsAlreadyInDb>: "
         "Got [%u] duplicate amp on sets. DB table needs fixing.", nres));
   }
   return kFALSE;
}


NSnDatabase::EWriteType TSnPQAmpOnSetKey::WriteToPQDb(
                                                    pqxx::dbtransaction& trans,
                                                    const TObject& obj) {
   
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnAmpOnSet::Class())) {
      
      const TSnAmpOnSet& aoset = dynamic_cast<const TSnAmpOnSet&>(obj);
      
      if (aoset.IsValidForDb()) {
         
         if (FetchPrimKey(trans)) {
            // update
            
            throw std::runtime_error(Form("<TSnPQAmpOnSetKey::WriteToPQDb>: "
               "This key must not be used to update an existing amp on set. "
               "If new values are needed, simply create a new amp on set."));
            

         } else {
            // insert
            
            // check if set already exists. if so, don't write
            if (IsAlreadyInDb(trans, aoset)) {
               wt = NSnDatabase::kNotWritten;
            } else {

               TString sql;

               // first make the set id
               sql = Form("INSERT INTO %s (%s) VALUES (DEFAULT) RETURNING %s",
                  kASTbl, kASidCol, kASidCol);
               pqxx::result res = trans.exec(sql.Data(), sql.Data());
               const Int_t id = res.begin()->at(kASidCol).as<Int_t>();
               // don't cache id in case some part of this procedure fails

               // now insert the values
               sql = Form("INSERT INTO %s (%s, %s, %s) VALUES ",
                          kATbl, kAidCol, kAchanCol, kAvalCol);
               
               Bool_t notfirst=kFALSE;
               const UInt_t nchans = aoset.GetNchans();
               for (UInt_t ch=0; ch<nchans; ch++) {
                  if (notfirst) {
                     sql += ", ";
                  }

                  sql += Form("(%d, %u, ", id, ch);
                  AddVarToSQL(trans, sql, GetBoolKeyword(aoset.IsAmpOn(ch)));
                  sql += ")";

                  notfirst=kTRUE;
               }

               trans.exec(sql.Data(), sql.Data());
               trans.commit();
               fId = id;
               wt = NSnDatabase::kInsert;
               
            }
         }
         
      } else {
         throw std::runtime_error(Form("<TSnPQAmpOnSetKey::WriteToPQDb>: "
            "Specified amp on set is not valid for writing to DB."));
      }
      
   } else {
      throw std::runtime_error(Form("<TSnPQAmpOnSetKey::WriteToPQDb>: "
         "Cannot write object of type [%s].",
         obj.IsA()->GetName()));
   }
   
   return wt;
}

