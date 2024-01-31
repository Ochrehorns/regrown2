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
 * add AttackDive state
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
	registerState(new StateAttackDive);
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
	usuba->enableEvent(0, EB_Untargetable);

	EnemyFunc::flickStickPikmin(usuba, CG_PARMS(usuba)->mGeneral.mShakeChance.mValue, CG_PARMS(usuba)->mGeneral.mShakeKnockback.mValue,
	                            CG_PARMS(usuba)->mGeneral.mShakeDamage.mValue, FLICK_BACKWARD_ANGLE, nullptr);
	usuba->mFlickTimer = 0.0f;
	usuba->startMotion(USUBAANIM_Dead, nullptr);

	usuba->endElec();

	
}

/*
 * --INFO--
 * Address:	80270CE0
 * Size:	000044
 */
void StateDead::exec(EnemyBase* enemy)
{
	f32 minY = mapMgr->getMinY(enemy->mPosition);

	if (enemy->mPosition.y - minY < 10.0f) {
		enemy->finishMotion();
	}
	if (enemy->mCurAnim->mIsPlaying) {
		switch (enemy->mCurAnim->mType) {
		case KEYEVENT_END:
			enemy->kill(nullptr);
			return;
		case KEYEVENT_2:
			enemy->getJAIObject()->startSound(PSSE_EN_YOROI_DEAD, 0);
			enemy->disableEvent(0, EB_Untargetable);
			break;
		case KEYEVENT_3:
			enemy->enableEvent(0, EB_Untargetable);
			OBJ(enemy)->createDownEffect();
			enemy->getJAIObject()->startSound(PSSE_EN_FROG_LAND, 0);
			rumbleMgr->startRumble(11, enemy->mPosition, 2);
			break;
		}
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
	usuba->enableEvent(0, EB_Cullable);
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

	OSReport("in appear state\n");

	usuba->enableEvent(0, EB_NoInterrupt);
	usuba->disableEvent(0, EB_Cullable);
	usuba->disableEvent(0, EB_ModelHidden);
	usuba->disableEvent(0, EB_BitterImmune);
	usuba->enableEvent(0, EB_Animating);
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_Appear, nullptr);
	usuba->createAppearEffect();

	usuba->mSoundObj->startSound(PSSE_EN_SNAKE_APPEAR, 0);

	usuba->setBossAppearBGM();

	mIsRising = false;


	

	// Vector3f position = usuba->getPosition();
	// cameraMgr->startVibration(6, position, 2);
	// rumbleMgr->startRumble(15, position, 2);
}

/*
 * --INFO--
 * Address:	80270CE0
 * Size:	000044
 */
void StateAppear::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);

	if (mIsRising) {
		usuba->setHeightVelocity();
	}

	if (usuba->mCurAnim->mIsPlaying) {
		switch (usuba->mCurAnim->mType) {
		case KEYEVENT_2:
			// flick upon leaving ground to prevent any cheese
			EnemyFunc::flickNearbyPikmin(enemy, CG_PARMS(enemy)->mGeneral.mShakeRange.mValue,
			                             CG_PARMS(enemy)->mGeneral.mShakeKnockback.mValue, CG_PARMS(enemy)->mGeneral.mShakeDamage.mValue,
			                             FLICK_BACKWARD_ANGLE, nullptr);
			EnemyFunc::flickNearbyNavi(enemy, CG_PARMS(enemy)->mGeneral.mShakeRange.mValue,
								CG_PARMS(enemy)->mGeneral.mShakeKnockback.mValue, CG_PARMS(enemy)->mGeneral.mShakeDamage.mValue,
								FLICK_BACKWARD_ANGLE, nullptr);
			EnemyFunc::flickStickPikmin(enemy, CG_PARMS(enemy)->mGeneral.mShakeChance.mValue,
								CG_PARMS(enemy)->mGeneral.mShakeKnockback.mValue, CG_PARMS(enemy)->mGeneral.mShakeDamage.mValue,
								FLICK_BACKWARD_ANGLE, nullptr);
			usuba->disableEvent(0, EB_NoInterrupt);
			mIsRising = true;
			break;
		case KEYEVENT_3:
			usuba->startElec();
			break;
		case KEYEVENT_END:
			if (usuba->mHealth <= 0.0f) {
				transit(usuba, USUBA_Dead, nullptr);
				return;
			}

			transit(usuba, USUBA_Wait, nullptr);
			break;
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

	// dude, this guy has wings
	if (usuba->mWaterBox) {
		// usuba->createEfxHamon();
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
	usuba->mStateTimer     = 0.0f;
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_Fall, nullptr);
	usuba->endElec();
	usuba->disableEvent(0, EB_Untargetable);
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

	if (pos.y - minY < 10.0f || usuba->mStateTimer > 1.0f) {
		usuba->finishMotion();
	}

	if (!usuba->isFinishMotion()) {
		f32 faceDir = usuba->getFaceDir() - 0.275f;

		clampAngle(faceDir);

		usuba->updateFaceDir(faceDir);
	}

	usuba->mStateTimer += sys->mDeltaTime;

	if (usuba->mCurAnim->mIsPlaying) {
		if (usuba->mCurAnim->mType == KEYEVENT_2) { // start falling

		} else if (usuba->mCurAnim->mType == KEYEVENT_3) { // hit ground
			usuba->createDownEffect();
			rumbleMgr->startRumble(11, pos, 2);
			enemy->getJAIObject()->startSound(PSSE_EN_FROG_LAND, 0);

		} else if (usuba->mCurAnim->mType == KEYEVENT_END) { // end fall anim
			if (usuba->mHealth <= 0.0f) {
				transit(usuba, USUBA_Dead, nullptr);
			} else {
				transit(usuba, USUBA_Ground, nullptr);
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
	Obj* usuba             = OBJ(enemy);
	usuba->mStateTimer     = 0.0f;
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->disableEvent(0, EB_Untargetable);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_Ground, nullptr);

	enemy->getJAIObject()->startSound(PSSE_EN_BOMBSARAI_STRUGGLE, 0);
}

/*
 * --INFO--
 * Address:	80270CE0
 * Size:	000044
 */
void StateGround::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	if (usuba->mHealth <= 0.0f || usuba->mStateTimer > CG_PROPERPARMS(usuba).mGroundTime.mValue) {
		usuba->finishMotion();
	}

	usuba->mStateTimer += sys->mDeltaTime;

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
	usuba->mStateTimer     = 0.0f;
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->disableEvent(0, EB_Untargetable);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_Damage, nullptr);
	// usuba->flickStickTarget();
}

/*
 * --INFO--
 * Address:	80270FD4
 * Size:	000108
 */
void StateDamage::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	if (usuba->mHealth <= 0.0f || usuba->mStateTimer > CG_PROPERPARMS(usuba).mGroundTime.mValue || usuba->getStickPikminNum() == 0) {
		usuba->finishMotion();
	}

	usuba->mStateTimer += sys->mDeltaTime;

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
	enemy->getJAIObject()->startSound(PSSE_EN_OOPAN_FLICK_2, 0);
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

	if (usuba->mHealth <= 0.0f || val > CG_PROPERPARMS(usuba).mTransitHeight.mValue) {
		usuba->finishMotion();
	}

	if (usuba->mCurAnim->mIsPlaying) {
		if (usuba->mCurAnim->mType == KEYEVENT_2) { // pop up from ground
			EnemyFunc::flickNearbyPikmin(enemy, CG_PARMS(enemy)->mGeneral.mShakeRange.mValue,
			                             CG_PARMS(enemy)->mGeneral.mShakeKnockback.mValue, CG_PARMS(enemy)->mGeneral.mShakeDamage.mValue,
			                             FLICK_BACKWARD_ANGLE, nullptr);
			EnemyFunc::flickNearbyNavi(enemy, CG_PARMS(enemy)->mGeneral.mShakeRange.mValue,
			                           CG_PARMS(enemy)->mGeneral.mShakeKnockback.mValue, CG_PARMS(enemy)->mGeneral.mShakeDamage.mValue,
			                           FLICK_BACKWARD_ANGLE, nullptr);
			EnemyFunc::flickStickPikmin(enemy, CG_PARMS(enemy)->mGeneral.mShakeChance.mValue,
			                            CG_PARMS(enemy)->mGeneral.mShakeKnockback.mValue, CG_PARMS(enemy)->mGeneral.mShakeDamage.mValue,
			                            FLICK_BACKWARD_ANGLE, nullptr);
			enemy->mFlickTimer = 0.0f;
			usuba->startBossFlickBGM();

		} else if (usuba->mCurAnim->mType == KEYEVENT_END) { // anim end
			usuba->startElec();
			int nextState = usuba->getNextStateOnHeight();
			if (nextState >= 0) {
				transit(usuba, nextState, nullptr);
				return;
			}

			transit(usuba, USUBA_Move, nullptr);
		}
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
	enemy->getJAIObject()->startSound(PSSE_EN_BOMBSARAI_FLICK, 0);
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
			EnemyFunc::flickStickPikmin(usuba, CG_PARMS(usuba)->mGeneral.mShakeChance.mValue,
			                            CG_PARMS(usuba)->mGeneral.mShakeKnockback.mValue, CG_PARMS(usuba)->mGeneral.mShakeDamage.mValue,
			                            FLICK_BACKWARD_ANGLE, nullptr);
			usuba->mFlickTimer = 0.0f;

		} else if (usuba->mCurAnim->mType == KEYEVENT_END) {
			if (usuba->mHealth <= 0.0f) {
				transit(usuba, USUBA_Fall, nullptr);
				return;
			}

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
	usuba->mStateTimer     = 0.0f;
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
	if (target || usuba->mStateTimer > CG_PROPERPARMS(usuba).mWaitTime.mValue) {
		usuba->finishMotion();
	}

	if (val > CG_PROPERPARMS(usuba).mTransitHeight.mValue || usuba->mStateTimer > 3.0f) {
		int nextState = usuba->getNextStateOnHeight();
		if (nextState >= 0) {
			transit(usuba, nextState, nullptr);
			return;
		}
	}

	usuba->mStateTimer += sys->mDeltaTime;

	if (usuba->mCurAnim->mIsPlaying && usuba->mCurAnim->mType == KEYEVENT_END) {
		if (target) {
			usuba->mTargetCreature = target;
			Vector3f targetPosition = target->getPosition();
			f32 theSqrDistance = sqrDistanceXZ(targetPosition, usuba->mPosition);
			bool isTooCloseForSwoop = theSqrDistance < SQUARE(100.0f);
			bool isTooFarForFire = theSqrDistance > SQUARE(300.0f);

			if (!isTooFarForFire && (isTooCloseForSwoop || randFloat() <= CG_PROPERPARMS(usuba).mFireBreathChance())) {
				transit(usuba, USUBA_AttackBreath, nullptr);
			}
			else {
				transit(usuba, USUBA_AttackDive, nullptr);
			}
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
	Obj* usuba         = OBJ(enemy);
	usuba->mStateTimer = 0.0f;
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

	if (target || usuba->mStateTimer > 10.0f || sqrDistanceXZ(pos, targetPos) < 625.0f) {
		usuba->mTargetVelocity = Vector3f(0.0f);
		usuba->finishMotion();
	} else {
		EnemyFunc::walkToTarget(usuba, targetPos, CG_PROPERPARMS(usuba).mFlySpeed.mValue, CG_PARMS(usuba)->mGeneral.mTurnSpeed.mValue,
		                        CG_PARMS(usuba)->mGeneral.mMaxTurnAngle.mValue);
	}

	if (height > CG_PROPERPARMS(usuba).mTransitHeight.mValue || usuba->mStateTimer > 3.0f) {
		int nextState = usuba->getNextStateOnHeight();
		if (nextState >= 0) {
			transit(usuba, nextState, nullptr);
			return;
		}
	}

	usuba->mStateTimer += sys->mDeltaTime;

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
	Obj* usuba         = OBJ(enemy);
	usuba->mStateTimer = 0.0f;
	usuba->disableEvent(0, EB_Cullable);
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_AttackBreath, nullptr);
	usuba->createChargeSE();
	usuba->startBossAttackBGM();

	mIsFireDone = false;
}

/*
 * --INFO--
 * Address:	8027190C
 * Size:	000614
 */
void StateAttackBreath::exec(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	if (usuba->mHealth <= 0.0f) {
		transit(enemy, USUBA_Fall, nullptr);
		return;
	}

	f32 frame = usuba->getMotionFrame();

	if (usuba->isFireActive()) {
		usuba->createDischargeSE();
	}

	if (!mIsFireDone && !usuba->isFireActive()) {
		if (frame <= 40.0f) {
			usuba->turnToTarget(usuba->mTargetCreature);
		}
		Vector3f targetPos = usuba->mTargetCreature->getPosition();
		if (sqrDistanceXZ(targetPos, usuba->mPosition) < SQUARE(100.0f)) {

			Vector3f distanceVec = targetPos - usuba->mPosition;
			distanceVec.y = 0.0f;
			f32 power = _normaliseXZ(distanceVec);

			enemy->mTargetVelocity = distanceVec * (-CG_PARMS(usuba)->mGeneral.mMoveSpeed());
		}
		else {
			enemy->mTargetVelocity = Vector3f(0.0f);
			
		}
	}
	else {
		enemy->mTargetVelocity = Vector3f(0.0f);
	}

	if (!enemy->mCurAnim->mIsPlaying) {
		return;
	}

	switch (enemy->mCurAnim->mType) {
	case KEYEVENT_2:
		usuba->startFireBreath();
		break;
	case KEYEVENT_3:
		usuba->createGroundFire();
		break;
	case KEYEVENT_4:
		usuba->endFireBreath();
		mIsFireDone = true;
		break;
	case KEYEVENT_END:
		if (usuba->mHealth <= 0.0f) {
			transit(enemy, USUBA_Dead, nullptr);
			return;
		}
		if (EnemyFunc::isStartFlick(usuba, false)) {
			transit(enemy, USUBA_Flick, nullptr);
			return;
		}

		transit(enemy, USUBA_Wait, nullptr);
		break;
	}
}

/*
 * --INFO--
 * Address:	80271F20
 * Size:	000044
 */
void StateAttackBreath::cleanup(EnemyBase* enemy)
{
	Obj* usuba = OBJ(enemy);
	usuba->enableEvent(0, EB_Cullable);
	usuba->disableEvent(0, EB_NoInterrupt);

	usuba->setEmotionCaution();
}

/*
 * --INFO--
 * Address:	80271894
 * Size:	000078
 */
void StateAttackDive::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* usuba         = OBJ(enemy);
	usuba->mStateTimer = 0.0f;
	usuba->disableEvent(0, EB_Cullable);
	usuba->mTargetVelocity = Vector3f(0.0f);
	usuba->enableEvent(0, EB_NoInterrupt);
	usuba->enableEvent(0, EB_Untargetable);
	usuba->setEmotionExcitement();
	usuba->startMotion(USUBAANIM_AttackDive, nullptr);
	usuba->startBossAttackBGM();

	usuba->getJAIObject()->startSound(PSSE_EN_SARAI_ATTACK, 0);
}

/*
 * --INFO--
 * Address:	8027190C
 * Size:	000614
 */
void StateAttackDive::exec(EnemyBase* enemy)
{
	Obj* usuba       = OBJ(enemy);
	Creature* target = usuba->mTargetCreature;
	if (target) {
		f32 frame = usuba->getMotionFrame();
		if (frame < 33.0f) {
			usuba->setHeightVelocity();
			usuba->changeFaceDir2(target);
		} else if (frame < 38.0f) { // after begin drop

			f32 targetHeight = target->getPosition().y + 17.5f;
			f32 usubaHeight  = usuba->getPosition().y;

			Vector3f vel = usuba->getVelocity();
			f32 factor   = ((targetHeight - usubaHeight) / sys->mDeltaTime) * CG_PROPERPARMS(usuba).mFp31();

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
			usuba->changeFaceDir2(target);
		
		} else if (frame < 55.0f) { // after hit ground
			usuba->mIsInDive = true;
			usuba->catchTarget();
		} else if (frame < 57.0f) { // after return to height
			usuba->mIsInDive = false;
			usuba->setHeightVelocity();
			f32 decayRate          = CG_PROPERPARMS(usuba).mFp32();
			usuba->mTargetVelocity = usuba->mTargetVelocity * decayRate;
		}
		else {
			usuba->mTargetVelocity = 0.0f;
		}
	} else {
		transit(usuba, USUBA_Move, nullptr);
	}

	if (usuba->mCurAnim->mIsPlaying) {
		switch (usuba->mCurAnim->mType)
		{
		case KEYEVENT_2:
			// obtuse sarai math to make it actually go towards the piki
			usuba->getJAIObject()->startSound(PSSE_EN_SARAI_BUZZ, 0);
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
			break;
		case KEYEVENT_3:
			usuba->disableEvent(0, EB_NoInterrupt);
			break;
		case KEYEVENT_END: 
			transit(usuba, USUBA_Move, nullptr);
			break;
		}
	}
}

/*
 * --INFO--
 * Address:	80271F20
 * Size:	000044
 */
void StateAttackDive::cleanup(EnemyBase* enemy)
{
	enemy->enableEvent(0, EB_Cullable);
	enemy->disableEvent(0, EB_NoInterrupt);
	enemy->mTargetCreature = nullptr;
	enemy->setEmotionCaution();
}

} // namespace Usuba
} // namespace Game
