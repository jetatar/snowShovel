#include "TSnCommStatusAction.h"

#include <TClass.h>

#include "TSnCommIncStatus.h"

#include <stdexcept>

ClassImp(TSnCommStatusAction);

TSnCommObject* TSnCommStatusAction::operator()(const TSnCommIncoming& in) {
   // this function is called externally, and should be considered "final"
   // (i.e. TSnCommStatusAction's shouldn't override this function)
   // 
   // TSnCommStatusAction's should override this operator
   TSnCommObject* o(0);
   if (in.IsA()->InheritsFrom(TSnCommIncStatus::Class())) {
      const TSnCommIncStatus& is = static_cast<const TSnCommIncStatus&>(in);
      o = this->operator()(is);
   } else {
      throw std::runtime_error(Form("<TSnCommStatusAction::operator()>: "
                                    "Incoming comm of type [%s] not of a "
                                    "TSnCommIncStatus as expected.",
                                    in.IsA()->GetName()));
   }
   return o;
}
