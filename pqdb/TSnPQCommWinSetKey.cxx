#include "TSnPQCommWinSetKey.h"

#include <TString.h>
#include <TClass.h>

#include "TSnCommWinSet.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>

ClassImp(TSnPQCommWinSetKey);

// the comm win table
const Char_t* TSnPQCommWinSetKey::kCWTbl         = "comm_window";
const Char_t* TSnPQCommWinSetKey::kCWidCol       = "id";
const Char_t* TSnPQCommWinSetKey::kCWdurCol      = "duration";
const Char_t* TSnPQCommWinSetKey::kCWperCol      = "period";
const Char_t* TSnPQCommWinSetKey::kCWsendDataCol = "send_data_option";

#if PQXX_VERSION_MAJOR<4
void TSnPQCommWinSetKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQCommWinSetKey::AddPKValToPrepSQL(
                                     pqxx::prepare::invocation& pi) const {
   pi(fId);
}


Bool_t TSnPQCommWinSetKey::IsEqual(const TObject* obj) const {
   if (obj!=0 && IsSameClass(*obj)) {
      const TSnPQCommWinSetKey* ck = 
         dynamic_cast<const TSnPQCommWinSetKey*>(obj);
      if ( ck->fId == fId ) {
         return kTRUE;
      }
   }
   return kFALSE;
}

TString TSnPQCommWinSetKey::AsString() const {
   TString s("comm win set, id=");
   s += fId;
   return s;
}

void TSnPQCommWinSetKey::GetFromPQDb(pqxx::dbtransaction& trans,
                                     pqxx::result& res) {
   TString sql(Form("SELECT %s, %s, %s FROM %s WHERE %s=",
                    kCWdurCol, kCWperCol, kCWsendDataCol,
                    kCWTbl, kCWidCol));
   AddVarToSQL(trans, sql, fId);
   res = trans.exec(sql.Data(), sql.Data());
}

TObject* TSnPQCommWinSetKey::BuildObjFromResult(const pqxx::result& res) {
   TSnCommWinSet* cw(0);
   if (res.size()==1) {
      pqxx::result::const_iterator row = res.begin();
      cw = new TSnCommWinSet(row->at(kCWperCol).as<UInt_t>(),
                             row->at(kCWdurCol).as<Int_t>(),
                             row->at(kCWsendDataCol).as<Short_t>());
   } else if (res.size()>1) {
      throw std::runtime_error(Form("<TSnPQCommWinSetKey::BuildObjFromResult>: "
                                    "Received unexpected number of rows [%u] "
                                    "for id=[%d]",
                                    static_cast<UInt_t>(res.size()), fId));
   } // else nothing found   
   return cw;
}

Bool_t TSnPQCommWinSetKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   return CheckUniquePrimKey(trans, kCWidCol, kCWTbl);
}

NSnDatabase::EWriteType TSnPQCommWinSetKey::WriteToPQDb(
                     pqxx::dbtransaction& trans, const TObject& obj) {
   
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnCommWinSet::Class())) {
      
      const TSnCommWinSet& cwset = dynamic_cast<const TSnCommWinSet&>(obj);
      if (cwset.IsValidForDb()) {
         if (FetchPrimKey(trans)) {
            // update

            throw std::runtime_error(Form("<TSnPQCommWinSetKey::WriteToPQDb>: "
                                          "This key must not be used to update an existing comm win set. "
                                          "If new values are needed, simply create a new comm win set."));
         
         } else {
            // insert
         
            TString sql(Form("INSERT INTO %s (%s, %s, %s, %s) VALUES "
                             "(DEFAULT, %u, %u, %hd) RETURNING %s",
                             kCWTbl, kCWidCol, 
                             kCWdurCol, kCWperCol, kCWsendDataCol,
                             cwset.GetPeriod(), cwset.GetDuration(),
                             cwset.GetSendData(),
                             kCWidCol));
            pqxx::result res = trans.exec(sql.Data(), sql.Data());
            trans.commit();
            fId = res.begin()->at(kCWidCol).as<Int_t>();
            wt = NSnDatabase::kInsert;
         
         }
      } else {
         throw std::runtime_error(Form("<TSnPQCommWinSetKey::WriteToPQDb>: "
            "Specified comm win is not valid for writing to DB."));
      }
   } else {
      throw std::runtime_error(Form("<TSnPQCommWinSetKey::WriteToPQDb>: "
         "Cannot write object of type [%s].",
         obj.IsA()->GetName()));
   }
   
   return wt;
}
