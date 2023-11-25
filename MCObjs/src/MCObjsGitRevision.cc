#include "MCObjs/MCObjsGitRevision.hh"
#include "TF1.h"

#include <iostream>

#define STRINGFY(X) #X
#define TOSTRING(X) STRINGFY(X)

const char *MCObjsGitRevision::fgceRevisionHash = TOSTRING(MCObjs_GIT_COMMIT_HASH);
const char *MCObjsGitRevision::fgceBranchName   = TOSTRING(MCObjs_GIT_BRANCH);
const char *MCObjsGitRevision::fgceLibraryName  = "MCObjs library";

const char *MCObjsGitRevision::GetRevisionHash() { return fgceRevisionHash; }
const char *MCObjsGitRevision::GetBranchName() { return fgceBranchName; }

using namespace std;

void MCObjsGitRevision::PrintGitInfo() {
    cout << fgceLibraryName << ": <Branch: " << fgceBranchName
         << ", Revision hash: " << fgceRevisionHash << ">" << endl;
}

int main() {
    cout << "          The MCObjs library for MC data classes" << endl;
    cout << "    Author: Center for Underground Physics (CUP), Korea" << endl;
    cout << "===============================================================" << endl;
    cout << "The simulation framework of CUP using Geant4 requires recording" << endl
         << "many information during the simulation such as interaction" << endl
         << "or energy deposition on a matter." << endl
         << "This library provides some classes for storing the information" << endl
         << "with ROOT I/O framework so it enables to store useful data with" << endl
         << "TFile or TTree class." << endl;
    cout << "===============================================================" << endl;
    MCObjsGitRevision::PrintGitInfo();
}
