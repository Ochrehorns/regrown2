#include "Game/Entities/SmokyFrog.h"
#include "Game/Navi.h"
#include "efx/TFrog.h"

namespace Game {
namespace SmokyFrog {

Obj::Obj()
{
	createEffect(); 
	// setScale(C_PROPERPARMS.mScaleMult.mValue); // don't assign parm shit in the ctor
}

void Obj::attackNaviPosition()
{
	Iterator<Navi> iter(naviMgr);

	CI_LOOP(iter)
	{
		Navi* navi = (*iter);
		if (navi->isAlive()) {
			f32 minAttackRange = static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMinAttackRange.mValue;
			f32 maxAttackRange = static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMaxAttackRange.mValue;

			f32 angledist = getCreatureViewAngle(navi);

			if (checkDistAndAngle(navi, angledist, maxAttackRange, minAttackRange)) {
				mTargetPosition = Vector3f(navi->getPosition());
			}
		}
	}
}

void Obj::createEffect() { mEfxPota = new efx::TFrogPota; }

} // namespace SmokyFrog
} // namespace Game
