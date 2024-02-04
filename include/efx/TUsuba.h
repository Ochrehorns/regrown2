#ifndef _EFX_TUSUBA_H
#define _EFX_TUSUBA_H

#include "efx/TChaseMtx.h"
#include "efx/TOneEmitter.h"
#include "efx/TForever.h"

namespace efx {
struct TUsubaFireHit;

struct TParticleCallBack_UsubaFire : public JPAParticleCallBack {
	TParticleCallBack_UsubaFire()
	    : _04(1000.0f)
	    , mEfxHit(nullptr)
	{
	}

	virtual ~TParticleCallBack_UsubaFire() { }               // _08 (weak)
	virtual void execute(JPABaseEmitter*, JPABaseParticle*); // _0C
	virtual void init(JPABaseEmitter*, JPABaseParticle*);    // _14

	// _00      = VTBL
	f32 _04;                // _04
	TUsubaFireHit* mEfxHit; // _08
};

struct TUsubaFireHit : public TOneEmitterSimple {
	TUsubaFireHit()
	    : TOneEmitterSimple(PID_TankFireHit)
	{
	}

	virtual ~TUsubaFireHit() { } // _3C (weak)

	// _00      = VTBL
	// _00-_18  = TOneEmitterSimple
};

struct TUsubaFireABC : public TChaseMtx3 {
	inline TUsubaFireABC(Mtx mtx)
	    : TChaseMtx3(mtx, PID_TankFireABC_1, PID_TankFireABC_2, PID_TankFireABC_3)
	{
	}

	virtual bool create(Arg*); // _08
	virtual void forceKill()   // _0C (weak)
	{
		TChaseMtx3::forceKill();
		if (mParticleCallBack.mEfxHit != nullptr) {
			mParticleCallBack.mEfxHit->forceKill();
		}
	}
	virtual void fade() // _10 (weak)
	{
		TChaseMtx3::fade();
		if (mParticleCallBack.mEfxHit) {
			mParticleCallBack.mEfxHit->fade();
		}
	}
	virtual void startDemoDrawOff() // _14 (weak)
	{
		TChaseMtx3::startDemoDrawOff();
		mEfxFireHit.startDemoDrawOff();
	}
	virtual void endDemoDrawOn() // _18 (weak)
	{
		TChaseMtx3::endDemoDrawOn();
		mEfxFireHit.endDemoDrawOn();
	}

	// _00      = VTBL
	// _00-_40  = TChaseMtx3
	TParticleCallBack_UsubaFire mParticleCallBack; // _40
	TUsubaFireHit mEfxFireHit;                     // _4C
};

struct TUsubaFireIND : public TChaseMtx {
	TUsubaFireIND(Mtx mtx)
	    : TChaseMtx(PID_TankFireIND, (Matrixf*)mtx)
	{
	}

	virtual bool create(Arg*);   // _08
	virtual ~TUsubaFireIND() { } // _48 (weak)

	// _00      = VTBL
	// _00-_14  = TChaseMtx
	TParticleCallBack_UsubaFire mParticleCallBack;
};

struct TUsubaFire : public TBase {
	TUsubaFire(Mtx mtx)
	    : mEfxABC(mtx)
	    , mEfxIND(mtx)
	{
	}

	virtual bool create(Arg*); // _08
	virtual void forceKill()
	{
		mEfxABC.forceKill();
		mEfxIND.forceKill();
	}                   // _0C (weak)
	virtual void fade() // _10 (weak)
	{
		mEfxABC.fade();
		mEfxIND.fade();
	}

	// _00      = VTBL
	TUsubaFireABC mEfxABC; // _04
	TUsubaFireIND mEfxIND; // _6C
};

struct TUsubaFireYodare : public TChaseMtx {
	TUsubaFireYodare(Mtx mtx)
	    : TChaseMtx(PID_TankFireYodare, (Matrixf*)mtx)
	{
	}

	virtual ~TUsubaFireYodare() { } // _48 (weak)

	// _00      = VTBL
	// _00-_14  = TChaseMtx
};

struct TUsubaEffect {
	TUsubaEffect(Mtx mtx)
	    : mEfxFire(mtx)
	    , mEfxFireYodare(mtx)
	{
	}

	TUsubaFire mEfxFire;             // _00
	TUsubaFireYodare mEfxFireYodare; // _8C
};

struct TUsubaFireNew : public TChaseMtx {
	TUsubaFireNew()
	    : TChaseMtx(PID_UsubaFlamethrower, nullptr)
	{
	}
};

struct TUsubaFirefly : public TChaseMtx {
	TUsubaFirefly()
	    : TChaseMtx(PID_UsubaFirefly, nullptr)
	{
	}
};

struct TUsubaFireGround : public TForever2 {
	TUsubaFireGround()
	    : TForever2(PID_UsubaFlameGroundFire, PID_UsubaFlameGroundSmoke)
	{
	}
};

struct TUsubaElecBody : public TChaseMtx2 {
	TUsubaElecBody()
	    : TChaseMtx2(nullptr, PID_HallowElecBolts, PID_HallowElecGlow)
	{
	}
};

} // namespace efx

#endif
