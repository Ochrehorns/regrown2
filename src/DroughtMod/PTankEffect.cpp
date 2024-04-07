#include "efx/TTank.h"

namespace efx
{

bool TTankGas::create(Arg* arg) {
    mParticleCallBack.mEfxHit = (TTankFireHit*)&mEfxHit;
    mParticleCallBack.mEfxHit->create(nullptr);
    mParticleCallBack._04 = 1000.0f;
    if (TChaseMtx3::create(arg)) {
        for (int i = 0; i < 3; i++) {
            mItems[i].mEmitter->mParticleCallback = &mParticleCallBack;
        }
    }
    return true;
}

} // namespace efx
