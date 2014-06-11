#include "TSnPQTransactors.h"

#include "TSnPQDbKey.h" // for the whisperers

void TSnPQReadTransactor::operator() (pqxx::work& trans) {
   // call GetFromPQDb
   if (IsKeyOk()) {
      TSnPQDbKeyReadWhisperer::GetFromPQDb(fDbKey, trans, fRes);
   }
}

void TSnPQReadTransactor::on_commit() {
   // call BuildObjFromResult
   fObj = TSnPQDbKeyReadWhisperer::BuildObjFromResult(fDbKey, fRes);
}
   
void TSnPQWriteTransactor::operator() (pqxx::work& trans) {
   // call WriteToPQDb
   if (IsKeyOk()) {
      fWt = TSnPQDbKeyWriteWhisperer::WriteToPQDb(fDbKey, trans, *fObj);
   }
}
