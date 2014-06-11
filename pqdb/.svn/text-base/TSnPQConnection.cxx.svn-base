#include "TSnPQConnection.h"

#include "TSnPQTransactors.h"
#include "TSnDatabase.h"

#include <pqxx/connection>

const Char_t* TSnPQConnection::kDefConnNm   = "PQXXconnection";
const Char_t* TSnPQConnection::kPQDBprops   = "host=snowflake.ps.uci.edu "
                                              "dbname=AriannaPGDB "
                                              "port=5432";
const Char_t* TSnPQConnection::kReadUser    = "ari_read";
const Char_t* TSnPQConnection::kReadPwd     = "digger";
const Char_t* TSnPQConnection::kWriteUser   = "ari_write";
const Char_t* TSnPQConnection::kWritePwd    = "o9jw3$dsl6jn";

ClassImp(TSnPQConnection);

TSnPQConnection::TSnPQConnection(const TSnPQConnection::EPQConnType type,
                                 const Char_t* name,
                                 const Char_t* title) :
   TSnConnection(name, title),
   fConn(new pqxx::connection(GetConnStr(type))) {
}

TSnPQConnection::~TSnPQConnection() {
   if (fConn!=0 && IsOpen()) {
      Disconnect();
   }
   delete fConn;
}

Bool_t TSnPQConnection::IsOpen() const {
   return fConn->is_open();
}

void TSnPQConnection::Disconnect() {
   fConn->disconnect();
}

void TSnPQConnection::Activate() {
   fConn->activate();
}

void TSnPQConnection::Deactivate() {
   fConn->deactivate();
}

void TSnPQConnection::Perform(const TSnPQReadTransactor& tr) {
   // call perform. can't use the transactor base class because
   // of the way perform is templated.
   fConn->perform(tr);
}

void TSnPQConnection::Perform(const TSnPQWriteTransactor& tr) {
   // call perform. can't use the transactor base class because
   // of the way perform is templated.
   fConn->perform(tr);
}

TSnPQConnection* TSnPQConnection::GetOrOpenPQConn(
                                        const TSnPQConnection::EPQConnType ct,
                                        TSnDatabase& db) {
   TString cname(kDefConnNm);
   cname += GetPQConnTypeStr(ct);
   
   TSnPQConnection* pqc = dynamic_cast<TSnPQConnection*>(
      db.GetConnection(cname));
   if (pqc==0) {
      // open a new connection
      pqc = new TSnPQConnection(ct, cname.Data());
      if ( db.OpenConnection(pqc)==kFALSE ) {
         throw std::runtime_error(Form("<TSnPQConnection::GetOrOpenPQConn>: "
            "Could not open connection with type [%s], name [%s].",
            GetPQConnTypeStr(ct), cname.Data()));
      }
   } else {
      pqc->Open();
   }
   
   return pqc;
   
}


