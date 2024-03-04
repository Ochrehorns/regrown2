#include "Game/Entities/Clam.h"
#include "Game/Entities/Pearl.h"
#include "Game/generalEnemyMgr.h"
#include "Radar.h"
#include "Dolphin/rand.h"

namespace Game {
namespace Clam {

Obj::Obj()
{
	mAnimator = new ProperAnimator();
	setFSM(new FSM());
	createEffect();

	mPearl    = nullptr;
	mTreasure = nullptr;
}

void Obj::setFSM(FSM* fsm)
{
	mFsm = fsm;
	mFsm->init(this);
	mCurrentLifecycleState = nullptr;
}

void Obj::getShadowParam(ShadowParam& shadow)
{
	shadow.mPosition = mPosition;
	shadow.mPosition.z -= 7.5f;
	shadow.mPosition.y += 5.0f;

	shadow.mBoundingSphere.mPosition.set(0.0f, 1.0f, 0.0f);
	shadow.mBoundingSphere.mRadius = 35.0f;
	shadow.mSize                   = 35.0f;
}

void Obj::createEffect() { mSparkleEfx = new efx::TClamSparkles; }

void Obj::doUpdate()
{
	mFsm->exec(this);

	if (mTreasure) {
		Matrixf mtx;
		PSMTXIdentity(mtx.mMatrix.mtxView);

		Vector3f pos = mtx.getTranslation();
		pos.x += 6.0f;
		Vector3f angle(0.0f, -1.5707f, -1.5707f);
		mtx.makeTR(pos, angle);
		mTreasure->updateCapture(mtx);
	}

	if (mPearl && mPearl->isAlive()) {
		Matrixf mtx;
		PSMTXIdentity(mtx.mMatrix.mtxView);
		mtx.mMatrix.structView.tx += 3.0f;
		mPearl->updateCapture(mtx);
	}
}

void Obj::onInit(CreatureInitArg* carg)
{
	EnemyBase::onInit(carg);

	mFsm->start(this, 0, nullptr);

	enableEvent(0, EB_Untargetable);
	disableEvent(0, EB_LeaveCarcass);
	disableEvent(0, EB_PlatformCollEnabled);
	enableEvent(0, EB_BitterImmune);

	PlatAddInstanceArg arg;
	arg.mItem     = (BaseItem*)this;
	arg.mMatrix   = mModel->getJoint("root")->getWorldMatrix();
	arg.mId       = 'cbot';
	arg.mPlatform = static_cast<Clam::Mgr*>(mMgr)->mPlatBottom;
	// arg.mRadius   = 90.0f;
	mPlatBottom = platMgr->addInstance(arg);

	PlatAddInstanceArg arg2;
	arg2.mItem     = (BaseItem*)this;
	arg2.mMatrix   = mModel->getJoint("top")->getWorldMatrix();
	arg2.mId       = 'ctop';
	arg2.mPlatform = static_cast<Clam::Mgr*>(mMgr)->mPlatTop;
	// arg2.mRadius   = 90.0f;
	mPlatTop = platMgr->addInstance(arg2);

	birthPearl();

	// enableEvent(0, EB_AttackingNavi0);
	// enableEvent(0, EB_AttackingNavi1);

	mSparkleEfx->mMtx = mModel->getJoint("tongue")->getWorldMatrix();
	mSparkleEfx->create(nullptr);
}

void Obj::setInitialSetting(EnemyInitialParamBase*) { }

void Obj::doDirectDraw(Graphics&) { }

void Obj::platCallback(PlatEvent& event)
{
	PlatInstance* plat = event.mInstance;

	if (getStateID() == 0 && plat->mId == 'cbot' && mCanEat) {
		mFsm->transit(this, ClamState_Attack, nullptr);
	}
}

void Obj::birthPearl()
{
	EnemyMgrBase* mgr = generalEnemyMgr->getEnemyMgr(EnemyTypeID::EnemyID_Pearl);
	if (!mgr)
		return;

	Matrixf* mtx = mModel->getJoint("tongue")->getWorldMatrix();

	EnemyBirthArg args;
	args.mFaceDir     = getFaceDir();
	Pearl::Obj* pearl = static_cast<Pearl::Obj*>(mgr->birth(args));
	if (!pearl)
		return;

	mPearl             = pearl;
	mPearl->mOwnerClam = this;
	mPearl->init(nullptr);
	mPearl->startCapture(mtx);
	mPearl->setAtari(true); // makes pearl have active collision while captured

	// if the clam has a ship part, make it hold the part
	if (!mPelletDropCode.isNull()) {
		PelletInitArg arg;
		pelletMgr->makePelletInitArg(arg, mPelletDropCode);

		mTreasure = pelletMgr->birth(&arg);
		if (mTreasure) {
			mPelletDropCode.mValue = 0;
			Radar::mgr->exit(this);
			mTreasure->startCapture(mtx);
			mTreasure->mScale = C_PROPERPARMS.mPartScale();
		}
	}
}

} // namespace Clam
} // namespace Game
