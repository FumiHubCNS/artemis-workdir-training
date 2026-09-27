#include "TTutorialDalitzData.h"

#include <TMath.h>

using art::TTutorialDalitzData;

ClassImp(TTutorialDalitzData)

TTutorialDalitzData::TTutorialDalitzData()
{
    Clear();
}

void TTutorialDalitzData::Clear(Option_t *)
{
    fM2_12 = TMath::QuietNaN();
    fM2_23 = TMath::QuietNaN();

    fMPrime = TMath::QuietNaN();
    fThetaPrime = TMath::QuietNaN();
}
