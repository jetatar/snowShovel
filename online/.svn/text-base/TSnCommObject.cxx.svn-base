#include "TSnCommObject.h"

#include <TROOT.h>
#include <TClass.h>
#include <TString.h>

#include "TSnIOBuffer.h"

#include <iostream>

ClassImp(TSnCommObject);

TSnIOBuffer TSnCommObject::GetIOByteArray() const {
   TSnIOBuffer b;
   this->WriteIOByteArrayTo(b);
   return b;
}

void TSnCommObject::ls(Option_t* option) const {
   TROOT::IndentLevel();
   std::cout << "OBJ: " << IsA()->GetName() << "\t" << GetLabel()
      << std::endl;
}
