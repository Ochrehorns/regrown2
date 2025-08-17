#include "Game/Entities/HallowMushi.h"
#include "Game/generalEnemyMgr.h"
#include "Dolphin/rand.h"
#include "Game/Navi.h"
#include "Game/Stickers.h"
#include "types.h"

#include "Game/EnemyAnimKeyEvent.h"
#include "Game/EnemyFunc.h"
#include "Game/CameraMgr.h"
#include "Game/rumble.h"
#include "nans.h"

namespace Game {

namespace HallowMushi {

Obj::Obj() { }

void Obj::createEffect()
{
	for (int i = 0; i < mTrailCount; i++) {
		mTrailArray[i].mEffect = new efx::THallow;
		mTrailArray[i].mActive = false;
	}

	mClawEffectBolts       = new efx::TElecClawBolts;
	mClawEffectBolts->mMtx = mModel->getJoint("hand_R")->getWorldMatrix();
	mClawEffectBolts->create(nullptr);
	mClawEffectGlow       = new efx::TElecClawGlow;
	mClawEffectGlow->mMtx = mModel->getJoint("hand_R")->getWorldMatrix();
	mClawEffectGlow->create(nullptr);
}

void Obj::onInit(CreatureInitArg* arg)
{
	DangoMushi::Obj::onInit(arg);

	mTrailCount = C_PARMS->mProperHallowParms.mTrailLingerTime() / C_PARMS->mProperHallowParms.mTrailPeriod();
	mTrailArray = new Trail[mTrailCount];
	createEffect();
}

void Trail::create(Vector3f& position, f32 scale, f32 time)
{
	mPosition = position;
	mTimer    = time;
	mActive   = true;
	efx::ArgScaleTime arg(mPosition, scale, time);
	mEffect->create(&arg);
}

void Trail::fade()
{
	mActive = false;
	mEffect->fade();
}

void Trail::update(EnemyBase* parent)
{
	mTimer -= sys->mDeltaTime;
	if (mTimer < 0.0f) {
		fade();
		return;
	}

	Sys::Sphere sphere(mPosition, CG_PARMS(parent)->mProperHallowParms.mTrailRadiusSize());

	CellIteratorArg arg(sphere);
	CellIterator iCell = arg;

	CI_LOOP(iCell)
	{
		CellObject* obj = *iCell;
		if (FABS(obj->getPosition().y - mPosition.y) > CG_PARMS(parent)->mProperHallowParms.mTrailHeight()) {
			continue;
		}

		if (obj->isPiki()) {
			Piki* piki = static_cast<Piki*>(obj);
			InteractGas gas(parent, 0.0f);
			piki->stimulate(gas);
		}
		// else if (obj->isNavi()) {
		// 	Navi* navi = static_cast<Navi*>(obj);
		// 	InteractBubble gas(parent, 5.0f);
		// 	navi->stimulate(gas);
		// }
	}
}

void Obj::addTrail(Vector3f& pos)
{
	// if there is an open trail, take it
	for (int i = 0; i < mTrailCount; i++) {
		if (!mTrailArray[i].mActive) {
			mTrailArray[i].create(pos, C_PARMS->mProperHallowParms.mTrailRadiusSize(), C_PARMS->mProperHallowParms.mTrailLingerTime());
			return;
		}
	}

	// else, find the closest trail to done and steal that
	f32 minTime                     = mTrailArray[0].mTimer;
	HallowMushi::Trail* targetTrail = &mTrailArray[0];

	for (int i = 1; i < mTrailCount; i++) {
		if (minTime < mTrailArray[i].mTimer) {
			targetTrail = &mTrailArray[i];
			minTime     = mTrailArray[i].mTimer;
		}
	}

	targetTrail->fade();
	targetTrail->create(pos, C_PARMS->mProperHallowParms.mTrailRadiusSize(), C_PARMS->mProperHallowParms.mTrailLingerTime());
}

bool Obj::bombCallBack(Creature* source, Vector3f& direction, f32 damage)
{
	if (source->isTeki() && static_cast<EnemyBase*>(source)->getEnemyTypeID() == EnemyTypeID::EnemyID_Bomb) {
		return false;
	}
	return DangoMushi::Obj::bombCallBack(source, direction, damage);
}

#define HALLOW_FALLING_BOMB_COUNT    (5)
#define HALLOW_FALLING_DWEEVIL_COUNT (1)

void Obj::createCrashEnemy()
{
	OSReport("Obj::createCrashEnemy()\n");

	int fallEnemyTypes[]  = { EnemyTypeID::EnemyID_Bomb, EnemyTypeID::EnemyID_BombOtakara };
	int fallEnemyCounts[] = { HALLOW_FALLING_BOMB_COUNT, HALLOW_FALLING_DWEEVIL_COUNT };

	// set number of falling eggs to either 1 or 0
	fallEnemyCounts[DangoMushi::DANGOFALL_Egg] = getFallEggNum();

	for (int i = 0; i < (int)ARRAY_SIZE(fallEnemyTypes); i++) {
		EnemyMgrBase* mgr = generalEnemyMgr->getEnemyMgr(fallEnemyTypes[i]);
		if (!mgr) {
			continue;
		}

		Vector3f fallPos = getFallPosition(i);

		f32 angleOffset1 = randWeightFloat(PI);
		f32 angleOffset2 = 0.5f + angleOffset1;
		f32 angleOffset3 = 0.25f + angleOffset1;
		f32 birthAngle   = 0.0f;
		f32 dist;

		for (int j = 0; j < fallEnemyCounts[i]; j++) {
			if (j == 0) {
				// first falling enemy goes at (fallPos.x, fallPos.y, dist + fallPos.z)
				dist = randWeightFloat(15.0f);

			} else if (j < 4) {
				// enemies (rocks) 2, 3, 4 go vaguely equally spaced around a circle, with some small randomness...
				birthAngle = (2 * PI / 3) * (f32)j + (angleOffset1 + randWeightFloat(1.0f));

				// ... at some distance between 70 and 85 units from fall position
				dist = 70.0f + randWeightFloat(15.0f);

			} else if (j < 10) {
				// enemies (rocks) 5, 6, 7, 8, 9, 10 go vaguely equally spaced around a circle, with some small(er) randomness...
				birthAngle = (PI / 3) * (f32)j + (angleOffset2 + randWeightFloat(0.5f));

				// ... at some distance between 140 and 155 units from fall position
				dist = 140.0f + randWeightFloat(15.0f);

			} else {
				// any additional enemies (rocks) keep going around a circle with smaller spacing (unused, max is 10 rocks)
				birthAngle = (PI / 6) * (f32)j + (angleOffset3 + randWeightFloat(0.1f));

				// they'd also go at a larger distance from fall position (220-235 units)
				dist = 220.0f + randWeightFloat(15.0f);
			}

			if (birthAngle > TAU) {
				birthAngle -= TAU;
			}

			EnemyBirthArg birthArg;
			birthArg.mTypeID        = (EnemyTypeID::EEnemyTypeID)fallEnemyTypes[i];
			birthArg.mFaceDir       = mFaceDir;
			birthArg.mPosition.x    = dist * sinf(birthAngle) + fallPos.x;
			birthArg.mPosition.y    = fallPos.y;
			birthArg.mPosition.z    = dist * cosf(birthAngle) + fallPos.z;
			birthArg.mTekiBirthType = EDG_AUTO;

			EnemyBase* enemy = mgr->birth(birthArg);
			if (enemy) {
				enemy->init(nullptr);
				enemy->disableEvent(0, EB_Cullable); // rocks/eggs are always loaded to go off, even if you're not watching

				// majorly increase trigger distance (for rocks, these are 75 and 150 by default; for eggs, 30 and 700)
				C_PARMS->mGeneral.mPrivateRadius() = 1000.0f;
				C_PARMS->mGeneral.mSightRadius()   = 1000.0f;
			}
		}
	}
}

void Obj::flickHandCollision(Creature* target)
{
	Vector3f targetPos = target->getPosition();
	if (target->isNavi()) {
		targetPos -= mPosition;
		targetPos.y = 0.0f;
		targetPos.normalise();
		targetPos.y = 1.0f;
		targetPos *= 300.0f;

		InteractDenki denki(this, C_PARMS->mGeneral.mAttackDamage(), &targetPos);
		target->stimulate(denki);
		return;
	}

	if (target->isPiki()) {

		targetPos -= mPosition;
		targetPos.y = 0.0f;
		targetPos.normalise();
		targetPos.y = 1.0f;
		targetPos *= 300.0f;

		if (static_cast<Piki*>(target)->getKind() == Yellow) {
			InteractHanaChirashi wither(this, C_PARMS->mGeneral.mAttackDamage(), &targetPos);
			target->stimulate(wither);
		} else {
			InteractDenki denki(this, C_PARMS->mGeneral.mAttackDamage(), &targetPos);
			target->stimulate(denki);
		}
	}
}

void Obj::setBodyCollision(bool check)
{
	// these are the attackable parts
	u32 collTags[2] = { 'bod0', 'bod1' };

	for (int i = 0; i < 2; i++) {
		CollPart* part = mCollTree->getCollPart(collTags[i]);
		if (part && check) {
			part->mSpecialID = '_t__';
		} else {
			part->mSpecialID = 'st__';
		}
	}

	if (check && mStuckPikminCount != 0 && !(mHealth <= 0.0f)) {
		f32 angle = TAU + mFaceDir;
		Vector3f effectPos;
		effectPos.x = 150.0f * sinf(mFaceDir + PI);
		effectPos.y = 150.0f;
		effectPos.z = 150.0f * cosf(mFaceDir + PI);

		Stickers stickers(this);
		Iterator<Creature> iter(&stickers);
		CI_LOOP(iter)
		{
			Creature* stuck = *iter;
			if (stuck->isPiki()) {
				if (static_cast<Piki*>(stuck)->getKind() == Purple) {
					InteractFlick flick(this, C_PARMS->mGeneral.mShakeKnockback(), C_PARMS->mGeneral.mShakeDamage(), angle);
					stuck->stimulate(flick);
				} else {
					InteractHanaChirashi wilt(this, C_PARMS->mGeneral.mAttackDamage(), &effectPos);
					stuck->stimulate(wilt);
				}
			}
		}
	}
}

void Obj::updateTrails()
{
	for (int i = 0; i < mTrailCount; i++) {
		if (mTrailArray[i].mActive) {
			mTrailArray[i].update(this);
		}
	}
}

void Obj::doUpdate()
{
	DangoMushi::Obj::doUpdate();
	updateTrails();

	if (mIsRolling) {
		mLastTrailTimer -= sys->mDeltaTime;
		if (mLastTrailTimer < 0.0f) {
			mLastTrailTimer = C_PARMS->mProperHallowParms.mTrailPeriod();
			Vector3f pos    = getPosition();
			addTrail(pos);
		}
	} else {
		mLastTrailTimer = C_PARMS->mProperHallowParms.mFirstTrailDelay();
	}

	// When the Hallowed Crawbster is in these states, an idle electricity sound will play.
	if (getStateID() == 3 || getStateID() == 4 || getStateID() == 6 || getStateID() == 7 || getStateID() == 8) {
		getJAIObject()->startSound(0x5196, 0);
	}

	// Extra rolling sound layer
	if (getStateID() == 5) {
		getJAIObject()->startSound(0x5197, 0);
	}

	// this will clean up the electricity particles when it dies
	if (getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		HallowMushi::Obj* hallow = static_cast<HallowMushi::Obj*>(this);
		if (getStateID() == 0) {
			hallow->mClawEffectBolts->fade();
			hallow->mClawEffectGlow->fade();;
		}
	}

}

} // namespace HallowMushi

namespace DangoMushi {
// createCrashEnemyBase__Q34Game10DangoMushi3ObjFv
void Obj::createCrashEnemyBase()
{
	if (getEnemyTypeID() == EnemyTypeID::EnemyID_DangoMushi) {
		DangoMushi::Obj::createCrashEnemy();
	} else if (getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		static_cast<HallowMushi::Obj*>(this)->HallowMushi::Obj::createCrashEnemy();
	}
}

void Obj::flickHandCollisionBase(Creature* arg)
{
	if (getEnemyTypeID() == EnemyTypeID::EnemyID_DangoMushi) {
		DangoMushi::Obj::flickHandCollision(arg);
	} else if (getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		static_cast<HallowMushi::Obj*>(this)->HallowMushi::Obj::flickHandCollision(arg);
	}
}

void Obj::setBodyCollisionBase(bool arg)
{
	if (getEnemyTypeID() == EnemyTypeID::EnemyID_DangoMushi) {
		DangoMushi::Obj::setBodyCollision(arg);
	} else if (getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		static_cast<HallowMushi::Obj*>(this)->HallowMushi::Obj::setBodyCollision(arg);
	}
}

void StateFlick::init(EnemyBase* enemy, StateArg* stateArg)
{
	Obj* crab            = OBJ(enemy);
	crab->mNextState     = DANGOMUSHI_NULL;
	crab->mStateTimer    = 0.0f;
	crab->mIsArmSwinging = false;
	crab->mIsBall        = false;
	crab->setEmotionExcitement();
	crab->mTargetVelocity = Vector3f(0.0f);
	crab->startBlendAnimation(DANGOANIM_Attack2, false);

	if (crab->getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		HallowMushi::Obj* hallow = static_cast<HallowMushi::Obj*>(crab);
		hallow->mClawEffectGlow->create(nullptr);
	}
}

/**
 * @note Address: 0x802FBAE8
 * @note Size: 0x1E0
 */
void StateFlick::exec(EnemyBase* enemy)
{
	Obj* crab = OBJ(enemy);
	if (crab->flickHandCollision()) {
		crab->createWallBreakEffect();
		crab->getJAIObject()->startSound(PSSE_EN_DANGO_ARM_GROUND, 0);
		if (crab->getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
			HallowMushi::Obj* hallow = static_cast<HallowMushi::Obj*>(crab);
			hallow->mClawEffectGlow->create(nullptr);
			crab->getJAIObject()->startSound(0x5998, 0);
		}	
		Vector3f crabPos = crab->getPosition();
		cameraMgr->startVibration(25, crabPos, 2);
		rumbleMgr->startRumble(14, crabPos, 2);
		transit(crab, DANGOMUSHI_Wait, (DangoStateArg*)("blend"));
		return;
	}

	if (crab->mCurAnim->mIsPlaying) {
		if (crab->mCurAnim->mType == KEYEVENT_END_BLEND) {
			crab->endBlendAnimation();

		} else if (crab->mCurAnim->mType == KEYEVENT_2) {
			crab->mIsArmSwinging = true;
			if (crab->getMotionFrame() < 30.0f) {
				crab->createFlickAttackEffect();
			}

		} else if (crab->mCurAnim->mType == KEYEVENT_3) {
			crab->mIsArmSwinging = false;
			Vector3f crabPos     = crab->getPosition();
			cameraMgr->startVibration(15, crabPos, 2);
			rumbleMgr->startRumble(11, crabPos, 2);

		} else if (crab->mCurAnim->mType == KEYEVENT_END) {
			transit(crab, DANGOMUSHI_Wait, nullptr);
		}
	}
}

/**
 * @note Address: 0x802FBCC8
 * @note Size: 0x2C
 */
void StateFlick::cleanup(EnemyBase* enemy)
{
	Obj* crab            = OBJ(enemy);
	crab->mIsArmSwinging = false;
	crab->setEmotionCaution();
	if (crab->getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		HallowMushi::Obj* hallow = static_cast<HallowMushi::Obj*>(crab);
		// hallow->mClawEffectGlow->fade();
	}
}

void Obj::startRollingMoveEffect()
{
	mEfxRun->create(nullptr);
	if (getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		HallowMushi::Obj* hallow = static_cast<HallowMushi::Obj*>(this);
		hallow->mClawEffectBolts->fade();
		hallow->mClawEffectGlow->fade();
	}
}

/**
 * @note Address: 0x802FF244
 * @note Size: 0x30
 */
void Obj::finishRollingMoveEffect()
{
	mEfxRun->fade();
	if (getEnemyTypeID() == EnemyTypeID::EnemyID_HallowMushi) {
		HallowMushi::Obj* hallow = static_cast<HallowMushi::Obj*>(this);
		hallow->mClawEffectBolts->create(nullptr);
		hallow->mClawEffectGlow->create(nullptr);
	}
}

} // namespace DangoMushi

} // namespace Game
