#include "TSnCommFileAction.h"

#include <TClass.h>

#include "TSnCommActor.h"
#include "TSnCommIncFile.h"

#include <stdexcept>

ClassImp(TSnCommFileAction);

TSnCommObject* TSnCommFileAction::operator()(const TSnCommIncoming& in) {
   // this function is called externally, and should be considered "final"
   // (i.e. TSnCommFileAction's shouldn't override this function)
   // 
   // TSnCommFileAction's should override the operator that
   // accepts a TSnCommIncFile. Since TSnCommIncFile has a lot of parameters
   // and each action may not want all of them, we will just pass the whole
   // thing and let each action get the parameters it needs
   TSnCommObject* o(0);
   if (in.IsA()->InheritsFrom(TSnCommIncFile::Class())) {
      const TSnCommIncFile& inf = static_cast<const TSnCommIncFile&>(in);
      o = this->operator()(inf);
   } else {
      throw std::runtime_error(Form("<TSnCommFileAction::operator()>: "
                                    "Incoming comm of type [%s] not of a "
                                    "TSnCommIncFile as expected.",
                                    in.IsA()->GetName()));
   }
   return o;
}
