#ifndef SNS_TSnWavelet
#define SNS_TSnWavelet

#include <TObject.h>
#include <TArrayF.h>

class TSnWavelet : public TObject {
 private:
   TArrayF fCC;
   TArrayF fCR;
   Int_t   fIoff;
   Int_t   fJoff;
   UInt_t  fNcof;


   void TransformStep(Float_t* const data,
                      const UInt_t n);
   void InverseStep(Float_t* const data,
                    const UInt_t n);
   
 protected:
   void BuildCRandOffs();

 public:
   TSnWavelet() : fIoff(0), fJoff(0), fNcof(0) {}
   TSnWavelet(const Int_t n, const Int_t ioff, const Int_t joff,
              const UInt_t ncof) : fCC(n), fCR(n), fIoff(ioff),
                                   fJoff(joff), fNcof(ncof) {}
   TSnWavelet(const TSnWavelet& o) : fCC(o.fCC), fCR(o.fCR),
                                     fIoff(o.fIoff), fJoff(o.fJoff),
                                     fNcof(o.fNcof) {}
   virtual ~TSnWavelet() {}
   
   virtual void          BuildWavelet() = 0;
   virtual const Char_t* GetLabel() const = 0;
   
   void Transform(const Float_t* const data,
                  const UInt_t n,
                  Float_t* const result);
   void Inverse(const Float_t* const data,
                const UInt_t n,
                Float_t* const result);
   
   
   const TArrayF& GetCC() const { return fCC; }
         TArrayF& GetCC()       { return fCC; }
   const TArrayF& GetCR() const { return fCR; }
         TArrayF& GetCR()       { return fCR; }
   Float_t GetCC(const UInt_t i) const { return fCC.At(i); }
   Float_t GetCR(const UInt_t i) const { return fCR.At(i); }
   Int_t   GetIoff() const { return fIoff; }
   Int_t   GetJoff() const { return fJoff; }
   UInt_t  GetNcof() const { return fNcof; }
   
   void    SetCC(const UInt_t i, const Float_t x) { fCC[i] = x; }
   void    SetCR(const UInt_t i, const Float_t x) { fCR[i] = x; }
   void    SetIoff(const Int_t o) { fIoff = o; }
   void    SetJoff(const Int_t o) { fJoff = o; }
   void    SetNcof(const UInt_t n) { fNcof = n; fCC.Set(n); fCR.Set(n); }
   
   ClassDef(TSnWavelet, 1); // ABC of wavelets
};

#endif // SNS_TSnWavelet
