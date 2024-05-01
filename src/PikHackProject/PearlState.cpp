#include "Game/Entities/Pearl.h"
#include "Game/Entities/Clam.h"
#include "Dolphin/rand.h"

namespace Game {
namespace Pearl {

/*
 * --INFO--
 * Address:	802708A0
 * Size:	0003D0
 */
void FSM::init(EnemyBase* enemy)
{
	create(PearlState_Count);
	registerState(new StateIdle);
	registerState(new StateDead);
}

void StateIdle::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(2, nullptr);
	obj->hardConstraintOn();
}

void StateIdle::exec(EnemyBase* obj)
{
	if (obj->mHealth <= 0.0f && obj->isAlive()) {
		obj->mSoundObj->startSound(PSSE_EN_PIKIMAKI_HIT, 0);
		OBJ(obj)->mFsm->transit(obj, PearlState_Dead, nullptr);
	}
}

void StateIdle::cleanup(EnemyBase* obj) { obj->hardConstraintOff(); }

void StateDead::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(3, nullptr);
	obj->deathProcedure();

	Pellet* shippart = nullptr;
	if (OBJ(obj)->mOwnerClam) {
		shippart = OBJ(obj)->mOwnerClam->mTreasure;
		OBJ(obj)->mOwnerClam->setZukanVisible(true);
	}

	if (shippart) {
		obj->disableEvent(0, EB_LeaveCarcass);
		OBJ(obj)->mOwnerClam->mTreasure = nullptr;
	}

	obj->endCapture();
	obj->mPosition.y += 15.0f;

	f32 facedir = obj->getFaceDir();
	Vector3f vel(sinf(facedir) * 120.0f, 180.0f, cosf(facedir) * 120.0f);
	obj->setVelocity(vel);
	obj->mTargetVelocity = vel;

	if (shippart) {
		obj->kill(nullptr);

		shippart->mPelletPosition.y += 35.0f;
		shippart->endCapture();

		Vector3f vel(sinf(facedir) * 120.0f, 180.0f, cosf(facedir) * 120.0f);
		shippart->setVelocity(vel);
		shippart->mScale = 1.0f;
		obj->mSoundObj->startSound(PSSE_EN_ENEMY_LOOSE_ITEM, 0);
	}

	efx::TClamDead deadefx;
	deadefx.mMtx = &obj->mBaseTrMatrix;
	deadefx.create(nullptr);

	obj->mSoundObj->startSound(0x598e, 0);
}

void StateDead::exec(EnemyBase* obj)
{
	if (obj->mHealth <= 0.0f && obj->isAlive()) {
		OBJ(obj)->mFsm->transit(obj, PearlState_Dead, nullptr);
	}
}

void StateDead::cleanup(EnemyBase* obj) { }

} // namespace Pearl
} // namespace Game
