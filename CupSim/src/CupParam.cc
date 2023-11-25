
#include "G4Version.hh"

#include "CupSim/CupParam.hh"

#include <fstream>
#include <sstream>

#if G4VERSION_NUMBER >= 1000
G4ThreadLocal CupParam *CupParam::theCupParam = nullptr;
CupParam *CupParam::theMasterCupParam = nullptr;
#else
CupParam *CupParam::theCupParam = nullptr;
CupParam *CupParam::theMasterCupParam = nullptr;
#endif

// Constructor
CupParam::CupParam() {}

// ReadFile
void CupParam::ReadFile(std::istream &is, EOverride oflag) {
    while (is.good()) {
        // get a line from the file
        char linebuffer[128];
        is.getline(linebuffer, sizeof(linebuffer) - 1);
        if (is.fail()) break;

        // put the line in an istrstream for convenient parsing
        std::istringstream lineStream(linebuffer);

        // parse out name
        G4String name;
        G4double value;
        lineStream >> name;

        // skip lines beginning with '#' or '\n';
        if (lineStream.fail() || name.length() == 0 || (name.data())[0] == '\n' ||
            (name.data())[0] == '#')
            continue;

        // parse out value
        lineStream >> value;
        if (lineStream.fail()) {
            G4cerr << "Warning: invalid/missing value encountered"
                      "in CupParam::ReadFile(): line was ``"
                   << linebuffer << "''" << G4endl;
            continue;
        }

        // does name already exist in hash?
        if (count(name)) {
            if (oflag == kKeepExistingValue) {
                G4cout << "Info: CupParam::ReadFile: retaining previous setting " << name << "="
                       << (*this)[name] << ", ignoring value " << value << " specified by file.\n";
            } else {
                G4cout << "Info: CupParam::ReadFile: OVERRIDING previous setting " << name << "="
                       << (*this)[name] << ", new value is " << value << " as specified by file.\n";
                // insert name/value into hash
                (*this)[name] = value;
            }
        } else {
            // insert name/value into hash
            (*this)[name] = value;
        }
    }
}

void CupParam::ReadFile(const char *filename, EOverride oflag) {
    std::ifstream ifs;
    ifs.open(filename);
    if (!ifs.good()) {
        G4cout << "CupSim/CupParam::ReadFile : could not open " << filename << G4endl;
        return;
    }
    ReadFile(ifs, oflag);
    ifs.close();
}

void CupParam::WriteFile(std::ostream &os) {
    iterator i;
    for (i = begin(); i != end(); i++)
        os << (*i).first << '\t' << (*i).second << G4endl;
    os.flush();
}
