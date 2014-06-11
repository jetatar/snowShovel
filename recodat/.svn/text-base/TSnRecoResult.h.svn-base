#ifndef SNS_TSnRecoResult
#define SNS_TSnRecoResult

#include <TNamed.h>
#include <TString.h>

class TSnRecoResult : public TNamed {
 private:
   
 public:
   TSnRecoResult() {}
   TSnRecoResult(const Char_t* name, const Char_t* minimizerTag) :
      TNamed(name, minimizerTag) {
   }
   virtual ~TSnRecoResult() {}
   
   virtual void CopyFrom(const TSnRecoResult& x) = 0;
   
   virtual void Copy(TObject& o) const; // copy to
   
   TString      GetMinimizerTag() const { return GetTitle(); }
   
   void         SetMinimizerTag(const Char_t* t) { SetTitle(t); }
   
   virtual void Print(Option_t* option = "") const { TNamed::Print(option); }
   
   ClassDef(TSnRecoResult, 1); // ABC providing a common interface for reconstruction fit results
};

#endif // SNS_TSnRecoResult
