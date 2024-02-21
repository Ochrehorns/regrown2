#ifndef _EFX_TPLASM_H_
#define _EFX_TPLASM_H_

#include "efx/TChaseMtx.h"

namespace efx {

struct TPlasmSparkle : public TChaseMtx {
	TPlasmSparkle()
	    : TChaseMtx(PID_PlasmWraithShimmer, nullptr)
	{
	}
};

} // namespace efx

#endif
