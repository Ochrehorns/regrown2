#ifndef _EFX_TSMOKYFROG_H
#define _EFX_TSMOKYFROG_H

#include "efx/TSimple.h"

namespace efx {
struct TSmokyFrog : public TSimple2 {
	inline TSmokyFrog()
	    : TSimple2(PID_OtaGas_1, PID_OtaGas_2)
	{
	}

	virtual bool create(Arg*); // _08

	// _00     = VTBL
	// _00-_10 = TSimple2
};
} // namespace efx

#endif
