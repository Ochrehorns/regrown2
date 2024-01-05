#include "Game/Entities/Hydrant.h"

namespace Game {
namespace Hydrant {

/*
 * --INFO--
 * Address:	8026152C
 * Size:	000050
 */
Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "HydrantMgr"; // hydrant manager
}

/*
 * --INFO--
 * Address:	8026157C
 * Size:	000048
 */
void Mgr::doAlloc() { init(new Houdai::Parms); }

/*
 * --INFO--
 * Address:	802615C4
 * Size:	000060
 */
void Mgr::createObj(int count) { mObj = new Obj[count]; }

/*
 * --INFO--
 * Address:	80261710
 * Size:	000010
 */
EnemyBase* Mgr::getEnemy(int index) { return &mObj[index]; }

} // namespace Hydrant
} // namespace Game
