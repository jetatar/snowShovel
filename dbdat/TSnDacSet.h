#ifndef SNS_TSnDacSet
#define SNS_TSnDacSet

#include <TObject.h>
#include <TString.h>

#include <vector>

class TSnDacSet : public TObject {
 public:
   typedef std::vector< UShort_t >  ChDacs_t;
   typedef std::vector< ChDacs_t >  DacSet_t;
   typedef std::vector< TString >   DacTypes_t;
   
 private:
   DacSet_t       fDacs;
   DacTypes_t     fDtypes;
   
 public:
   TSnDacSet() {}
   virtual ~TSnDacSet() {}
   
   DacSet_t::size_type
                     GetNchans() const { return fDacs.size(); }
   const DacSet_t&   GetDacs() const { return fDacs; }
   const ChDacs_t&   GetDacsChan(const DacSet_t::size_type ch) const
      { return fDacs.at(ch); }
   UShort_t          GetDac(const DacSet_t::size_type ch,
                            const ChDacs_t::size_type dn) const
      { return GetDacsChan(ch).at(dn); }
   const Char_t*     GetDacType(const DacTypes_t::size_type dn) const
      { return fDtypes.at(dn).Data(); }
   
   void              SetDac(const DacSet_t::size_type ch,
                            const ChDacs_t::size_type dn,
                            const UShort_t dac);
   void              SetDac(const DacSet_t::size_type ch,
                            const ChDacs_t::size_type dn,
                            const UShort_t dac,
                            const Char_t*  type);
   
   
   Bool_t   IsValidForDb() const;
   
   void     Print(Option_t* option="") const;

   ClassDef(TSnDacSet, 1); // a set of DAC values
};

#endif // SNS_TSnDacSet
