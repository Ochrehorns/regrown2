#ifndef _EFX_HALLOW_H
#define _EFX_HALLOW_H

#include "efx/TSimple.h"
#include "efx/TChaseMtx.h"

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

struct TElecClawBolts : public TChaseMtx
{
	TElecClawBolts()
	    : TChaseMtx(PID_HallowElecBolts, nullptr)
	{
	}
};

struct TElecClawGlow : public TChaseMtx
{
	TElecClawGlow()
	    : TChaseMtx(PID_HallowElecGlow, nullptr)
	{
	}
};


} // namespace efx

#endif