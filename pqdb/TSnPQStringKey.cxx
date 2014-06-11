#include "TSnPQStringKey.h"

#include "TSnPQSimpleKey.h"

ClassImp(TSnPQStringKey);


TSnPQStringKey::TSnPQStringKey(const TString k, const Char_t* table,
                               const Char_t* kcol, const Char_t* vcol,
                               const NSnPQSimpleKey::EValType vtype) :
   fSkey(new TSnPQSimpleKey<TString>(k, table, kcol, vcol, vtype)) {
}

TSnPQStringKey::~TSnPQStringKey() {
   delete fSkey;
}

#if PQXX_VERSION_MAJOR<4
void TSnPQStringKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   TSnPQSimpleKeyWhisperer::AddPKToPrepSQL(fSkey, pd);
}
#endif

void TSnPQStringKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   TSnPQSimpleKeyWhisperer::AddPKValToPrepSQL(fSkey, pi);
}

Bool_t TSnPQStringKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  return TSnPQSimpleKeyWhisperer::FetchPrimKey(fSkey, trans);
}

void TSnPQStringKey::GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res) {
   TSnPQSimpleKeyWhisperer::GetFromPQDb(fSkey, trans, res);
}

NSnDatabase::EWriteType TSnPQStringKey::WriteToPQDb(pqxx::dbtransaction& trans, 
						    const TObject& obj) {
   return TSnPQSimpleKeyWhisperer::WriteToPQDb(fSkey, trans, obj);
}

TObject* TSnPQStringKey::BuildObjFromResult(const pqxx::result& res) {
   return TSnPQSimpleKeyWhisperer::BuildObjFromResult(fSkey, res);
}

Bool_t TSnPQStringKey::IsEqual(const TObject* obj) const {
   return fSkey->IsEqual(obj);
}

TString TSnPQStringKey::AsString() const {
   return fSkey->AsString();
}

Bool_t TSnPQStringKey::IsPKvalid() const {
   return fSkey->IsPKvalid();
}

TObject* TSnPQStringKey::Clone(const char* newname) const {
   return new TSnPQStringKey(fSkey->GetPK(),
                             fSkey->GetTable(),
                             fSkey->GetKcol(),
                             fSkey->GetVcol(),
                             fSkey->GetVtype());
}
