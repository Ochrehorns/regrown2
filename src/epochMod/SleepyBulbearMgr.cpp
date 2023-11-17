#include "Game/Entities/SleepyBulbear.h"

namespace Game {
namespace SleepyBulbear {

Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "SleepyBulbearMgr";
}

void Mgr::doAlloc() { init(new ChappyBase::Parms); }

void Mgr::createObj(int count) { mObj = new Obj[count]; }

EnemyBase* Mgr::getEnemy(int index) { return &mObj[index]; }

void Mgr::loadModelData()
{
	EnemyMgrBase::loadModelData();
	J3DShape* shape;
	for (u16 j = 0; j < mModelData->getShapeNum(); j++) {
		shape         = mModelData->mShapeTable.mItems[j];
		shape->mFlags = (shape->mFlags & (~0xF000)) | 0x2000;
	}
}

} // namespace SleepyBulbear
} // namespace Game
