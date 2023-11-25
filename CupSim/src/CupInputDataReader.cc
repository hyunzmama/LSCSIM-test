
#include "CupSim/CupInputDataReader.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include <ctype.h>

int CupInputDataReader::ReadMaterials(std::istream &is) {
    static const char funcname[] = "CupSim/CupInputDataReader::ReadMaterials";

    G4Material *currentMaterial           = NULL;
    G4MaterialPropertiesTable *currentMPT = NULL;
    G4MaterialPropertyVector *currentPV   = NULL;

    MyTokenizer t(is);
    int wavelength_opt = 0;
    int errorCount     = 0;

    while (t.nextToken() != MyTokenizer::TT_EOF) {

        // expect either a pair of numbers or a keyword
        if (t.ttype == MyTokenizer::TT_STRING) {
            if (t.sval == "MATERIAL") {
                if (t.nextToken() == MyTokenizer::TT_STRING) {
                    currentMaterial = G4Material::GetMaterial(t.sval);
                    currentPV       = NULL;
                    wavelength_opt  = 0;
                    if (currentMaterial == NULL) {
                        currentMPT = NULL;
                        errorCount++; // error message issued in GetMaterial
                    } else {
                        currentMPT = currentMaterial->GetMaterialPropertiesTable();
                        if (currentMPT == NULL) {
                            currentMPT = new G4MaterialPropertiesTable();
                            currentMaterial->SetMaterialPropertiesTable(currentMPT);
                        }
                    }
                } else {
                    G4cerr << funcname << " expected string after MATERIAL\n";
                    errorCount++;
                }
            } else if (t.sval == "PROPERTY") {
                if (t.nextToken() == MyTokenizer::TT_STRING) {
                    currentPV      = NULL;
                    wavelength_opt = 0;
                    if (currentMPT != NULL) {
                        currentPV = currentMPT->GetProperty((char *)(const char *)(t.sval));
                        if (currentPV == NULL) {
                            currentPV = new G4MaterialPropertyVector();
                            currentMPT->AddProperty((char *)(const char *)(t.sval), currentPV);
                        }
                    }
                } else {
                    G4cerr << funcname << " expected string after PROPERTY\n";
                    errorCount++;
                }
            } else if (t.sval == "CONSTPROPERTY") {
                if (t.nextToken() == MyTokenizer::TT_STRING) {
                    G4String cosntpropertyname = t.sval;
                    if (t.nextToken() == MyTokenizer::TT_NUMBER) {
                        G4double constval = t.nval;
                        if (currentMPT != NULL)
                            currentMPT->AddConstProperty((char *)(const char *)(t.sval), constval);
                    } else {
                        G4cerr << funcname << " expected number for CONSTPROPERTY\n";
                        errorCount++;
                    }
                } else {
                    G4cerr << funcname << " expected string after CONSTPROPERTY\n";
                    errorCount++;
                }
            } else if (t.sval == "OPTION") {
                if (t.nextToken() == MyTokenizer::TT_STRING) {
                    if (t.sval == "wavelength")
                        wavelength_opt = 1;
                    else if (t.sval == "dy_dwavelength")
                        wavelength_opt = 2;
                    else if (t.sval == "energy")
                        wavelength_opt = 0;
                    else {
                        G4cerr << funcname << " unknown option " << t.sval << G4endl;
                        errorCount++;
                    }
                } else {
                    G4cerr << funcname << " expected string after OPTION\n";
                    errorCount++;
                }
            } else {
                G4cerr << funcname << " unknown keyword " << t.sval << G4endl;
                errorCount++;
            }
        } else if (t.ttype == MyTokenizer::TT_NUMBER) {
            double E_value = t.nval;
            if (t.nextToken() == MyTokenizer::TT_NUMBER) {
                double p_value = t.nval;
                if (currentMPT != NULL && currentPV != NULL) {
                    if (wavelength_opt) {
                        if (E_value != 0.0) {
                            double lam = E_value;
                            E_value    = 2 * pi * hbarc / (lam * nanometer);
                            if (wavelength_opt == 2) p_value *= lam / E_value;
                        } else {
                            G4cerr << funcname << " zero wavelength!\n";
                            errorCount++;
                        }
                    }
                    // currentPV->AddElement(E_value, p_value);
                    currentPV->InsertValues(E_value, p_value);
                } else {
                    G4cerr << funcname << " got number pair, but have no pointer to ";
                    if (currentMPT == NULL) G4cerr << "MaterialPropertyTable ";
                    if (currentPV == NULL) G4cerr << "MaterialPropertyVector ";
                    G4cerr << G4endl;
                    errorCount++;
                }
            } else {
                G4cerr << funcname << " expected second number, but tokenizer state is ";
                t.dumpOn(G4cerr);
                G4cerr << G4endl;
                errorCount++;
            }
        } else {
            G4cerr << funcname << " expected a number or a string, but tokenizer state is ";
            t.dumpOn(G4cerr);
            G4cerr << G4endl;
            errorCount++;
        }
    }

    return errorCount;
}

void CupInputDataReader::MyTokenizer::dumpOn(std::ostream &os) {
    os << "CupSim/CupInputDataReader::MyTokenizer[ttype=" << ttype << ",nval=" << nval
       << ",sval=" << sval << "] ";
}

int CupInputDataReader::MyTokenizer::nextToken(void) {
    int i             = 0;
    G4bool negateFlag = false;
    do {
        i = isptr->get();
        if (i == '+') i = isptr->get();
        if (i == '-') {
            i          = isptr->get();
            negateFlag = !negateFlag;
        }
        if (i == '#') { // comment to end of line
            do {
                i = isptr->get();
            } while (i != EOF && i != '\n');
        }
        if (i == EOF) return (ttype = TT_EOF);
    } while (isspace(i));

    if (isdigit(i) || i == '.') {
        nval = 0.0;
        isptr->putback(i);
        (*isptr) >> nval;
        if (negateFlag) nval = -nval;
        return (ttype = TT_NUMBER);
    } else if (negateFlag) {
        isptr->putback(i);
        return (ttype = '-');
    } else if (isalpha(i) || i == '_') {
        isptr->putback(i);
        (*isptr) >> sval;
        return (ttype = TT_STRING);
    } else if (i == '"') {
        sval = "";
        while (true) {
            i = isptr->get();
            while (i == '\\') {
                i = isptr->get();
                sval.append((char)i);
                i = isptr->get();
            }
            if (i == EOF || i == '"') break;
            sval.append((char)i);
        }
        return (ttype = TT_STRING);
    } else {
        return (ttype = i);
    }
}
