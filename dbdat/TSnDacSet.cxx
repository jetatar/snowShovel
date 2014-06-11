#include "TSnDacSet.h"

ClassImp(TSnDacSet);


void TSnDacSet::SetDac(const DacSet_t::size_type ch,
                       const ChDacs_t::size_type dn,
                       const UShort_t dac) {
   if (GetNchans()<=ch) {
      fDacs.resize(ch+1);
   }
   ChDacs_t& cds = fDacs.at(ch);
   if (cds.size()<=dn) {
      cds.resize(dn+1);
   }
   cds.at(dn) = dac;
   
}

void TSnDacSet::SetDac(const DacSet_t::size_type ch,
                       const ChDacs_t::size_type dn,
                       const UShort_t dac,
                       const Char_t*  type) {
   SetDac(ch, dn, dac);
   if (fDtypes.size()<=dn) {
      fDtypes.resize(dn+1);
   }
   fDtypes.at(dn) = TString(type);
}

void TSnDacSet::Print(Option_t* option) const {
   const DacSet_t::size_type nch = GetNchans();
   for (UInt_t ch=0; ch<nch; ch++) {
      printf("    ch%u: [",ch);
      const ChDacs_t& chd = GetDacsChan(ch);
      ChDacs_t::const_iterator d, end=chd.end();
      DacTypes_t::const_iterator t=fDtypes.begin(),
	tend = fDtypes.end();
      for (d=chd.begin(); d!=end;) {
 	 printf("%hu", *d);
         d++;
	 if (t!=tend) {
	    printf(" (%s)", t->Data());
	    t++;
	 }
         if (d!=end) {
            printf(", ");
         }
      }
      printf("]\n");
   }
}

Bool_t TSnDacSet::IsValidForDb() const {
   // check that there are at least 1 channel
   // and that each channel has the same number of dacs.
   
   const DacSet_t::size_type nchans = GetNchans();
   if (nchans>0) {
      const ChDacs_t::size_type ndacs = fDacs.at(0).size();
      Bool_t allsame=kTRUE;
      for (DacSet_t::size_type i=1; i<nchans; i++) {
         if (fDacs.at(i).size()!=ndacs) {
            allsame=kFALSE;
            break;
         }
         if (allsame) {
            return kTRUE;
         }
      }
   }
   return kFALSE;
}

