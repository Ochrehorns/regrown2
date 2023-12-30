#include "types.h"
#include "efx/TUsuba.h"

namespace efx {

/*
 * --INFO--
 * Address:	803B43A8
 * Size:	000004
 */
void TParticleCallBack_UsubaFire::init(JPABaseEmitter*, JPABaseParticle*) { }

/*
 * --INFO--
 * Address:	803B43AC
 * Size:	000144
 */
void TParticleCallBack_UsubaFire::execute(JPABaseEmitter* emit, JPABaseParticle* particle)
{
	f32 z = particle->getCalcCurrentPositionZ(emit);
	f32 y = particle->getCalcCurrentPositionY(emit);
	f32 x = particle->getCalcCurrentPositionX(emit);
	Vector3f tgt(x, y, z);

	if (tgt.distance(emit->mPosition) > _04) {
		particle->mFlags |= 2;

		TUsubaFireHit* hit = mEfxHit;
		if (hit && hit->_14 < hit->_18) {
			Vector3f* pos = &hit->_10[hit->_14];
			pos->x        = x;
			pos->y        = y;
			pos->z        = z;
			hit->_14++;
		}
	}
}

bool TUsubaFireABC::create(Arg* arg)
{
	mParticleCallBack.mEfxHit = &mEfxFireHit;
	mParticleCallBack.mEfxHit->create(nullptr);
	mParticleCallBack._04 = 1000.0f;
	if (TSyncGroup3<TChaseMtx>::create(arg)) {
		for (int i = 0; i < 3; i++) {
			mItems[i].mEmitter->mParticleCallback = &mParticleCallBack;
		}
	}
	return true;
}

bool TUsubaFireIND::create(Arg* arg)
{
	if (TSync::create(arg)) {
		mEmitter->mParticleCallback = &mParticleCallBack;
	}
	return true;
}

bool TUsubaFire::create(Arg* arg)
{
	bool ret = false;
	if (mEfxABC.create(arg) && mEfxIND.create(arg)) {
		ret = true;
	}
	return ret;
}
} // namespace efx
