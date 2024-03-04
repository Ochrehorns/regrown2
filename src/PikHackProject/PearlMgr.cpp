#include "Game/Entities/Pearl.h"
#include "LoadResource.h"
#include "Sys/MatBaseAnimation.h"

namespace Game {
namespace Pearl {

Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "PearlMgr";
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

void Mgr::loadTexData()
{
	SysShape::Model::enableMaterialAnim(mModelData, 0);

	LoadResource::Arg arg("/enemy/data/Pearl/anims.btk");
	LoadResource::Node* node = gLoadResourceMgr->load(arg);
	void* file               = node->mFile;

	mMatAnim = new Sys::MatTexAnimation;
	mMatAnim->attachResource(file, mModelData);
}

SysShape::Model* Mgr::createModel()
{
	SysShape::Model* model = new SysShape::Model(mModelData, 0x80000, mModelType);

	model->mJ3dModel->calc();
	model->mJ3dModel->calcMaterial();
	model->mJ3dModel->makeDL();
	return model;
}

} // namespace Pearl
} // namespace Game
