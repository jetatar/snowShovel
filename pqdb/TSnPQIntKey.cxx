#include "TSnPQIntKey.h"

#include "TSnPQSimpleKey.h"

ClassImp(TSnPQIntKey);


TSnPQIntKey::TSnPQIntKey(const Int_t k, const Char_t* table,
                         const Char_t* kcol, const Char_t* vcol,
                         const NSnPQSimpleKey::EValType vtype) :
   fSkey(new TSnPQSimpleKey<Int_t>(k, table, kcol, vcol, vtype)) {
}

TSnPQIntKey::~TSnPQIntKey() {
   delete fSkey;
}

#if PQXX_VERSION_MAJOR<4
void TSnPQIntKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   TSnPQSimpleKeyWhisperer::AddPKToPrepSQL(fSkey, pd);
}
#endif

void TSnPQIntKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   TSnPQSimpleKeyWhisperer::AddPKValToPrepSQL(fSkey, pi);
}

Bool_t TSnPQIntKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  return TSnPQSimpleKeyWhisperer::FetchPrimKey(fSkey, trans);
}

void TSnPQIntKey::GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res) {
   TSnPQSimpleKeyWhisperer::GetFromPQDb(fSkey, trans, res);
}

NSnDatabase::EWriteType TSnPQIntKey::WriteToPQDb(pqxx::dbtransaction& trans,
						 const TObject& obj) {
   return TSnPQSimpleKeyWhisperer::WriteToPQDb(fSkey, trans, obj);
}

TObject* TSnPQIntKey::BuildObjFromResult(const pqxx::result& res) {
   return TSnPQSimpleKeyWhisperer::BuildObjFromResult(fSkey, res);
}

Bool_t TSnPQIntKey::IsEqual(const TObject* obj) const {
   return fSkey->IsEqual(obj);
}

TString TSnPQIntKey::AsString() const {
   return fSkey->AsString();
}

Bool_t TSnPQIntKey::IsPKvalid() const {
   return fSkey->IsPKvalid();
}

TObject* TSnPQIntKey::Clone(const char* newname) const {
   return new TSnPQIntKey(fSkey->GetPK(),
                          fSkey->GetTable(),
                          fSkey->GetKcol(),
                          fSkey->GetVcol(),
                          fSkey->GetVtype());
}
