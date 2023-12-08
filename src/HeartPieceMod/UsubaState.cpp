#include "Game/Entities/Usuba.h"
#include "Game/EnemyAnimKeyEvent.h"
#include "Game/EnemyFunc.h"
#include "Game/MapMgr.h"
#include "Game/rumble.h"
#include "Game/CameraMgr.h"
#include "Dolphin/rand.h"

/*
 * TODOS:
 *
 * implement Stay, Appear, Ground states
 * fix up Damage, Recover, AttackBreath states
 * fix up other states
 * connect up animation key frames properly
 * connect up functions for turning on/off zapping and fire
 *
 */

namespace Game {
namespace Usuba {

/*
 * --INFO--
 * Address:	802708A0
 * Size:	0003D0
 */
void FSM::init(EnemyBase* enemy)
{
	create(USUBA_StateCount);
	registerState(new StateDead);
	registerState(new StateStay);
	registerState(new StateAppear);
	registerState(new StateFall);
	registerState(new StateGround);
	registerState(new StateDamage);
	registerState(new StateRecover);
	registerState(new StateFlick);
	registerState(new StateWait);
	registerState(new StateMove);
	registerState(new StateAttackBreath);
}

/*
 * --INFO--
 * Address:	80270C70
 * Size:	000070
 */
void StateDead::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba = OBJ(enemy);
	usuba->deathProcedure();
	usuba->disableEvent(0, EB_Cullable);
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->disableEvent(0, EB_Untargetable);
	usuba->flickStickTarget();
	usuba->startMotion(USUBAANIM_Dead, nullptr);
}

/*
 * --INFO--
 * Address:	80270CE0
 * Size:	000044
 */
void StateDead::exec(EnemyBase* enemy)
{
	if (enemy->mCurAnim->mIsPlaying && enemy->mCurAnim->mType == KEYEVENT_END) {
		enemy->kill(nullptr);
	}
}

/*
 * --INFO--
 * Address:	80270D24
 * Size:	000004
 */
void StateDead::cleanup(EnemyBase* enemy) { }

// TODO: modify stay state (poached from Damagumo)
/*
 * --INFO--
 * Address:	80270C70
 * Size:	000070
 */
void StateStay::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba = OBJ(enemy);
	// usuba->mNextState = USUBA_NULL;
	usuba->enableEvent(0, EB_BitterImmune);
	usuba->disableEvent(0, EB_Animating);
	usuba->enableEvent(0, EB_ModelHidden);
	usuba->mTargetCreature = nullptr;

	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->startMotion(USUBAANIM_Appear, nullptr);
	usuba->stopMotion();
}

/*
 * --INFO--
 * Address:	80270CE0
 * Size:	000044
 */
void StateStay::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	bool isTarget;
	f32 detectRadius = static_cast<Parms*>(usuba->mParms)->mGeneral.mPrivateRadius.mValue;
	if (EnemyFunc::isThereOlimar(usuba, detectRadius, nullptr)) {
		isTarget = true;
	} else if (EnemyFunc::isTherePikmin(usuba, detectRadius, nullptr)) {
		isTarget = true;
	} else {
		isTarget = false;
	}

	if (isTarget) {
		transit(usuba, USUBA_Appear, nullptr);
	}
}

/*
 * --INFO--
 * Address:	80270D24
 * Size:	000004
 */
void StateStay::cleanup(EnemyBase* enemy) { }

// TODO: modify appear state (poached from snakecrow)
/*
 * --INFO--
 * Address:	80270C70
 * Size:	000070
 */
void StateAppear::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba = OBJ(enemy);
	usuba->enableEvent(0, EB_NoInterrupt);
	usuba->disableEvent(0, EB_Cullable);
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_Appear, nullptr);
	// usuba->createAppearEffect(0);

	Vector3f position = usuba->getPosition();
	cameraMgr->startVibration(6, position, 2);
	rumbleMgr->startRumble(15, position, 2);
}

/*
 * --INFO--
 * Address:	80270CE0
 * Size:	000044
 */
void StateAppear::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	if (usuba->mCurAnim->mIsPlaying) {
		if (usuba->mCurAnim->mType == KEYEVENT_2) {
			usuba->disableEvent(0, EB_NoInterrupt);
			// usuba->startBossAttackBGM();
			// usuba->startJointShadow();

		} else if (usuba->mCurAnim->mType == KEYEVENT_END) {
			// usuba->setAttackPosition();
			if (usuba->mHealth <= 0.0f) {
				transit(usuba, USUBA_Dead, nullptr);
				return;
			}

			// if (usuba->getAttackPiki(5) != nullptr || usuba->getAttackNavi(5)) {
			// 	if (snagret->getStickHeadPikmin() != 0) {
			// 		transit(usuba, SNAKECROW_Struggle, nullptr);
			// 	} else {
			// 		transit(usuba, SNAKECROW_Attack, nullptr);
			// 	}
			// } else {
			transit(usuba, USUBA_Wait, nullptr);
			// }
		}
	}
}

/*
 * --INFO--
 * Address:	80270D24
 * Size:	000004
 */
void StateAppear::cleanup(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	usuba->disableEvent(0, EB_NoInterrupt);
	usuba->enableEvent(0, EB_Cullable);
	// usuba->startWaitEffect();

	if (usuba->mWaterBox) {
		usuba->createEfxHamon();
	}
}

/*
 * --INFO--
 * Address:	80270D28
 * Size:	000068
 */
void StateFall::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba             = OBJ(enemy);
	usuba->_2C0            = 0.0f;
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->disableEvent(0, EB_Untargetable);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_Fall, nullptr);
	usuba->flickStickTarget();
}

/*
 * --INFO--
 * Address:	80270D90
 * Size:	0001B8
 */
void StateFall::exec(EnemyBase* enemy)
{
	Obj* usuba   = OBJ(enemy);
	Vector3f pos = usuba->getPosition();

	f32 minY = mapMgr->getMinY(pos);

	if (pos.y - minY < 10.0f || usuba->_2C0 > 1.0f) {
		usuba->finishMotion();
	}

	if (!usuba->isFinishMotion()) {
		f32 faceDir = usuba->getFaceDir() - 0.275f;

		clampAngle(faceDir);

		usuba->mFaceDir    = faceDir;
		usuba->mRotation.y = usuba->mFaceDir;
	}

	usuba->_2C0 += sys->mDeltaTime;

	if (usuba->mCurAnim->mIsPlaying) {
		if (usuba->mCurAnim->mType == KEYEVENT_2) {
			usuba->createDownEffect();
			rumbleMgr->startRumble(11, pos, 2);
		} else if (usuba->mCurAnim->mType == KEYEVENT_END) {
			if (usuba->mHealth <= 0.0f) {
				transit(usuba, USUBA_Dead, nullptr);
			} else {
				transit(usuba, USUBA_Damage, nullptr);
			}
		}
	}
}

/*
 * --INFO--
 * Address:	80270F48
 * Size:	000024
 */
void StateFall::cleanup(EnemyBase* enemy) { enemy->setEmotionCaution(); }

// TODO: implement Ground
/*
 * --INFO--
 * Address:	80270C70
 * Size:	000070
 */
void StateGround::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba = OBJ(enemy);
	usuba->deathProcedure();
	usuba->disableEvent(0, EB_Cullable);
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->disableEvent(0, EB_Untargetable);
	usuba->flickStickTarget();
	usuba->startMotion(USUBAANIM_Dead, nullptr);
}

/*
 * --INFO--
 * Address:	80270CE0
 * Size:	000044
 */
void StateGround::exec(EnemyBase* enemy)
{
	if (enemy->mCurAnim->mIsPlaying && enemy->mCurAnim->mType == KEYEVENT_END) {
		enemy->kill(nullptr);
	}
}

/*
 * --INFO--
 * Address:	80270D24
 * Size:	000004
 */
void StateGround::cleanup(EnemyBase* enemy) { }

/*
 * --INFO--
 * Address:	80270F6C
 * Size:	000068
 */
void StateDamage::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba             = OBJ(enemy);
	usuba->_2C0            = 0.0f;
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->disableEvent(0, EB_Untargetable);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_Damage, nullptr);
	usuba->flickStickTarget();
}

/*
 * --INFO--
 * Address:	80270FD4
 * Size:	000108
 */
void StateDamage::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	if (usuba->mHealth <= 0.0f || usuba->_2C0 > CG_PROPERPARMS(usuba).mFp23.mValue || usuba->getStickPikminNum() == 0) {
		usuba->finishMotion();
	}

	usuba->_2C0 += sys->mDeltaTime;

	if (usuba->mCurAnim->mIsPlaying && usuba->mCurAnim->mType == KEYEVENT_END) {
		if (usuba->mHealth <= 0.0f) {
			transit(usuba, USUBA_Dead, nullptr);
		} else {
			transit(usuba, USUBA_Recover, nullptr);
		}
	}
}

/*
 * --INFO--
 * Address:	802710DC
 * Size:	000024
 */
void StateDamage::cleanup(EnemyBase* enemy) { enemy->setEmotionCaution(); }

/*
 * --INFO--
 * Address:	80271100
 * Size:	00004C
 */
void StateRecover::init(EnemyBase* enemy, StateArg* stateArg)
{
	enemy->enableEvent(0, EB_Untargetable);
	enemy->setEmotionExcitement();
	enemy->startMotion(USUBAANIM_Recover, nullptr);
}

/*
 * --INFO--
 * Address:	8027114C
 * Size:	000108
 */
void StateRecover::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	f32 val    = usuba->setHeightVelocity();

	if (usuba->mHealth <= 0.0f || val > CG_PROPERPARMS(usuba).mFp03.mValue) {
		usuba->finishMotion();
	}

	if (usuba->mCurAnim->mIsPlaying && usuba->mCurAnim->mType == KEYEVENT_END) {
		int nextState = usuba->getNextStateOnHeight();
		if (nextState >= 0) {
			transit(usuba, nextState, nullptr);
			return;
		}

		// if (usuba->getCatchTargetNum()) {
		// 	transit(usuba, USUBA_CatchFly, nullptr);
		// 	return;
		// }

		transit(usuba, USUBA_Move, nullptr);
	}
}

/*
 * --INFO--
 * Address:	80271254
 * Size:	000024
 */
void StateRecover::cleanup(EnemyBase* enemy) { enemy->setEmotionCaution(); }

/*
 * --INFO--
 * Address:	80271278
 * Size:	000058
 */
void StateFlick::init(EnemyBase* enemy, StateArg* stateArg)
{
	enemy->enableEvent(0, EB_Untargetable);
	enemy->enableEvent(0, EB_NoInterrupt);
	enemy->setEmotionExcitement();
	enemy->startMotion(USUBAANIM_Flick, nullptr);
}

/*
 * --INFO--
 * Address:	802712D0
 * Size:	000118
 */
void StateFlick::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	usuba->setHeightVelocity();

	if (usuba->mCurAnim->mIsPlaying) {
		if (usuba->mCurAnim->mType == KEYEVENT_2) {
			EnemyFunc::flickStickPikmin(usuba, CG_PARMS(usuba)->mGeneral.mShakeRateMaybe.mValue,
			                            CG_PARMS(usuba)->mGeneral.mShakeKnockback.mValue, CG_PARMS(usuba)->mGeneral.mShakeDamage.mValue,
			                            FLICK_BACKWARD_ANGLE, nullptr);
			usuba->mToFlick = 0.0f;

		} else if (usuba->mCurAnim->mType == KEYEVENT_END) {
			if (usuba->mHealth <= 0.0f) {
				transit(usuba, USUBA_Fall, nullptr);
				return;
			}

			// if (usuba->getCatchTargetNum()) {
			// 	transit(usuba, USUBA_CatchFly, nullptr);
			// 	return;
			// }

			transit(usuba, USUBA_Move, nullptr);
		}
	}
}

/*
 * --INFO--
 * Address:	802713E8
 * Size:	000030
 */
void StateFlick::cleanup(EnemyBase* enemy)
{
	enemy->disableEvent(0, EB_NoInterrupt);
	enemy->setEmotionCaution();
}

/*
 * --INFO--
 * Address:	80271418
 * Size:	0000B0
 */
void StateWait::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba             = OBJ(enemy);
	usuba->_2C0            = 0.0f;
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->mTargetCreature = nullptr;
	usuba->enableEvent(0, EB_Untargetable);

	if (randWeightFloat(1.0f) < 0.5f) {
		usuba->startMotion(USUBAANIM_Wait, nullptr);
	} else {
		usuba->startMotion(USUBAANIM_Move, nullptr);
	}
}

/*
 * --INFO--
 * Address:	802714C8
 * Size:	000160
 */
void StateWait::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	f32 val    = usuba->setHeightVelocity();

	FakePiki* target = usuba->getAttackableTarget();
	if (target || usuba->_2C0 > CG_PROPERPARMS(usuba).mFp06.mValue) {
		usuba->finishMotion();
	}

	if (val > CG_PROPERPARMS(usuba).mFp03.mValue || usuba->_2C0 > 3.0f) {
		int nextState = usuba->getNextStateOnHeight();
		if (nextState >= 0) {
			transit(usuba, nextState, nullptr);
			return;
		}
	}

	usuba->_2C0 += sys->mDeltaTime;

	if (usuba->mCurAnim->mIsPlaying && usuba->mCurAnim->mType == KEYEVENT_END) {
		if (target) {
			usuba->mTargetCreature = target;
			transit(usuba, USUBA_AttackBreath, nullptr);
			return;
		}

		transit(usuba, USUBA_Move, nullptr);
	}
}

/*
 * --INFO--
 * Address:	80271628
 * Size:	000004
 */
void StateWait::cleanup(EnemyBase* enemy) { }

/*
 * --INFO--
 * Address:	8027162C
 * Size:	00005C
 */
void StateMove::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba  = OBJ(enemy);
	usuba->_2C0 = 0.0f;
	usuba->setRandTarget();
	usuba->mTargetCreature = nullptr;
	usuba->enableEvent(0, EB_Untargetable);
	usuba->startMotion(USUBAANIM_Move, nullptr);
}

/*
 * --INFO--
 * Address:	80271688
 * Size:	000208
 */
void StateMove::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);

	f32 height         = usuba->setHeightVelocity();
	Vector3f pos       = usuba->getPosition();
	Vector3f targetPos = Vector3f(usuba->mTargetPos);

	FakePiki* target = usuba->getAttackableTarget();

	if (target || usuba->_2C0 > 10.0f || sqrDistanceXZ(pos, targetPos) < 625.0f) {
		usuba->mTargetVelocity = Vector3f(0.0f);
		usuba->finishMotion();
	} else {
		EnemyFunc::walkToTarget(usuba, targetPos, CG_PROPERPARMS(usuba).mFp04.mValue, CG_PARMS(usuba)->mGeneral.mRotationalAccel.mValue,
		                        CG_PARMS(usuba)->mGeneral.mRotationalSpeed.mValue);
	}

	if (height > CG_PROPERPARMS(usuba).mFp03.mValue || usuba->_2C0 > 3.0f) {
		int nextState = usuba->getNextStateOnHeight();
		if (nextState >= 0) {
			transit(usuba, nextState, nullptr);
			return;
		}
	}

	usuba->_2C0 += sys->mDeltaTime;

	if (usuba->mCurAnim->mIsPlaying && usuba->mCurAnim->mType == KEYEVENT_END) {
		if (target) {
			usuba->mTargetCreature = target;
			transit(usuba, USUBA_AttackBreath, nullptr);
			return;
		}

		transit(usuba, USUBA_Wait, nullptr);
	}
}

/*
 * --INFO--
 * Address:	80271890
 * Size:	000004
 */
void StateMove::cleanup(EnemyBase* enemy) { }

/*
 * --INFO--
 * Address:	80271894
 * Size:	000078
 */
void StateAttackBreath::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba  = OBJ(enemy);
	usuba->_2C0 = 0.0f;
	usuba->disableEvent(0, EB_Cullable);
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->enableEvent(0, EB_NoInterrupt);
	usuba->enableEvent(0, EB_Untargetable);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_AttackBreath, nullptr);
}

/*
 * --INFO--
 * Address:	8027190C
 * Size:	000614
 */
void StateAttackBreath::exec(EnemyBase* enemy)
{
	Obj* usuba       = OBJ(enemy);
	Creature* target = usuba->mTargetCreature;
	if (target) {
		f32 frame = usuba->getMotionFrame();
		if (frame <= 10.0f) {
			usuba->setHeightVelocity();
			usuba->changeFaceDir2(target);
		} else if (frame <= 30.0f) {
			if (usuba->mBounceTriangle) {
				usuba->_2C0 = 30.0f;
			}

			if (usuba->_2C0 < 30.0f) {
				f32 targetHeight = target->getPosition().y + 17.5f;
				f32 usubaHeight  = usuba->getPosition().y;

				Vector3f vel = usuba->getVelocity();
				f32 factor   = ((targetHeight - usubaHeight) / sys->mDeltaTime) * *CG_PROPERPARMS(usuba).mFp31();

				f32 vertSpeed;
				if (factor < -2500.0f) {
					vertSpeed = -2500.0f;
				} else if (factor > 1000.0f) {
					vertSpeed = 1000.0f;
				} else {
					vertSpeed = factor;
				}
				vel.y = vertSpeed;
				usuba->setVelocity(vel);
				if (frame > 16.0f) {
					usuba->catchTarget();
				}
				usuba->changeFaceDir2(target);
			}
		} else {
			usuba->setHeightVelocity();
			f32 decayRate          = *CG_PROPERPARMS(usuba).mFp32();
			usuba->mTargetVelocity = usuba->mTargetVelocity * decayRate;
		}
	} else {
		transit(usuba, USUBA_Move, nullptr);
	}

	if (usuba->mCurAnim->mIsPlaying) {
		if (usuba->mCurAnim->mType == KEYEVENT_2) {
			if (usuba->mTargetCreature) {
				Vector3f targetPos = usuba->mTargetCreature->getPosition();
				Vector3f usubaPos  = usuba->getPosition();

				Vector3f sep = targetPos - usubaPos;
				Vector3f dir = sep;
				dir.normalise();
				dir *= 25.0f;
				sep -= dir;
				sep.y = 0.0f;
				sep *= (0.06666667f / sys->mDeltaTime);
				usuba->setVelocity(sep);
				usuba->mTargetVelocity = sep;
			}
		} else if (usuba->mCurAnim->mType == KEYEVENT_3) {
			usuba->disableEvent(0, EB_NoInterrupt);

		} else if (usuba->mCurAnim->mType == KEYEVENT_END) {
			transit(usuba, USUBA_Move, nullptr);
		}
	}
}

/*
 * --INFO--
 * Address:	80271F20
 * Size:	000044
 */
void StateAttackBreath::cleanup(EnemyBase* enemy)
{
	enemy->enableEvent(0, EB_Cullable);
	enemy->disableEvent(0, EB_NoInterrupt);
	enemy->mTargetCreature = nullptr;
	enemy->setEmotionCaution();
}

} // namespace Usuba
} // namespace Game
