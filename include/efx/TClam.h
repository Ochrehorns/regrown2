#ifndef _EFX_TCLAM_H
#define _EFX_TCLAM_H

#include "efx/TChaseMtx.h"
#include "efx/TSimpleMtx.h"
#include "efx/TOneEmitter.h"
#include "efx/TForever.h"

namespace efx {

struct TClamSparkles : public TChaseMtx {
	TClamSparkles()
	    : TChaseMtx(713, nullptr)
	{
	}
};

struct TClamShut : public TSimpleMtx1 {
	inline TClamShut()
	    : TSimpleMtx1(nullptr, 712)
	{
	}

	// _00      = VTBL
	// _00-_0C  = TSimple1
};

struct TClamDead : public TSimpleMtx1 {
	inline TClamDead()
	    : TSimpleMtx1(nullptr, 711)
	{
	}

	// _00      = VTBL
	// _00-_0C  = TSimple1
};

} // namespace efx

#endif
