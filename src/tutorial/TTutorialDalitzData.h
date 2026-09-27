#ifndef TTUTORIALDALITZDATA_H
#define TTUTORIALDALITZDATA_H

#include <TObject.h>

namespace art {

class TTutorialDalitzData : public TObject {
public:
    TTutorialDalitzData();
    virtual ~TTutorialDalitzData() = default;

    void Clear(Option_t *opt = "") override;

    void SetM2_12(Double_t value) { fM2_12 = value; }
    void SetM2_23(Double_t value) { fM2_23 = value; }

    void SetMPrime(Double_t value) {
        fMPrime = value;
    }

    void SetThetaPrime(Double_t value) {
        fThetaPrime = value;
    }

    Double_t GetM2_12() const { return fM2_12; }
    Double_t GetM2_23() const { return fM2_23; }

    Double_t GetMPrime() const {
        return fMPrime;
    }

    Double_t GetThetaPrime() const {
        return fThetaPrime;
    }

private:
    Double_t fM2_12;
    Double_t fM2_23;

    Double_t fMPrime;
    Double_t fThetaPrime;

    ClassDef(TTutorialDalitzData, 1)
};

} // namespace art

#endif
