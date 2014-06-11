//
// $Id$
//

//
// Create ROOT style HTML documentation
//
// Use like:
// .x build/make_doc.C+("$SNS")
//
#include <TClassEdit.h>
#include <TClass.h>
#include <TClassTable.h>
#include <TError.h>
#include <THashList.h>
#include <THtml.h>
#include <TList.h>
#include <TNamed.h>
#include <TRegexp.h>
#include <TROOT.h>
#include <TSystem.h>

class BetterHtml : public THtml {
public:
   void CreateIndex(const char **classNames, Int_t numberOfClasses)
      { THtml::CreateIndex(classNames,numberOfClasses); }
   ClassDef(BetterHtml,0)
};

void get_class_list(TList *cl)
{
   // start from begining
   gClassTable->Init();

   while(const char *cname = gClassTable->Next()) {
      cl->Add(new TNamed(cname,cname));
   }
}

void load_libs()
{
   TDirectory* prevdir = gDirectory;
   TRegexp re("*.so", true);
   void *dir = gSystem->OpenDirectory("lib");
   if(!dir) return;
   while(const char *file = gSystem->GetDirEntry(dir)) {
      TString s(file);
      if (s.Index(re) == kNPOS)
         continue;
      ::Info("make_doc","Loading %s", file);
      gSystem->Load(file);
   }
   gSystem->FreeDirectory(dir);
   gDirectory = prevdir;
}

void make_doc(const char *topvar)
{
   THashList  base;
   TList      all;
   BetterHtml html;

   // Get list of ROOT classes
   get_class_list(&base);

   // Load our libraries
   load_libs();

   // Get list of all classes
   get_class_list(&all);

   // Generate HTML
   const char **classes = new const char* [all.GetSize()];
   int n = 0;
   TIter next(&all);
   TNamed *cl;
   while((cl = dynamic_cast<TNamed*>(next()))) {
   
      if (base.FindObject(cl->GetName()) != 0) continue;
      
      TClass *classp = gROOT->GetClass(cl->GetName());
      TString path(topvar);
      path += "/";
      path += classp->GetImplFileName();
      gSystem->ExpandPathName(path);
      ::Info("make_doc","AccessPathName(%s) == %d", path.Data(),
         gSystem->AccessPathName(path));
      if (gSystem->AccessPathName(path)) continue;
      
      if (TClassEdit::IsStdClass(cl->GetName())) continue;
      ::Info("make_doc","Processing %s (%s)",
         cl->GetName(), classp->GetImplFileName());
      html.MakeClass(cl->GetName(),true);
      classes[n++] = StrDup(cl->GetName());
   }

   html.CreateIndex(classes,n);
}

