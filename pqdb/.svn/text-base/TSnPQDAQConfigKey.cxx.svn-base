#include "TSnPQDAQConfigKey.h"

#include <TString.h>
#include <TClass.h>
#include <TTimeStamp.h>

#include "TSnDatabase.h"
#include "TSnDAQConfig.h"

#include "TSnPQDacSetKey.h"
#include "TSnPQPlaSetKey.h"
#include "TSnPQTrigSetupKey.h"
#include "TSnPQAmpOnSetKey.h"
#include "TSnPQCommWinSetKey.h"
#include "TSnPQConnection.h"

#include <pqxx/transaction>
#include <pqxx/result>
#include <pqxx/connection>
#include <pqxx/prepared_statement>

#include <sstream>
#include <stdexcept>

ClassImp(TSnPQDAQConfigKey);

// the daq_config table
const Char_t* TSnPQDAQConfigKey::kDConfTbl        = "daq_config";
const Char_t* TSnPQDAQConfigKey::kLblCol          = "label";
const Char_t* TSnPQDAQConfigKey::kDateBltCol      = "date_built";
const Char_t* TSnPQDAQConfigKey::kUsageCol        = "usage";
const Char_t* TSnPQDAQConfigKey::kUserCol         = "username";
const Char_t* TSnPQDAQConfigKey::kDacIdCol        = "dac_set";
const Char_t* TSnPQDAQConfigKey::kPlaIdCol        = "pla_set";
const Char_t* TSnPQDAQConfigKey::kTrgIdCol        = "trigger_setup";
const Char_t* TSnPQDAQConfigKey::kCommIdCol       = "comm_window";
const Char_t* TSnPQDAQConfigKey::kAmpIdCol        = "amp_on_set";
const Char_t* TSnPQDAQConfigKey::kDescCol         = "description";
const Char_t* TSnPQDAQConfigKey::kLPCommIdCol     = "low_pwr_comm_window";
const Char_t* TSnPQDAQConfigKey::kRunModCol       = "run_mode";
const Char_t* TSnPQDAQConfigKey::kHrtBtCol        = "heartbeat_period";
const Char_t* TSnPQDAQConfigKey::kStrmHiLoCol     = "stream_hi_lo_plas";
const Char_t* TSnPQDAQConfigKey::kWvLoseLSBCol    = "wv_lose_low_sig_bits";
const Char_t* TSnPQDAQConfigKey::kWvLoseMSBCol    = "wv_lose_high_sig_bits";
const Char_t* TSnPQDAQConfigKey::kWvBaselineCol   = "wv_baseline";
const Char_t* TSnPQDAQConfigKey::kDatPackCol      = "data_packing";
const Char_t* TSnPQDAQConfigKey::kPwrModeCol      = "power_mode";
const Char_t* TSnPQDAQConfigKey::kBVToLowPwrCol   = "bat_volt_to_low_power";
const Char_t* TSnPQDAQConfigKey::kBVFromLowPwrCol = "bat_volt_from_low_power";
const Char_t* TSnPQDAQConfigKey::kWchDogCol       = "watchdog_period";
const Char_t* TSnPQDAQConfigKey::kVoltChkPerCol   = "voltage_check_period";

#if PQXX_VERSION_MAJOR<4
void TSnPQDAQConfigKey::AddPKToPrepSQL(pqxx::prepare::declaration& pd) const {
   pd("varchar", pqxx::prepare::treat_string);
}
#endif

void TSnPQDAQConfigKey::AddPKValToPrepSQL(pqxx::prepare::invocation& pi) const {
   pi(fLabel.Data());
}

Bool_t TSnPQDAQConfigKey::IsEqual(const TObject* obj) const {
   if (obj!=0 && IsSameClass(*obj)) {
      const TSnPQDAQConfigKey* k = dynamic_cast<const TSnPQDAQConfigKey*>(obj);
      if ( k->fLabel.CompareTo(fLabel.Data())==0 ) {
         return kTRUE;
      }
   }
   return kFALSE;
}

TString TSnPQDAQConfigKey::AsString() const {
   TString s("daq config: ");
   s += fLabel.Data();
   return s;
}

void TSnPQDAQConfigKey::GetFromPQDb(pqxx::dbtransaction& trans,
                                    pqxx::result& res) {
   // use the specified label to get the complete daq_config from the db
   
   // here we just get the IDs for the various pieces that
   // aren't directly contained in the daq_config table...
   // except for the comm win, for which no separate key & dbdat
   // class exists
   TSnPQConnection::PQxxConn_t* conn = GetConnection()->GetPQConn();
   TString sql(Form("SELECT %s, %s, %s, %s, "
                    "%s, %s, "
                    "%s, %s, %s, %s, %s, "
                    "%s, %s, %s, %s, "
                    "%s, %s, %s, %s, "
                    "%s, %s, %s "
                    "FROM %s WHERE %s=$1",
                    // first the "sets"
                    kDacIdCol, kPlaIdCol, kTrgIdCol, kAmpIdCol, 
                    kCommIdCol, kLPCommIdCol,
                    // then the individual variables
                    kDateBltCol, kUsageCol, kUserCol, kDescCol, kRunModCol,
                    kHrtBtCol, kStrmHiLoCol, kWvLoseLSBCol, kWvLoseMSBCol,
                    kWvBaselineCol, kDatPackCol, kPwrModeCol, kBVToLowPwrCol,
                    kBVFromLowPwrCol, kVoltChkPerCol, kWchDogCol,
                    // from, where
                    kDConfTbl, kLblCol));
   conn->prepare("getDAQConfig",sql.Data())
#if PQXX_VERSION_MAJOR<4
      ("varchar", pqxx::prepare::treat_string)
#endif
      ;
   res = trans.prepared("getDAQConfig")(fLabel.Data()).exec();
   
}


NSnDatabase::EWriteType
   TSnPQDAQConfigKey::WriteToPQDb(pqxx::dbtransaction& trans,
                                  const TObject& obj) {
   NSnDatabase::EWriteType wt = NSnDatabase::kWrtUndef;
   
   if (obj.IsA()->InheritsFrom(TSnDAQConfig::Class())) {
      const TSnDAQConfig& conf = dynamic_cast<const TSnDAQConfig&>(obj);
      if (conf.IsValidForDb()) {
         if (FetchPrimKey(trans)) {
            // update
            
            throw std::runtime_error(Form("<TSnPQDAQConfigKey::WriteToPQDb>: "
               "This key must not be used to update an existing DAQ config. "
               "If new values are needed, simply create a new DAQ config."));
            
         } else {
            // insert
            
            // if label isn't in the DB, then this config isn't in the DB,
            // even if all the other parameters are the same. so no need to
            // check if there's another config that has all the same params
            
            // insert subkeys
            TSnDatabase& db = TSnDatabase::GetDB();
            // dac set
            TSnPQDacSetKey dackey;
            db.WriteNoCache(dackey, conf.GetDacSet());
            // pla set
            TSnPQPlaSetKey plakey;
            db.WriteNoCache(plakey, conf.GetPlaSet());
            // trigger
            TSnPQTrigSetupKey trgkey;
            db.WriteNoCache(trgkey, conf.GetTrigSetup());
            // amp set
            TSnPQAmpOnSetKey ampkey;
            db.WriteNoCache(ampkey, conf.GetAmpOnSet());
            // comm wins
            TSnPQCommWinSetKey cwkey, lpcwkey;
            db.WriteNoCache(cwkey, conf.GetCommWin());
            db.WriteNoCache(lpcwkey, conf.GetLowPwrCommWin());
            
            // now the daq config table
            TString sql(Form("INSERT INTO %s "
                             "(%s, %s, %s, %s, "
                             "%s, %s, %s, %s, "
                             "%s, %s, %s, %s, "
                             "%s, %s, "
                             "%s, %s, %s, "
                             "%s, %s, "
                             "%s, %s, "
                             "%s, %s) VALUES "
                             "($1, $2, $3, $4, "
                             "$5, $6, $7, $8, "
                             "$9, $10, $11, $12, "
                             "$13, $14, "
                             "$15, $16, $17, "
                             "$18, $19, "
                             "$20, $21, "
                             "$22, $23) ",
                             kDConfTbl,
                             kLblCol, kDateBltCol, kUsageCol, kUserCol,
                             kDacIdCol, kPlaIdCol, kTrgIdCol, kCommIdCol,
                             kAmpIdCol, kDescCol, kLPCommIdCol, kRunModCol,
                             kHrtBtCol, kStrmHiLoCol, 
                             kWvLoseLSBCol, kWvLoseMSBCol, kWvBaselineCol,
                             kDatPackCol, kPwrModeCol, 
                             kBVToLowPwrCol, kBVFromLowPwrCol,
                             kWchDogCol, kVoltChkPerCol));
            TSnPQConnection::PQxxConn_t* conn = GetConnection()->GetPQConn();
            conn->prepare("insDAQConfig",sql.Data())
#if PQXX_VERSION_MAJOR<4
               ("varchar", pqxx::prepare::treat_string)
               ("timestamp", pqxx::prepare::treat_string)
               ("varchar", pqxx::prepare::treat_string)
               ("varchar", pqxx::prepare::treat_string)
               ("integer")("integer")("integer")("integer")
               ("integer")
               ("varchar", pqxx::prepare::treat_string)
               ("integer")
               ("smallint")
               ("integer")("boolean")
               ("smallint")("smallint")("smallint")
               ("smallint")("smallint")
               ("smallint")("smallint")
               ("integer")("integer")
#endif               
               ;
            trans.prepared("insDAQConfig")
               (conf.GetLabel())
               (GetSQLTimestampUTC(conf.GetDateBuilt()))
               (conf.GetUsage())
               (conf.GetUser())
               (dackey.GetId())(plakey.GetId())(trgkey.GetId())(cwkey.GetId())
               (ampkey.GetId())
               (conf.GetDescription())
               (lpcwkey.GetId())
               (static_cast<Short_t>(conf.GetRunMode()))
               (conf.GetHeartBeatPeriod())
               (conf.IsStreamingHiLoPlas())
               (static_cast<Short_t>(conf.GetWvLoseLSB()))
               (static_cast<Short_t>(conf.GetWvLoseMSB()))(conf.GetWvBaseline())
               (static_cast<Short_t>(conf.GetDatPackType()))
               (static_cast<Short_t>(conf.GetPowerMode()))
               (conf.GetBatVoltToLowPwr())(conf.GetBatVoltFromLowPwr())
               (conf.GetWatchdogPeriod())(conf.GetVoltCheckPeriod())
               .exec();
            // the casts to Short_t are because pqxx can't handle 8bit integers
            trans.commit();
            fLabel = conf.GetLabel();
            wt = NSnDatabase::kInsert;
            
         }
      } else {
         throw std::runtime_error(Form("<TSnPQDAQConfigKey::WriteToPQDb>: "
            "Specified DAQ config is not valid for writing to DB."));
      }
   } else {
      throw std::runtime_error(Form("<TSnPQDAQConfigKey::WriteToPQDb>: "
         "Cannot write object of type [%s].",
         obj.IsA()->GetName()));
   }
   
   return wt;
}

TObject*  TSnPQDAQConfigKey::BuildObjFromResult(const pqxx::result& res) {
   // use sub-keys to build the full config.
   // objects generated by the subkeys will be removed from the cache.
   
   TSnDAQConfig* conf(0);
   
   if (res.size()==1) {
      TSnDatabase& db = TSnDatabase::GetDB();
      pqxx::result::const_iterator row = res.begin();
      // dac set
      const Int_t dacid = row->at(kDacIdCol).as<Int_t>();
      TSnPQDacSetKey dackey(dacid);
      const TSnDacSet& dacset = dynamic_cast<const TSnDacSet&>(db.Get(dackey));
      // pla set
      const Int_t plaid = row->at(kPlaIdCol).as<Int_t>();
      TSnPQPlaSetKey plakey(plaid);
      const TSnPlaSet& plaset = dynamic_cast<const TSnPlaSet&>(db.Get(plakey));
      // trigger
      const Int_t trgid = row->at(kTrgIdCol).as<Int_t>();
      TSnPQTrigSetupKey trgkey(trgid);
      const TSnTriggerSetup& trgset = dynamic_cast<const TSnTriggerSetup&>(
         db.Get(trgkey));
      // amp set
      const Int_t ampid = row->at(kAmpIdCol).as<Int_t>();
      TSnPQAmpOnSetKey ampkey(ampid);
      const TSnAmpOnSet& ampset = dynamic_cast<const TSnAmpOnSet&>(
         db.Get(ampkey));
      // comm windows
      const Int_t cwid = row->at(kCommIdCol).as<Int_t>();
      TSnPQCommWinSetKey cwkey(cwid);
      const TSnCommWinSet& cwset = dynamic_cast<const TSnCommWinSet&>(
         db.Get(cwkey));
      const Int_t lpcwid = row->at(kLPCommIdCol).as<Int_t>();
      TSnPQCommWinSetKey lpcwkey(lpcwid);
      const TSnCommWinSet& lpcwset = dynamic_cast<const TSnCommWinSet&>(
         db.Get(lpcwkey));
      // date built
      TTimeStamp datb;
      GetTimestampFromStr(datb,
                          row->at(kDateBltCol).as<const Char_t*>());
      // misc pieces. collect here to check casts, and so an exception
      // might get thrown before the new config
      TString usage(row->at(kUsageCol).as<const Char_t*>());
      TString user(row->at(kUserCol).as<const Char_t*>());
      TString desc(row->at(kDescCol).as<const Char_t*>());
      const UChar_t  runmod(SafeUChar_cast(row->at(kRunModCol).as<UShort_t>()));
      const UShort_t hrtbt(row->at(kHrtBtCol).as<UShort_t>());
      const Bool_t   strhl(row->at(kStrmHiLoCol).as<Bool_t>());
      const UChar_t  loselsb(SafeUChar_cast(
                                row->at(kWvLoseLSBCol).as<UShort_t>()));
      const UChar_t  losemsb(SafeUChar_cast(
                                row->at(kWvLoseMSBCol).as<UShort_t>()));
      const UShort_t wvbasel(row->at(kWvBaselineCol).as<UShort_t>());
      const UChar_t  datpak(SafeUChar_cast(row->at(kDatPackCol).as<UShort_t>()));
      const UChar_t  pwrmod(SafeUChar_cast(row->at(kPwrModeCol).as<UShort_t>()));
      const Short_t  voltToLp(row->at(kBVToLowPwrCol).as<Short_t>());
      const Short_t  voltFromLp(row->at(kBVFromLowPwrCol).as<Short_t>());
      const UShort_t vchkper(row->at(kVoltChkPerCol).as<UShort_t>());
      const UInt_t   wchdog(row->at(kWchDogCol).as<UInt_t>());

      // make the config
      conf = new TSnDAQConfig;
      conf->SetLabel(fLabel.Data());
      conf->SetUsage(usage.Data());
      conf->SetUser(user.Data());
      conf->SetDescription(desc.Data());
      conf->SetDateBuilt(datb);
      conf->SetDacSet(dacset);
      conf->SetPlaSet(plaset);
      conf->SetTrigSetup(trgset); // logic, throttle, therm on, forced per
      conf->SetAmpOnSet(ampset);
      conf->SetCommWinSet(cwset);
      conf->SetLowPwrCommWinSet(lpcwset);
      conf->SetRunMode(runmod);
      conf->SetHeartbeatPeriod(hrtbt);
      conf->SetStreamHiLoPlas(strhl);
      conf->SetWvLoseLSB(loselsb);
      conf->SetWvLoseMSB(losemsb);
      conf->SetWvBaseline(wvbasel);
      conf->SetDatPackType(datpak);
      conf->SetPowerMode(pwrmod);
      conf->SetBatVoltToLowPwr(voltToLp);
      conf->SetBatVoltFromLowPwr(voltFromLp);
      conf->SetVoltCheckPeriod(vchkper);
      conf->SetWatchdogPeriod(wchdog);
      
      // cleanup redundant pieces
      delete db.Reclaim(dackey);
      delete db.Reclaim(plakey);
      delete db.Reclaim(trgkey);
      delete db.Reclaim(ampkey);
      delete db.Reclaim(cwkey);
      delete db.Reclaim(lpcwkey);
      
   } else if (res.size()>1) {
      throw std::runtime_error(Form("<TSnPQDAQConfigKey::BuildObjFromResult>: "
                                    "Unexpected number of rows returned from "
                                    "DB: [%u]",
                                    static_cast<UInt_t>(res.size())));
   } // else not in DB
   
   return conf;
}

Bool_t    TSnPQDAQConfigKey::FetchPrimKey(pqxx::dbtransaction& trans) {
   return CheckUniquePrimKey(trans, kLblCol, kDConfTbl);
}
   
   
