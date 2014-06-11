#include "TSnPQDacSetKey.h"

#include <TString.h>
#include <TClass.h>

#include "TSnDacSet.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/prepared_statement>

#include <stdexcept>


ClassImp(TSnPQDacSetKey);

// the dac table
const Char_t* TSnPQDacSetKey::kDCTbl       = "dac";
const Char_t* TSnPQDacSetKey::kDCidCol     = "dac_set_id";
const Char_t* TSnPQDacSetKey::kDCchanCol   = "chan";
const Char_t* TSnPQDacSetKey::kDCvalCol    = "val";
const Char_t* TSnPQDacSetKey::kDCtypeCol   = "dac_type";

// the dac_set table
const Char_t* TSnPQDacSetKey::kDSTbl       = "dac_set";
const Char_t* TSnPQDacSetKey::kDSidCol     = "id";

// the dac_type table
const Char_t* TSnPQDacSetKey::kDTTbl       = "dac_type";
const Char_t* TSnPQDacSetKey::kDTnumCol    = "num";
const Char_t* TSnPQDacSetKey::kDTdescCol   = "description";

#if PQXX_VERSION_MAJOR<4
void TSnPQDacSetKey::AddPKToPrepSQL(
                              pqxx::prepare::declaration& pd) const {
   pd("integer");
}
#endif

void TSnPQDacSetKey::AddPKValToPrepSQL(
                              pqxx::prepare::invocation& pi) const {
   pi(fId);
}

Bool_t TSnPQDacSetKey::IsEqual(const TObject* obj) const {
   if (obj!=0 && IsSameClass(*obj)) {
      const TSnPQDacSetKey* ck = dynamic_cast<const TSnPQDacSetKey*>(obj);
      if ( ck->fId == fId ) {
         return kTRUE;
      }
   }
   return kFALSE;
}

TString TSnPQDacSetKey::AsString() const {
   TString s("dac set, id=");
   s += fId;
   return s;
}

void TSnPQDacSetKey::GetFromPQDb(pqxx::dbtransaction& trans,
                                 pqxx::result& res) {
   TString sql(Form("SELECT d.%s, d.%s, d.%s, t.%s FROM %s d, %s t WHERE d.%s=",
      kDCchanCol, kDCtypeCol, kDCvalCol, kDTdescCol, kDCTbl, kDTTbl, kDCidCol));
   AddVarToSQL(trans, sql, fId);
   sql += Form(" AND d.%s=t.%s", kDCtypeCol, kDTnumCol);
   res = trans.exec(sql.Data(), sql.Data());
}

TObject*  TSnPQDacSetKey::BuildObjFromResult(const pqxx::result& res) {
   TSnDacSet* ds = new TSnDacSet;
   UShort_t ch=0, dn=0, val=0;
   TString desc;
   pqxx::result::const_iterator row, end=res.end();
   for (row=res.begin(); row!=end; row++) {
      ch   = row->at(kDCchanCol).as<UShort_t>();
      dn   = row->at(kDCtypeCol).as<UShort_t>();
      val  = row->at(kDCvalCol).as<UShort_t>();
      const pqxx::result::field dsc = row->at(kDTdescCol);
      if (dsc.is_null()==false) {
         desc = dsc.as<const Char_t*>();
         ds->SetDac(ch, dn, val, desc.Data());
      } else {
         desc = "";
         ds->SetDac(ch, dn, val);
      }
   }
   return ds;
}

Bool_t TSnPQDacSetKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   return CheckUniquePrimKey(trans, kDSidCol, kDSTbl);
}

Bool_t TSnPQDacSetKey::IsAlreadyInDb(pqxx::dbtransaction& trans,
                                     const TSnDacSet& dacset) {
   // check if the dacset is alread in the database, and set
   // fId if so. if not, the value of fId is unchanged
   
   
   // this uses a (potentially) long sql to do the following:
   // (a) find all the dac_set_id's for the (chan, dac, num) combo of
   //     chan=0, num=0
   // (b) repeat (a) for all (chan, num) combos
   // (c) find the common dac_set_id's between all those requests (INTERSECT)
   // If no duplicates, this procedure produces a single value.
   //
   // Note that two alternative nesting procudures were observed to be slower.
   // Perhaps because for any (chan, num) combo, only the dac_set_id can
   // be common among rows with different (chan, num) combo's, so there is no
   // good way to operate on (previously selected) subsets of the table.
   
   TString sql, csub;
   Bool_t notfirst=kFALSE;
   const UInt_t nchans = dacset.GetNchans();
   for (UInt_t ch=0; ch<nchans; ch++) {
      const TSnDacSet::ChDacs_t& chdacs = dacset.GetDacsChan(ch);
      const UInt_t nds = chdacs.size();
      for (UInt_t dn=0; dn<nds; dn++) {
         
         csub = Form("SELECT %s FROM %s WHERE %s=%u and %s=%hu and %s=%u",
                     kDCidCol, kDCTbl,
                     kDCchanCol, ch,
                     kDCvalCol, chdacs[dn],
                     kDCtypeCol, dn);
         if (notfirst) {
            sql += " INTERSECT ";
         }
         sql += csub;

         notfirst=kTRUE;
         
      }
   }
   
   // do it
   pqxx::result res = trans.exec(sql.Data(), sql.Data());
   
   const UInt_t nres = res.size();
   if (nres==1) {
      fId = res.begin()->at(kDSidCol).as<Int_t>();
      SetPKcached(kTRUE);
      return kTRUE;
   } else if (nres>1) {
      throw std::runtime_error(Form("<TSnPQDacSetKey::IsAlreadyInDb>: "
         "Got [%u] duplicate dac sets. DB table needs fixing.", nres));
   }
   return kFALSE;
}

NSnDatabase::EWriteType TSnPQDacSetKey::WriteToPQDb(pqxx::dbtransaction& trans,
                                                    const TObject& obj) {
   
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   if (obj.IsA()->InheritsFrom(TSnDacSet::Class())) {
      
      const TSnDacSet& dacset = dynamic_cast<const TSnDacSet&>(obj);
      
      if (dacset.IsValidForDb()) {
         
         if (FetchPrimKey(trans)) {
            // update
            
            throw std::runtime_error(Form("<TSnPQDacSetKey::WriteToPQDb>: "
               "This key must not be used to update an existing dac set. "
               "If new values are needed, simply create a new dac set."));
            
            /* -- leave the code, in case this makes sense in the future
            TString sql;
            
            const UInt_t nchans = dacset.GetNchans();
            for (UInt_t ch=0; ch<nchans; ch++) {
               const ChDacs_t& chdacs = dacset.GetDacsChan(ch);
               TSnDacSet::ChDacs_t::const_iterator d, cend=chdacs.end();
               for (d=chdacs.begin(); d!=cend; d++) {
                  
                  // update this dac value
                  sql = Form("UPDATE %s SET %s=%hu "
                             "WHERE %s=%d and %s=%u and %s=%d",
                             kDCTbl, kDCvalCol, d->fVal,
                             kDCidCol, fId,
                             kDCchanCol, ch,
                             kDCtypeCol, d->fNum);
                  
                  trans.exec(sql.Data(), sql.Data());
                  trans.commit();
                  
               }
            }
            wt = NSnDatabase::kUpdate;
            */
            
         } else {
            // insert
            
            // check if set already exists. if so, don't write
            if (IsAlreadyInDb(trans, dacset)) {
               wt = NSnDatabase::kNotWritten;
            } else {

               TString sql;

               // first make the set id
               sql = Form("INSERT INTO %s (%s) VALUES (DEFAULT) RETURNING %s",
                  kDSTbl, kDSidCol, kDSidCol);
               pqxx::result res = trans.exec(sql.Data(), sql.Data());
               const Int_t id = res.begin()->at(kDSidCol).as<Int_t>();
               // don't cache id in case some part of this procedure fails

               // now insert the values
               sql = Form("INSERT INTO %s (%s, %s, %s, %s) VALUES ",
                          kDCTbl, kDCidCol, kDCchanCol, kDCvalCol, kDCtypeCol);
               
               Bool_t first=kTRUE;
               const UInt_t nchans = dacset.GetNchans();
               for (UInt_t ch=0; ch<nchans; ch++) {
                  const TSnDacSet::ChDacs_t& chdacs = dacset.GetDacsChan(ch);
                  const UInt_t nds = chdacs.size();
                  for (UInt_t dn=0; dn<nds; dn++) {
                     if (first==kFALSE) {
                        sql += ", ";
                     }
                     
                     sql += Form("(%d, %u, %hu, %u)",
                               id, ch, chdacs.at(dn), dn);

                     first = kFALSE;
                  }      
               }

               trans.exec(sql.Data(), sql.Data());
               trans.commit();
               fId = id;
               wt = NSnDatabase::kInsert;
               
            }
         }
         
      } else {
         throw std::runtime_error(Form("<TSnPQDacSetKey::WriteToPQDb>: "
            "Specified dac set is not valid for writing to DB."));
      }
      
   } else {
      throw std::runtime_error(Form("<TSnPQDacSetKey::WriteToPQDb>: "
         "Cannot write object of type [%s].",
         obj.IsA()->GetName()));
   }
   
   return wt;
}

