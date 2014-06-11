#ifndef SNS_TSnPlaSet
#define SNS_TSnPlaSet

#include <TObject.h>

#include <vector>


class TSnPlaSet : public TObject {
 public:
   typedef std::vector< UShort_t >  PlaSet_t;
   
   static const Int_t      kPLAPtnLen; // PLA length (string char length)
   
 private:
   PlaSet_t       fPlas;
   
 public:
   TSnPlaSet() {}
   virtual ~TSnPlaSet() {}
   
   const PlaSet_t&         GetPlas() const { return fPlas; }
   UShort_t                GetPla(const PlaSet_t::size_type pn) const
      { return fPlas.at(pn); }
   PlaSet_t::size_type     GetNplas() const { return fPlas.size(); }
   
   void     SetPla(const PlaSet_t::size_type pn,
                   const UShort_t pla);
   void     AddPla(const UShort_t pla);
   void     SetPla(const PlaSet_t::size_type pn,
                   const Char_t* pla,
                   const Bool_t r2l=kFALSE)
      { SetPla(pn, GetPlaFromStr(pla, r2l)); }
   void     AddPla(const Char_t* pla, const Bool_t r2l=kFALSE)
      { AddPla(GetPlaFromStr(pla, r2l)); }
   
   void     Print(Option_t* option="") const;
   Bool_t   IsValidForDb() const;
   
   static const Char_t*    GetPlaAsStr(const UShort_t pla,
                                       const Bool_t r2l=kFALSE);
   static       UShort_t   GetPlaFromStr(const Char_t* const ps,
                                         const Bool_t r2l=kFALSE);
   static
   void                    GetHiLoPlaFromStr(const Char_t* const plastr,
                                             UShort_t& hiPla,
                                             UShort_t& loPla,
                                             const Bool_t r2l=kFALSE);
   static
   void                    GetHiLoPlas(const UShort_t pla,
                                       UShort_t& hiPla,
                                       UShort_t& loPla,
                                       const bool r2l=kFALSE);
   
   
   ClassDef(TSnPlaSet, 1); // a set of patterns
};

#endif // SNS_TSnPlaSet
