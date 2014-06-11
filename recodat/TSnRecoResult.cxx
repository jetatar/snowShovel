#include "TSnRecoResult.h"

ClassImp(TSnRecoResult);

void TSnRecoResult::Copy(TObject& o) const {
   TSnRecoResult& r = dynamic_cast<TSnRecoResult&>(o);
   r.CopyFrom(*this);
}

