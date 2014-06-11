#include "TSnPQShortKey.h"

#include "TSnPQSimpleKey.h"

ClassImp(TSnPQShortKey);


TSnPQShortKey::TSnPQShortKey(const Short_t k, const Char_t* table,
                             const Char_t* kcol, const Char_t* vcol,
                             const NSnPQSimpleKey::EValType vtype) :
   fSkey(new TSnPQSimpleKey<Short_t>(k, table, kcol, vcol, vtype)) {
}

TSnPQShortKey::~TSnPQShortKey() {
   delete fSkey;
}

#if PQXX_VERSION_MAJOR<4
void TSnPQShortKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   TSnPQSimpleKeyWhisperer::AddPKToPrepSQL(fSkey, pd);
}
#endif

void TSnPQShortKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   TSnPQSimpleKeyWhisperer::AddPKValToPrepSQL(fSkey, pi);
}

Bool_t TSnPQShortKey::FetchPrimKey(pqxx::dbtransaction& trans) {
  return TSnPQSimpleKeyWhisperer::FetchPrimKey(fSkey, trans);
}

void TSnPQShortKey::GetFromPQDb(pqxx::dbtransaction& trans, pqxx::result& res) {
   TSnPQSimpleKeyWhisperer::GetFromPQDb(fSkey, trans, res);
}

NSnDatabase::EWriteType TSnPQShortKey::WriteToPQDb(pqxx::dbtransaction& trans, 
						   const TObject& obj) {
   return TSnPQSimpleKeyWhisperer::WriteToPQDb(fSkey, trans, obj);
}

TObject* TSnPQShortKey::BuildObjFromResult(const pqxx::result& res) {
   return TSnPQSimpleKeyWhisperer::BuildObjFromResult(fSkey, res);
}

Bool_t TSnPQShortKey::IsEqual(const TObject* obj) const {
   return fSkey->IsEqual(obj);
}

TString TSnPQShortKey::AsString() const {
   return fSkey->AsString();
}

Bool_t TSnPQShortKey::IsPKvalid() const {
   return fSkey->IsPKvalid();
}

TObject* TSnPQShortKey::Clone(const char* newname) const {
   return new TSnPQShortKey(fSkey->GetPK(),
                            fSkey->GetTable(),
                            fSkey->GetKcol(),
                            fSkey->GetVcol(),
                            fSkey->GetVtype());
}
