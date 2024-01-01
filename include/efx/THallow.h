#ifndef _EFX_HALLOW_H
#define _EFX_HALLOW_H

#include "efx/TSimple.h"

namespace efx {

struct THallow : public TSimple1 {
	THallow()
	    : TSimple1(PID_BabaHe_2)
	{
	}

	virtual bool create(Arg*); // _08

	// _00      = VTBL
	// _00-_0C  = TSimple1
};

} // namespace efx

#endif