#include "efx/THallow.h"
#include "Game/Entities/HallowMushi.h"
#include "efx/Arg.h"

namespace efx {

bool THallow::create(Arg* arg)
{

	P2ASSERT(!strcmp(arg->getName(), "ArgScaleTime"));

	ArgScaleTime* scaleArg = static_cast<ArgScaleTime*>(arg);

	const f32 paritcleScale = scaleArg->mScale * (1.0f / 12.0f);
	const f32 emitterScale  = scaleArg->mScale * (7.0f / 30.0f);

	if (TSimple1::create(arg)) {
		for (int i = 0; i < ARRAY_SIZE(mEmitters); i++) {
			mEmitters[i]->mLocalScl.set(emitterScale, 1.0f, emitterScale);
			mEmitters[i]->mGlobalPScl = paritcleScale;
			mEmitters[i]->mLifeTime = (int)(scaleArg->mTime * 30);
		}
		return true;
	}
	return false;
}

} // namespace efx
