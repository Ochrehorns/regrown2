#include "Game/Entities/Clam.h"
#include "Game/EnemyAnimKeyEvent.h"
#include "Game/EnemyFunc.h"
#include "Game/MapMgr.h"
#include "Game/rumble.h"
#include "Game/CameraMgr.h"
#include "Dolphin/rand.h"

namespace Game {
namespace Clam {

/*
 * --INFO--
 * Address:	802708A0
 * Size:	0003D0
 */
void FSM::init(EnemyBase* enemy)
{
	create(ClamState_Count);
	registerState(new StateWait);
	registerState(new StateAttack);
	registerState(new StateShut);
	registerState(new StateEat);
}

void StateWait::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(3, nullptr);
	obj->hardConstraintOn();
	obj->setEmotionCaution();
	OBJ(obj)->mCanEat = false;
}

void StateWait::exec(EnemyBase* obj)
{
	if (obj->mCurAnim->mIsPlaying) {
		OBJ(obj)->mCanEat = true;
	}
}

void StateWait::cleanup(EnemyBase* obj) { }

void StateAttack::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(0, nullptr);
	OBJ(obj)->mOpenTimer = 0.0f;
	obj->setEmotionExcitement();
}

void StateAttack::exec(EnemyBase* obj)
{
	if (obj->mCurAnim->mType == 1000 && obj->mCurAnim->mIsPlaying) {
		OBJ(obj)->mFsm->transit(obj, ClamState_Shut, nullptr);
	}

	OBJ(obj)->mOpenTimer += sys->mDeltaTime;
	if (OBJ(obj)->mOpenTimer > CG_PROPERPARMS(OBJ(obj)).mAttackWaitTime()) {
		obj->finishMotion();
	}
}

void StateAttack::cleanup(EnemyBase* obj) { }

void StateShut::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(1, nullptr);
	OBJ(obj)->mKilledPiki = false;
}

void StateShut::exec(EnemyBase* obj)
{
	Obj* clam = OBJ(obj);

	// on mouth shut, eat pikmin in mouth range
	if (clam->mCurAnim->mType == 2 && clam->mCurAnim->mIsPlaying) {
		Sys::Sphere bound;
		Matrixf* mtx    = clam->mModel->getJoint("tongue")->getWorldMatrix();
		bound.mPosition = mtx->getTranslation();
		bound.mRadius   = CG_GENERALPARMS(clam).mAttackRadius();

		CellIteratorArg arg(bound);
		CellIterator c_it(arg);
		CI_LOOP(c_it)
		{
			Creature* cpiki = (Creature*)(*c_it);
			if (cpiki->isAlive()) {
				InteractPress act(clam, CG_GENERALPARMS(clam).mAttackDamage(), nullptr);
				cpiki->stimulate(act);
				if (cpiki->isPiki())
					clam->mKilledPiki = true;
			}
		}

		efx::TClamShut shutefx;
		shutefx.mMtx = mtx;
		shutefx.create(nullptr);
	}

	// on anim finished, go to eat animation if it got any pikmin
	if (clam->mCurAnim->mType = 1000 && clam->mCurAnim->mIsPlaying) {
		if (!clam->mKilledPiki) {
			clam->mFsm->transit(clam, ClamState_Wait, 0);
		} else {
			clam->mFsm->transit(clam, ClamState_Eat, 0);
		}
	}
}

void StateShut::cleanup(EnemyBase* obj) { }

void StateEat::init(EnemyBase* obj, StateArg* arg) { obj->startMotion(2, nullptr); }

void StateEat::exec(EnemyBase* obj)
{
	if (obj->mCurAnim->mType = 1000 && obj->mCurAnim->mIsPlaying) {
		OBJ(obj)->mFsm->transit(obj, ClamState_Wait, 0);
	}
}

void StateEat::cleanup(EnemyBase* obj) { }

} // namespace Clam
} // namespace Game
