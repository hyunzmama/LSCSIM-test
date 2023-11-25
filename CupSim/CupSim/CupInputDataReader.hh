
#ifndef CupInputDataReader_H
#define CupInputDataReader_H 1

#include "G4ThreeVector.hh"
#include "globals.hh"
#include "iostream"

using namespace CLHEP;

class CupInputDataReader {
  public:
    class MyTokenizer {
      private:
        std::istream *isptr;

      public:
        MyTokenizer(std::istream &is) {
            isptr = &is;
            nval  = 0.0;
        }

        enum { TT_EOF = -1, TT_STRING = 'a', TT_NUMBER = '0' };

        int ttype;

        G4double nval;
        G4String sval;

        int nextToken(void);

        void dumpOn(std::ostream &os);
    };

    static int ReadMaterials(std::istream &is);
};

#endif // CupInputDataReader_H
