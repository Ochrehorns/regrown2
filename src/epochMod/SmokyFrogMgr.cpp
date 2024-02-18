#include "Game/Entities/SmokyFrog.h"
#include "LoadResource.h"
#include "Sys/MatBaseAnimation.h"

namespace Game {
namespace SmokyFrog {

/*
 * --INFO--
 * Address:	8026152C
 * Size:	000050
 */
Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "SmokyFrogMgr"; // smoky frog manager
}

/*
 * --INFO--
 * Address:	8026157C
 * Size:	000048
 */
void Mgr::doAlloc() { init(new Frog::Parms); }

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

void Mgr::loadTexData()
{
	SysShape::Model::enableMaterialAnim(mModelData, 0);
	void* btkFile = nullptr;
	LoadResource::Arg loadArgBtk("/enemy/data/SmokyFrog/dororo_model.btk");
	LoadResource::Node* resourceBtk = gLoadResourceMgr->load(loadArgBtk);
	if (resourceBtk) {
		btkFile = resourceBtk->mFile;
	}

	mTexAnimation = new Sys::MatTexAnimation;
	mTexAnimation->attachResource(btkFile, mModelData);
}

SysShape::Model* Mgr::createModel()
{
	SysShape::Model* model = new SysShape::Model(mModelData, 0x40000, mModelType);
	return model;
}

} // namespace SmokyFrog
} // namespace Game
