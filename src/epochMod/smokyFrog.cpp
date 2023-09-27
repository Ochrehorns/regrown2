#include "Game/Entities/SmokyFrog.h"
#include "Game/Navi.h"
#include "efx/TFrog.h"

namespace Game {
namespace SmokyFrog {

/*
 * --INFO--
 * Address:	802617E0
 * Size:	000098
 */
Obj::Obj() { createEffect(); }

/*
 * --INFO--
 * Address:	80261878
 * Size:	00045C
 */
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

/*
 * --INFO--
 * Address:	80261CD4
 * Size:	0000B0
 */
void Obj::createEffect() { mEfxPota = new efx::TFrogPota; }

} // namespace SmokyFrog
} // namespace Game
