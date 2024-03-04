#include "Game/Entities/Clam.h"
#include "Game/Entities/Pearl.h"
#include "Game/generalEnemyMgr.h"
#include "Radar.h"
#include "Dolphin/rand.h"

namespace Game {
namespace Pearl {

Obj::Obj()
{
	mAnimator = new ProperAnimator();
	setFSM(new FSM());

	mOwnerClam = nullptr;
	mMatAnim   = new Sys::MatLoopAnimator;
}

void Obj::setFSM(FSM* fsm)
{
	mFsm = fsm;
	mFsm->init(this);
	mCurrentLifecycleState = nullptr;
}

void Obj::changeMaterial()
{
	J3DModel* model = mModel->mJ3dModel;

	Pellet* shippart = nullptr;
	if (mOwnerClam)
		shippart = mOwnerClam->mTreasure;

	if (shippart) {
		// hide pearl center if a part is present
		model->mModelData->mMaterialTable.mMaterials[0]->mShape->onFlag(1);
	}

	if (isAlive()) {
		mMatAnim->animate(30.0f);
		model->calc();
		model->calcMaterial();
		model->diff();
	}
}

void Obj::getShadowParam(ShadowParam& shadow)
{
	shadow.mPosition = mPosition;
	shadow.mPosition.y += 3.0f;

	shadow.mBoundingSphere.mPosition.set(0.0f, 1.0f, 0.0f);
	shadow.mBoundingSphere.mRadius = 8.0f;
	shadow.mSize                   = 8.0f;
}

void Obj::doUpdate() { mFsm->exec(this); }

void Obj::onInit(CreatureInitArg* carg)
{
	EnemyBase::onInit(carg);

	enableEvent(0, EB_BitterImmune);

	mFsm->start(this, 0, nullptr);

	mIsAlive = true;
	mMatAnim->start(static_cast<Mgr*>(mMgr)->mMatAnim);
}

void Obj::setInitialSetting(EnemyInitialParamBase*) { }

void Obj::doDirectDraw(Graphics&) { }

void Obj::doDebugDraw(Graphics& gfx) { EnemyBase::doDebugDraw(gfx); }

void Obj::bounceCallback(Sys::Triangle*)
{
	if (getStateID() == PearlState_Dead && mIsAlive) {
		kill(nullptr);
		mIsAlive = 0;
	}
}

bool Obj::damageCallBack(Creature* owner, f32 damage, CollPart* part)
{
	if (owner->isNavi())
		return false;

	bool hit = EnemyBase::damageCallBack(owner, damage, part);
	if (hit && getStateID() == PearlState_Idle) {
		startMotion(1, nullptr);
	}
	return hit;
}

void Obj::startCarcassMotion() { startMotion(3, nullptr); }

} // namespace Pearl
} // namespace Game
