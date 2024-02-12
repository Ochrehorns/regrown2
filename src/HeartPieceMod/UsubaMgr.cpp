#include "Game/Entities/Usuba.h"

namespace Game {
namespace Usuba {

Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "UsubaMgr";
}

void Mgr::doAlloc() { init(new Parms); }

void Mgr::createObj(int count) { mObj = new Obj[count]; }

EnemyBase* Mgr::getEnemy(int index) { return &mObj[index]; }

void Mgr::loadModelData()
{
	EnemyMgrBase::loadModelData();
	J3DShape* shape;
	for (u16 j = 0; j < mModelData->getShapeNum(); j++) {
		shape = mModelData->mShapeTable.mItems[j];
		shape->setTexMtxLoadType(0x2000);
	}
}

} // namespace Usuba
} // namespace Game
