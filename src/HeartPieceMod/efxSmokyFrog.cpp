#include "efx/TSmokyFrog.h"

namespace efx {
bool TSmokyFrog::create(Arg* arg)
{
	ArgScale* argScale = static_cast<ArgScale*>(arg);
	f32 scale          = argScale->mScale;

	if (TSimple2::create(argScale)) {
		mEmitters[0]->setScale(scale);
		mEmitters[1]->setScale(scale);
		return true;
	}
	return false;
}
} // namespace efx
