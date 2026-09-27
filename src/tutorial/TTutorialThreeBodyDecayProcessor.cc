#include "TTutorialThreeBodyDecayProcessor.h"

#include "TTutorialDalitzData.h"

#include "TArtParticle.h"

#include <TClonesArray.h>
#include <TDatabasePDG.h>
#include <TMath.h>
#include <TRandom.h>
#include <TLorentzVector.h>
#include <TVector3.h>

#include <iostream>

using art::TTutorialThreeBodyDecayProcessor;

ClassImp(TTutorialThreeBodyDecayProcessor)


TTutorialThreeBodyDecayProcessor::
TTutorialThreeBodyDecayProcessor()
    : fBeamLab(nullptr),
      fPiMinus1CM(nullptr),
      fPiMinus2CM(nullptr),
      fPiPlus1CM(nullptr),
      fPiMinus1Lab(nullptr),
      fPiMinus2Lab(nullptr),
      fPiPlus1Lab(nullptr),
      fDalitz(nullptr),
      fBeamMomentum(1000.0),
      fVerbose(0),
      fMassKm(0.0),
      fMassPiMinus(0.0),
      fMassPiPlus(0.0)
{
    // --------------------------------------------------
    // TArtParticle outputs
    // --------------------------------------------------

    RegisterOutputCollection(
        "BeamLab",
        "K- beam in laboratory frame",
        fNameBeamLab,
        "BeamLab",
        &fBeamLab,
        TClonesArray::Class_Name(),
        TArtParticle::Class_Name()
    );

    RegisterOutputCollection(
        "PiMinus1CM",
        "first pi- in K- center-of-mass frame",
        fNamePiMinus1CM,
        "PiMinus1CM",
        &fPiMinus1CM,
        TClonesArray::Class_Name(),
        TArtParticle::Class_Name()
    );

    RegisterOutputCollection(
        "PiMinus2CM",
        "second pi- in K- center-of-mass frame",
        fNamePiMinus2CM,
        "PiMinus2CM",
        &fPiMinus2CM,
        TClonesArray::Class_Name(),
        TArtParticle::Class_Name()
    );

    RegisterOutputCollection(
        "PiPlus1CM",
        "pi+ in K- center-of-mass frame",
        fNamePiPlus1CM,
        "PiPlus1CM",
        &fPiPlus1CM,
        TClonesArray::Class_Name(),
        TArtParticle::Class_Name()
    );

    RegisterOutputCollection(
        "PiMinus1Lab",
        "first pi- in laboratory frame",
        fNamePiMinus1Lab,
        "PiMinus1Lab",
        &fPiMinus1Lab,
        TClonesArray::Class_Name(),
        TArtParticle::Class_Name()
    );

    RegisterOutputCollection(
        "PiMinus2Lab",
        "second pi- in laboratory frame",
        fNamePiMinus2Lab,
        "PiMinus2Lab",
        &fPiMinus2Lab,
        TClonesArray::Class_Name(),
        TArtParticle::Class_Name()
    );

    RegisterOutputCollection(
        "PiPlus1Lab",
        "pi+ in laboratory frame",
        fNamePiPlus1Lab,
        "PiPlus1Lab",
        &fPiPlus1Lab,
        TClonesArray::Class_Name(),
        TArtParticle::Class_Name()
    );

    // --------------------------------------------------
    // Dalitz output
    // --------------------------------------------------

    RegisterOutputCollection(
        "Dalitz",
        "Dalitz plot variables",
        fNameDalitz,
        "Dalitz",
        &fDalitz,
        TClonesArray::Class_Name(),
        art::TTutorialDalitzData::Class_Name()
    );

    // --------------------------------------------------
    // Parameters
    // --------------------------------------------------

    RegisterOptionalParameter(
        "BeamMomentum",
        "K- beam momentum along z [MeV/c]",
        fBeamMomentum,
        1000.0
    );

    RegisterOptionalParameter(
        "Verbose",
        "verbose level",
        fVerbose,
        0
    );
}


TTutorialThreeBodyDecayProcessor::
~TTutorialThreeBodyDecayProcessor()
{
    delete fBeamLab;

    delete fPiMinus1CM;
    delete fPiMinus2CM;
    delete fPiPlus1CM;

    delete fPiMinus1Lab;
    delete fPiMinus2Lab;
    delete fPiPlus1Lab;

    delete fDalitz;
}


void TTutorialThreeBodyDecayProcessor::
Init(TEventCollection *)
{
    TDatabasePDG *pdg = TDatabasePDG::Instance();

    const auto *km = pdg->GetParticle("K-");
    const auto *pim = pdg->GetParticle("pi-");
    const auto *pip = pdg->GetParticle("pi+");

    if (!km || !pim || !pip) {
        SetStateError(
            "Failed to obtain particle masses from TDatabasePDG"
        );
        return;
    }

    // ROOT PDG masses are GeV/c^2.
    // ARTEMIS calculation here uses MeV/c^2.
    fMassKm      = km->Mass()  * 1000.0;
    fMassPiMinus = pim->Mass() * 1000.0;
    fMassPiPlus  = pip->Mass() * 1000.0;

    if (fVerbose > 0) {
        std::cout
            << "[TTutorialThreeBodyDecayProcessor]\n"
            << "  m(K-)  = " << fMassKm << " MeV/c2\n"
            << "  m(pi-) = " << fMassPiMinus << " MeV/c2\n"
            << "  m(pi+) = " << fMassPiPlus << " MeV/c2\n"
            << "  pBeam  = " << fBeamMomentum << " MeV/c"
            << std::endl;
    }
}


void TTutorialThreeBodyDecayProcessor::
Process()
{
    // ==================================================
    // Clear output collections
    // ==================================================

    fBeamLab->Clear("C");

    fPiMinus1CM->Clear("C");
    fPiMinus2CM->Clear("C");
    fPiPlus1CM->Clear("C");

    fPiMinus1Lab->Clear("C");
    fPiMinus2Lab->Clear("C");
    fPiPlus1Lab->Clear("C");

    fDalitz->Clear("C");

    // ==================================================
    // K- beam
    // ==================================================

    TArtParticle beam;

    beam.SetXYZM(
        0.0,
        0.0,
        fBeamMomentum,
        fMassKm
    );

    const TVector3 boostToLab = beam.BoostVector();

    // ==================================================
    // K- -> pi- + (pi+ pi-)
    //
    // particle 1 = pi-
    // particle 2 = pi+
    // particle 3 = pi-
    // ==================================================

    const Double_t m23Min =
        fMassPiPlus + fMassPiMinus;

    const Double_t m23Max =
        fMassKm - fMassPiMinus;

    const Double_t m23 =
        gRandom->Uniform(m23Min, m23Max);

    // Momentum for:
    //
    // K- -> pi- + M23
    //
    // in the K- rest frame.

    const Double_t pNumerator1 =
        fMassKm * fMassKm
        - TMath::Power(m23 + fMassPiMinus, 2);

    const Double_t pNumerator2 =
        fMassKm * fMassKm
        - TMath::Power(m23 - fMassPiMinus, 2);

    const Double_t p2 =
        pNumerator1 * pNumerator2;

    if (p2 < 0.0) {
        if (fVerbose > 0) {
            std::cerr
                << "[TTutorialThreeBodyDecayProcessor] "
                << "invalid decay momentum"
                << std::endl;
        }
        return;
    }

    const Double_t p =
        TMath::Sqrt(p2) / (2.0 * fMassKm);

    // --------------------------------------------------
    // First decay:
    //
    // K- -> pi-1 + M23
    // --------------------------------------------------

    TArtParticle piMinus1CM;
    TArtParticle intermediate23;

    piMinus1CM.SetPxPyPzE(
        0.0,
        0.0,
        p,
        TMath::Sqrt(
            fMassPiMinus * fMassPiMinus + p * p
        )
    );

    intermediate23.SetXYZM(
        0.0,
        0.0,
        -p,
        m23
    );

    // Isotropic direction

    const Double_t theta =
        TMath::ACos(
            2.0 * gRandom->Uniform() - 1.0
        );

    const Double_t phi =
        gRandom->Uniform(
            0.0,
            2.0 * TMath::Pi()
        );

    piMinus1CM.RotateY(theta);
    piMinus1CM.RotateZ(phi);

    intermediate23.RotateY(theta);
    intermediate23.RotateZ(phi);

    // --------------------------------------------------
    // Second decay:
    //
    // M23 -> pi+ + pi-
    // --------------------------------------------------

    intermediate23.SetTwoBodyDecay(
        fMassPiPlus,
        fMassPiMinus
    );

    intermediate23.Decay();

    TArtParticle *piPlus1CM =
        intermediate23.GetDaughter(0);

    TArtParticle *piMinus2CM =
        intermediate23.GetDaughter(1);

    if (!piPlus1CM || !piMinus2CM) {
        if (fVerbose > 0) {
            std::cerr
                << "[TTutorialThreeBodyDecayProcessor] "
                << "failed to generate daughter particles"
                << std::endl;
        }
        return;
    }

    // ==================================================
    // Laboratory frame
    // ==================================================

    TArtParticle piMinus1Lab(
        piMinus1CM.Vect(),
        piMinus1CM.E()
    );

    TArtParticle piMinus2Lab(
        piMinus2CM->Vect(),
        piMinus2CM->E()
    );

    TArtParticle piPlus1Lab(
        piPlus1CM->Vect(),
        piPlus1CM->E()
    );

    piMinus1Lab.Boost(boostToLab);
    piMinus2Lab.Boost(boostToLab);
    piPlus1Lab.Boost(boostToLab);

    // ==================================================
    // Fill particle output collections
    // ==================================================

    auto copyParticle =
        [](TClonesArray *array,
           const TArtParticle &particle)
        {
            auto *out =
                static_cast<TArtParticle*>(
                    array->ConstructedAt(0)
                );

            out->SetPxPyPzE(
                particle.Px(),
                particle.Py(),
                particle.Pz(),
                particle.E()
            );
        };

    copyParticle(
        fBeamLab,
        beam
    );

    copyParticle(
        fPiMinus1CM,
        piMinus1CM
    );

    copyParticle(
        fPiMinus2CM,
        *piMinus2CM
    );

    copyParticle(
        fPiPlus1CM,
        *piPlus1CM
    );

    copyParticle(
        fPiMinus1Lab,
        piMinus1Lab
    );

    copyParticle(
        fPiMinus2Lab,
        piMinus2Lab
    );

    copyParticle(
        fPiPlus1Lab,
        piPlus1Lab
    );

    // ==================================================
    // Dalitz variables
    // ==================================================

    const TLorentzVector lv12 =
        static_cast<TLorentzVector>(piMinus1CM)
        + static_cast<TLorentzVector>(*piPlus1CM);

    const TLorentzVector lv23 =
        static_cast<TLorentzVector>(*piPlus1CM)
        + static_cast<TLorentzVector>(*piMinus2CM);

    // Real invariant-mass squared variables.
    const Double_t m2_12 = lv12.M2();
    const Double_t m2_23 = lv23.M2();

    const Double_t m12 = lv12.M();

    const Double_t m12Min =
        fMassPiMinus + fMassPiPlus;

    const Double_t m12Max =
        fMassKm - fMassPiMinus;

    const Double_t mPrime =
        TMath::ACos(
            2.0
            * (m12 - m12Min)
            / (m12Max - m12Min)
            - 1.0
        )
        / TMath::Pi();

    const TVector3 p12 =
        piMinus1CM.Vect()
        + piPlus1CM->Vect();

    const Double_t thetaPrime =
        p12.Theta() / TMath::Pi();

    auto *dalitz =
        static_cast<art::TTutorialDalitzData*>(
            fDalitz->ConstructedAt(0)
        );

    dalitz->SetM2_12(m2_12);
    dalitz->SetM2_23(m2_23);
    dalitz->SetMPrime(mPrime);
    dalitz->SetThetaPrime(thetaPrime);
}
