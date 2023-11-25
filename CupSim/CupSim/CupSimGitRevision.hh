#ifndef __CupSimLibGitRevision_H_
#define __CupSimLibGitRevision_H_

#include <iostream>

class CupSimGitRevision {
  public:
    static void PrintGitInfo();
    static const char *GetRevisionHash();
    static const char *GetBranchName();

  private:
    CupSimGitRevision(){}; // Don't create an object of this cless!
    ~CupSimGitRevision(){};
    static const char *fgceRevisionHash;
    static const char *fgceBranchName;
    static const char *fgceLibraryName;
};

#endif
