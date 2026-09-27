#ifndef TTUTORIALTHREEBODYDECAYPROCESSOR_H
#define TTUTORIALTHREEBODYDECAYPROCESSOR_H

#include "TProcessor.h"
#include "TEventCollection.h"

class TClonesArray;

namespace art {

class TTutorialThreeBodyDecayProcessor : public TProcessor {
public:
    TTutorialThreeBodyDecayProcessor();
    virtual ~TTutorialThreeBodyDecayProcessor();

    void Init(TEventCollection *col) override;
    void Process() override;

protected:
    // --------------------------------------------------
    // Output collection names
    // --------------------------------------------------

    TString fNameBeamLab;

    TString fNamePiMinus1CM;
    TString fNamePiMinus2CM;
    TString fNamePiPlus1CM;

    TString fNamePiMinus1Lab;
    TString fNamePiMinus2Lab;
    TString fNamePiPlus1Lab;

    TString fNameDalitz;

    // --------------------------------------------------
    // Output collections
    // --------------------------------------------------

    TClonesArray *fBeamLab;

    TClonesArray *fPiMinus1CM;
    TClonesArray *fPiMinus2CM;
    TClonesArray *fPiPlus1CM;

    TClonesArray *fPiMinus1Lab;
    TClonesArray *fPiMinus2Lab;
    TClonesArray *fPiPlus1Lab;

    TClonesArray *fDalitz;

    // --------------------------------------------------
    // Parameters
    // --------------------------------------------------

    Double_t fBeamMomentum;
    Int_t fVerbose;

    // --------------------------------------------------
    // Particle masses [MeV/c^2]
    // --------------------------------------------------

    Double_t fMassKm;
    Double_t fMassPiMinus;
    Double_t fMassPiPlus;

private:
    ClassDef(TTutorialThreeBodyDecayProcessor, 1)
};

} // namespace art

#endif
