#ifndef __MCObjsLibGitRevision_H_
#define __MCObjsLibGitRevision_H_

#include <iostream>

class MCObjsGitRevision {
  public:
    static void PrintGitInfo();
    static const char *GetRevisionHash();
    static const char *GetBranchName();

  private:
    MCObjsGitRevision(){}; // Don't create an object of this cless!
    ~MCObjsGitRevision(){};
    static const char *fgceRevisionHash;
    static const char *fgceBranchName;
    static const char *fgceLibraryName;
};

#endif
