#include "TSnPQDbKey.h"

#include "TSnPQConnection.h"
#include "TSnPQTransactors.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/connection>

#include <stdexcept>

ClassImp(TSnPQDbKey);


TObject* TSnPQDbKey::GetFromDb(TSnDatabase& db) {
   TObject* res=0;
   TSnPQReadTransactor reader(Form("ReadTr_%s",AsString().Data()), this, res);
   fConn = TSnPQConnection::GetOrOpenPQConn(TSnPQConnection::kRead, db);
   fConn->Perform(reader);
   return res;
}

NSnDatabase::EWriteType TSnPQDbKey::WriteToDb(TSnDatabase& db,
					      const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   TSnPQWriteTransactor writer(Form("WriteTr_%s",AsString().Data()),
                               this, obj, wt);
   //TSnPQWriteTransactor writer("PQDbKeyWriteTr", this, obj, wt);
   fConn = TSnPQConnection::GetOrOpenPQConn(TSnPQConnection::kWrite, db);
   fConn->Perform(writer);
   return wt;
}

void TSnPQDbKey::AddVarToSQL(const pqxx::dbtransaction& trans,
                             TString& sql,
                             const Char_t* var,
                             const Bool_t withQuotes) const {
   // add a string to the statement, possibly with single quotes
   if (withQuotes) {
      sql += "'";
   }

   sql += trans.esc(var);

   if (withQuotes) {
      sql += "'";
   }
}

void TSnPQDbKey::AddVarToSQL(const pqxx::dbtransaction& trans,
                             TString& sql,
                             const UChar_t* var,
                             const size_t len) const {
   // add binary data to the statement
   sql += trans.esc_raw(var, len);
}

Bool_t TSnPQDbKey::CheckUniquePrimKey(pqxx::dbtransaction& trans,
                                      const Char_t* keycol,
                                      const Char_t* table) {
   // use when the only unique identifier IS the primary key
   // so we must assume the user already knows it
   // which means here we are basically only checking that it's
   // in the database and setting the cached flag
   if (IsPKcached()) {
      return kTRUE;
   } else if (IsPKvalid()) {
      // fetch it from the DB (and cache the result)
      TString sql(Form("SELECT %s FROM %s WHERE %s=$1",
                       keycol, table, keycol));
      TString psn(Form("selPK_%s",AsString().Data()));
#if PQXX_VERSION_MAJOR<4
      TSnPQConnection::PQxxConn_t* conn = GetConnection()->GetPQConn();
      pqxx::prepare::declaration pd(conn->prepare(psn.Data(), sql.Data()));
      AddPKToPrepSQL(pd);
#endif
      pqxx::prepare::invocation pi(trans.prepared(psn.Data()));
      AddPKValToPrepSQL(pi);
      pqxx::result res = pi.exec();
      if (res.size()==1) {
         SetPKcached(kTRUE);
         return kTRUE;
      } else if (res.size()>1) {
         throw std::runtime_error(Form("<TSnPQDbKey::CheckUniquePrimKey>: "
            "Invalid number of primary key rows selected [%u]",
            static_cast<UInt_t>(res.size())));
      } // 0 rows is fine
   } // we don't know the PK -- so a write should insert
   return kFALSE;   
}

void TSnPQDbKey::ThrowOOR(const Char_t* e) {
   throw std::out_of_range(e);
}

