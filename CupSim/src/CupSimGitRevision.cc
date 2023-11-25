#include "CupSim/CupSimGitRevision.hh"

#define STRINGFY(X) #X
#define TOSTRING(X) STRINGFY(X)

const char *CupSimGitRevision::fgceRevisionHash = TOSTRING(CupSim_GIT_COMMIT_HASH);
const char *CupSimGitRevision::fgceBranchName   = TOSTRING(CupSim_GIT_BRANCH);
const char *CupSimGitRevision::fgceLibraryName  = "CupSim library";

const char *CupSimGitRevision::GetRevisionHash() { return fgceRevisionHash; }
const char *CupSimGitRevision::GetBranchName() { return fgceBranchName; }
void CupSimGitRevision::PrintGitInfo() {
    std::cout << fgceLibraryName << ": <Branch: " << fgceBranchName
              << ", Revision hash: " << fgceRevisionHash << ">" << std::endl;
}
