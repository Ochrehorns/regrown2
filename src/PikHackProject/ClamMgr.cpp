#include "Game/Entities/Clam.h"

namespace Game {
namespace Clam {

Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "ClamMgr";

	JKRArchive* arc = JKRArchive::mount("enemy/data/Clam/plat.szs", (JKRArchive::EMountMode)1, 0, (JKRArchive::EMountDirection)1);

	mPlatBottom = new Platform;
	mPlatBottom->load(arc, "plat_bottom.bin");
	mPlatTop = new Platform;
	mPlatTop->load(arc, "plat_top.bin");
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

} // namespace Clam
} // namespace Game
