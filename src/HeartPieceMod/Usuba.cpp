#include "types.h"
#include "Game/Entities/Usuba.h"
#include "Game/MapMgr.h"
#include "Game/EnemyFunc.h"
#include "Game/Stickers.h"
#include "Game/PikiMgr.h"
#include "efx/THebi.h"
#include "JSystem/JMath.h"
#include "DroughtMath.h"
#include "Dolphin/rand.h"
#include "PSM/EnemyBoss.h"
#include "PSSystem/PSMainSide_ObjSound.h"

/*
 * TODOS:
 *
 * steal boss music trigger functions from bosses
 * add functions for effects (appear etc)
 * add functions to turn on/off zapping and fire attack
 * potentially modify shadow functions?
 * rework/remove piki grab functions
 *
 */

namespace Game {
namespace Usuba {

void Obj::constructor()
{
	EnemyBase::constructor();
	resetBossAppearBGM();
}

void Obj::startBossAttackBGM()
{
	PSM::EnemyBoss* soundObj = static_cast<PSM::EnemyBoss*>(mSoundObj);
	PSM::checkBoss(soundObj);
	soundObj->jumpRequest(3);
}

/*
 * --INFO--
 * Address:	802951A0
 * Size:	0000C8
 */
void Obj::startBossFlickBGM()
{
	PSM::EnemyBoss* soundObj = static_cast<PSM::EnemyBoss*>(mSoundObj);
	PSM::checkBoss(soundObj);
	soundObj->jumpRequest(4);
}

/*
 * --INFO--
 * Address:	80295268
 * Size:	0000FC
 */
void Obj::updateBossBGM()
{
	PSM::EnemyBoss* soundObj = static_cast<PSM::EnemyBoss*>(mSoundObj);
	PSM::checkBoss(soundObj);
	if (mStuckPikminCount != 0) {
		soundObj->postPikiAttack(true);
	} else {
		soundObj->postPikiAttack(false);
	}
}

/*
 * --INFO--
 * Address:	80295364
 * Size:	0000D0
 */
void Obj::resetBossAppearBGM()
{
	PSM::EnemyBoss* soundObj = static_cast<PSM::EnemyBoss*>(mSoundObj);
	PSM::checkBoss(soundObj);
	soundObj->setAppearFlag(false);
}

/*
 * --INFO--
 * Address:	80295434
 * Size:	0000CC
 */
void Obj::setBossAppearBGM()
{
	PSM::EnemyBoss* soundObj = static_cast<PSM::EnemyBoss*>(mSoundObj);
	PSM::checkBoss(soundObj);
	soundObj->setAppearFlag(true);
}

/*
 * --INFO--
 * Address:	80272BC4
 * Size:	000138
 */
Obj::Obj()
{
	mAnimator = new ProperAnimator;
	setFSM(new FSM);
	createEffect();
}

/*
 * --INFO--
 * Address:	80272CFC
 * Size:	000004
 */
void Obj::setInitialSetting(EnemyInitialParamBase*) { }

/*
 * --INFO--
 * Address:	80272D00
 * Size:	000078
 */
void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);

	enableEvent(0, EB_Untargetable);
	enableEvent(0, EB_ModelHidden);

	mStateTimer      = 0.0f;
	mIsBreathingFire = false;
	mIsElecBody      = false;
	resetAttackableTimer(12800.0f);

	setupEffect();

	mFsm->start(this, USUBA_Stay, nullptr);
	resetBossAppearBGM();
}

/*
 * --INFO--
 * Address:	80272D78
 * Size:	000048
 */
void Obj::doUpdate()
{
	mFsm->exec(this);

	attackTargets();

	if (mIsFirePoolActive) {
		mGroundedFireTimer -= sys->mDeltaTime;
		if (mGroundedFireTimer < 0.0f) {
			mIsFirePoolActive = false;
			fadeFireHitGroundEffect();
		}
	}

	// Drought Here: Don't do this lmao
	// OSReport("Current state: %i\n", getStateID());
}

void Obj::doUpdateCommon()
{
	EnemyBase::doUpdateCommon();
	updateBossBGM();
}

/*
 * --INFO--
 * Address:	80272DC0
 * Size:	00004C
 */
void Obj::setFSM(FSM* fsm)
{
	mFsm = fsm;
	mFsm->init(this);
	mCurrentLifecycleState = nullptr;
}

/*
 * --INFO--
 * Address:	80272E0C
 * Size:	000004
 */
void Obj::doDirectDraw(Graphics&) { }

/*
 * --INFO--
 * Address:	80272E10
 * Size:	000020
 */
void Obj::doDebugDraw(Graphics& gfx) { EnemyBase::doDebugDraw(gfx); }

void Obj::collisionCallback(CollEvent& event)
{
	EnemyBase::collisionCallback(event);
	Creature* collCreature = event.mCollidingCreature;
	if (isElecBody() && collCreature) {
		InteractDenki denki(this, C_PARMS->mGeneral.mAttackDamage(), &Vector3f::zero);
		collCreature->stimulate(denki);
	}
}

/*
 * --INFO--
 * Address:	80272E30
 * Size:	000140
 */
void Obj::getShadowParam(ShadowParam& shadowParam)
{
	Matrixf* bodyMtx      = mModel->getJoint("body")->getWorldMatrix();
	shadowParam.mPosition = bodyMtx->getColumn(3);

	if (isAlive()) {
		int stateId = getStateID();

		// grounded states
		if (stateId == USUBA_Appear || stateId == USUBA_Fall || stateId == USUBA_Ground || stateId == USUBA_Damage
		    || stateId == USUBA_Recover) {
			shadowParam.mPosition.y -= 5.0f;
			shadowParam.mBoundingSphere.mRadius = 100.0f + static_cast<Parms*>(mParms)->mProperParms.mFlightHeight.mValue;
		} else if (mBounceTriangle) {
			shadowParam.mPosition.y -= 5.0f;
			shadowParam.mBoundingSphere.mRadius = 50.0f;
		} else {
			shadowParam.mPosition.y -= 20.0f;
			shadowParam.mBoundingSphere.mRadius = 100.0f + static_cast<Parms*>(mParms)->mProperParms.mFlightHeight.mValue;
		}
	} else {
		shadowParam.mPosition.y             = 2.5f + mPosition.y;
		shadowParam.mBoundingSphere.mRadius = 25.0f;
	}

	shadowParam.mBoundingSphere.mPosition = Vector3f(0.0f, 1.0f, 0.0f);
	shadowParam.mSize                     = 12.5f;
}

/*
 * --INFO--
 * Address:	80272F70
 * Size:	000034
 */
void Obj::doStartStoneState() { EnemyBase::doStartStoneState(); }

/*
 * --INFO--
 * Address:	80272FA4
 * Size:	00005C
 */
void Obj::doFinishStoneState()
{
	EnemyBase::doFinishStoneState();

	int stateId = getStateID();
	if (stateId >= USUBA_Recover) { // if previously airborne
		mFsm->transit(this, USUBA_Recover, nullptr);
	}
}

/*
 * --INFO--
 * Address:	80273000
 * Size:	000028
 */
void Obj::startCarcassMotion() { EnemyBase::startMotion(USUBAANIM_Carry, nullptr); }

/*
 * --INFO--
 * Address:	802730D0
 * Size:	000108
 */
f32 Obj::setHeightVelocity()
{
	// The maximum amount of Pikmin that can have an effect on the upward velocity of the Usuba
	// Calculate the weight factor based on Pikmin stuck
	int pikminWeightFactor
	    = (mStuckPikminCount < 0) ? (0) : (mStuckPikminCount <= USUBA_MAX_STICK_PIKI ? (mStuckPikminCount) : (USUBA_MAX_STICK_PIKI));

	f32 riseFactor     = static_cast<Parms*>(mParms)->mProperParms.mUnladenClimbFactor.mValue;
	f32 climbingFactor = static_cast<Parms*>(mParms)->mProperParms.mMaxLadenClimbFactor.mValue;
	f32 weight         = pikminWeightFactor;

	// lerp v0, v1, t -> (1 - t) * v0 + t * v1
	f32 velFactor
	    = (((USUBA_MAX_STICK_PIKI - weight) / USUBA_MAX_STICK_PIKI) * riseFactor) + (weight / USUBA_MAX_STICK_PIKI) * climbingFactor;

	// Get the Y position of the map model (equivalent to a downwards raycast)
	f32 mapPosY = mapMgr->getMinY(mPosition);

	// Get intended flight height
	f32 flightHeight = static_cast<Parms*>(mParms)->mProperParms.mFlightHeight.mValue; // Normal flight height

	// Upward velocity is offset by map height
	mCurrentVelocity.y = velFactor * ((mapPosY + flightHeight) - mPosition.y);

	return mPosition.y - mapPosY;
}

/*
 * --INFO--
 * Address:	802731D8
 * Size:	000230
 */
void Obj::setRandTarget()
{
	// Sets a random target near the home radius, if in a cave then completely random
	f32 radius;
	if (gameSystem && gameSystem->mIsInCave) {
		radius = 50.0f + randWeightFloat(50.0f);
	} else {
		radius = static_cast<Parms*>(mParms)->mGeneral.mHomeRadius.mValue
		       + randWeightFloat(static_cast<Parms*>(mParms)->mGeneral.mTerritoryRadius.mValue
		                         - static_cast<Parms*>(mParms)->mGeneral.mHomeRadius.mValue);
	}

	// Get the direction from the home position towards our position
	f32 dirToSarai = JMAAtan2Radian(mPosition.x - mHomePosition.x, mPosition.z - mHomePosition.z);

	// Randomise the angle a bit and set the target position
	f32 rngAngle = HALF_PI + (dirToSarai + randWeightFloat(PI));
	mTargetPos   = Vector3f((radius * pikmin2_sinf(rngAngle)) + mHomePosition.x, mHomePosition.y,
                          (radius * pikmin2_cosf(rngAngle)) + mHomePosition.z);
}

/*
 * --INFO--
 * Address:	80273718
 * Size:	000144
 * TODO
 */
int Obj::getNextStateOnHeight()
{
	if (mHealth <= 0.0f) {
		return USUBA_Fall;
	}

	int stuckPiki = getStickPikminNum();
	if (stuckPiki) {
		int cappedPikiCount;
		if (stuckPiki < 1) {
			cappedPikiCount = 0;
		} else {
			cappedPikiCount = (USUBA_MAX_STICK_PIKI - 1);
			if (stuckPiki - 1 <= (USUBA_MAX_STICK_PIKI - 1)) {
				cappedPikiCount = stuckPiki - 1;
			}
		}

		f32 minFallChance = static_cast<Parms*>(mParms)->mProperParms.mFallChanceMinPiki.mValue;
		f32 maxFallChance = static_cast<Parms*>(mParms)->mProperParms.mFallChanceMaxPiki.mValue;

		f32 invPikiWeight = ((f32)(USUBA_MAX_STICK_PIKI - 1) - cappedPikiCount) / (USUBA_MAX_STICK_PIKI - 1);
		f32 pikiWeight    = (cappedPikiCount / (USUBA_MAX_STICK_PIKI - 1));

		f32 fallChance = (invPikiWeight * minFallChance) + (pikiWeight * maxFallChance);

		if (randWeightFloat(1.0f) < fallChance) {
			return USUBA_Flick;
		} else {
			return USUBA_Fall;
		}
	}

	return USUBA_NULL;
}

/*
 * --INFO--
 * Address:	8027390C
 * Size:	000080
 */
int Obj::getStickPikminNum() { return mStuckPikminCount; }

/*
 * --INFO--
 * Address:	8027398C
 * Size:	0003D0
 */
FakePiki* Obj::getAttackableTarget()
{
	if (sqrDistanceXZ(mPosition, mHomePosition) < SQUARE(C_PARMS->mGeneral.mTerritoryRadius())) {
		f32 maxAngle = PI * (DEG2RAD * C_PARMS->mGeneral.mViewAngle());
		f32 maxDist  = SQUARE(C_PARMS->mGeneral.mSightRadius());

		Iterator<Piki> iterator(pikiMgr);
		CI_LOOP(iterator)
		{
			Piki* piki = *iterator;

			if (piki->isAlive() && piki->isPikmin() && !piki->isStickToMouth() && piki->mSticker != this && piki->mBounceTriangle) {
				f32 ang = getAngDist(piki);
				if (FABS(ang) <= maxAngle) {
					Vector3f pos = piki->getPosition();
					if (sqrDistanceXZ(mPosition, pos) < maxDist) {
						return piki;
					}
				}
			}
		}
	}

	return nullptr;
}

/*
 * --INFO--
 * Address:	80273D5C
 * Size:	000024
 */
int Obj::catchTarget() { EnemyFunc::eatPikmin(this, nullptr); }

/*
 * --INFO--
 * Address:	80273D80
 * Size:	000044
 */
void Obj::createDownEffect() { EnemyBase::createBounceEffect(mPosition, getDownSmokeScale()); }

bool Obj::attackTargets()
{
	if (mIsBreathingFire) {

		Vector3f lineEnd   = mFireGroundHitPos;
		Vector3f lineStart = mModel->getJoint("root")->getWorldMatrix()->getTranslation();

		Sys::Sphere sphereStart(lineStart, 0.0f);
		Sys::Sphere sphereEnd(lineEnd, 50.0f);

		Sys::Sphere sphere = DroughtMath::makeBoundingSphere(sphereStart, sphereEnd);

		CellIteratorArg ciArg = sphere;
		CellIterator iCell    = ciArg;

		CI_LOOP(iCell)
		{
			Creature* creature = static_cast<Creature*>(*iCell);

			Vector3f creaturePos = creature->getPosition();

			f32 lineprogress;

			if (DroughtMath::getSqrDistanceToLine(creaturePos, lineStart, lineEnd, lineprogress) < SQUARE(50.0f) && lineprogress > 0.2f) {
				InteractFire fire(this, C_PARMS->mGeneral.mAttackDamage());
				creature->stimulate(fire);
			}
		}
	} // this sphere is within the fire-breath line, hence the else
	else if (mIsFirePoolActive) {
		Sys::Sphere fireball(mFireGroundHitPos, 50.0f);
		CellIteratorArg ciArg2 = fireball;
		CellIterator iCell2    = ciArg2;
		CI_LOOP(iCell2)
		{
			Creature* creature = static_cast<Creature*>(*iCell2);
			InteractFire fire(this, C_PARMS->mGeneral.mAttackDamage());
			creature->stimulate(fire);
		}
	}

	return true;
}

void Obj::createChargeSE() { getJAIObject()->startSound(PSSE_EN_TANK_BREATH, 0); }

void Obj::createDischargeSE() { getJAIObject()->startSound(PSSE_EN_TANK_FIRE, 0); }

void Obj::createAppearEffect()
{
	efx::Arg fxArg(mPosition);
	efx::THebiAphd_appear1 appear1;
	appear1.create(&fxArg);
}

void Obj::setupEffect()
{
	// efx::TUsubaEffect* effect;
	// Matrixf* mtx = mModel->getJoint("body")->getWorldMatrix();
	// effect       = mFireEfx;

	// effect->mEfxFire.mEfxABC.setMtxptr(mtx->mMatrix.mtxView);
	// effect->mEfxFire.mEfxIND.mMtx = mtx;
	// effect->mEfxFireYodare.mMtx   = mtx;

	for (int i = 0; i < mModel->mJointCount; i++) {
		OSReport("Joint %i %s\n", i, mModel->mJoints[i].mName);
	}

	mFireEfx->mMtx = mModel->getJoint("root")->getWorldMatrix();

	mFireflyEfx->mMtx = mModel->getJoint("root")->getWorldMatrix();
}

void Obj::startFirefly() { mFireflyEfx->create(nullptr); }

void Obj::fadeFirefly() { mFireflyEfx->fade(); }

void Obj::createEffect()
{
	mFireEfx       = new efx::TUsubaFireNew;
	mFireflyEfx    = new efx::TUsubaFirefly;
	mFireGroundEfx = new efx::TUsubaFireGround;
} // mFireEfx = new efx::TUsubaEffect(nullptr); }

void Obj::createGroundFire()
{
	mGroundedFireTimer = C_PROPERPARMS.mFirePoolLingerTime();
	mIsFirePoolActive  = true;

	f32 faceDir = getFaceDir();

	mFireGroundHitPos = mPosition;

	f32 spawnDistance = C_PROPERPARMS.mFirePoolSpawnDistance();

	mFireGroundHitPos.x += pikmin2_sinf(faceDir) * spawnDistance;
	mFireGroundHitPos.z += pikmin2_cosf(faceDir) * spawnDistance;

	mFireGroundHitPos.y = mapMgr->getMinY(mFireGroundHitPos);

	createFireHitGroundEffect();
}

void Obj::startFireBreath()
{
	mIsBreathingFire = true;
	createFireEffect();
	createDischargeSE();
}

void Obj::endFireBreath()
{
	mIsBreathingFire = false;
	fadeFireEffect();
}

void Obj::createFireEffect() { mFireEfx->create(nullptr); }

void Obj::createFireHitGroundEffect()
{
	efx::Arg arg(mFireGroundHitPos);
	mFireGroundEfx->create(&arg);
}

void Obj::fadeFireEffect() { mFireEfx->fade(); }

void Obj::fadeFireHitGroundEffect() { mFireGroundEfx->fade(); }

void Obj::startElec()
{
	mIsElecBody = true;
	startFirefly();
}

void Obj::endElec()
{
	mIsElecBody = false;
	fadeFirefly();
}

} // namespace Usuba
} // namespace Game
