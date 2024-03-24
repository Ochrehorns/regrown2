#include "Game/Entities/SmokyFrog.h"
#include "Game/EnemyFunc.h"
#include "Game/CameraMgr.h"
#include "Game/rumble.h"
#include "Game/Navi.h"
#include "Game/MapMgr.h"
#include "efx/TFrog.h"
#include "efx/TSmokyFrog.h"
#include "PS.h"
#include "LoadResource.h"
#include "PSM/EnemyBoss.h"
#include "PSSystem/PSMainSide_ObjSound.h"

#define GAS_CLOUD_MODIFIER (1.0f)

namespace Game {
namespace SmokyFrog {

Obj::Obj()
{
	createEffect();
	setFSM(new FSM);
	mMatLoopAnimator = new Sys::MatLoopAnimator;
}

void Obj::constructor()
{
	EnemyBase::constructor();
	resetBossAppearBGM();
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

void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);
	mAlertTimer  = 128.0f;
	mAirTimer    = 0.0f;
	mAttackTimer = 128.0f;
	mShadowScale = 0.0f;
	mNextState   = Frog::FROG_NULL;
	mIsInAir     = false;
	mIsFalling   = false;
	setupEffect();
	shadowMgr->delShadow(this);
	if (gameSystem && gameSystem->isZukanMode()) {
		mFsm->start(this, SMOKYFROG_Appear, nullptr);
	} else {
		mFsm->start(this, SMOKYFROG_Stay, nullptr);
	}
	resetBossAppearBGM();
	mMatLoopAnimator->start(C_MGR->mTexAnimation);
}

void Obj::setParameters()
{
	EnemyBase::setParameters();

	setScale(static_cast<Frog::Parms*>(mParms)->mProperParms.mScaleMult.mValue); // don't assign parm shit in the ctor
	// mCurLodSphere.mRadius = mScaleModifier * static_cast<Frog::Parms*>(mParms)->mGeneral.mOffCameraRadius.mValue;
	// update collision
	Sys::Sphere collSphere;
	mCollTree->mPart->getSphere(collSphere);
	collSphere.mRadius *= mScaleModifier;
	mBoundingSphere = collSphere;
	mCollTree->mPart->setScale(mScaleModifier);
	mCollTree->update();
}

void Obj::getShadowParam(ShadowParam& param)
{
	Sys::Sphere boundingSphere;
	getBoundingSphere(boundingSphere);
	param.mPosition.x = boundingSphere.mPosition.x;
	param.mPosition.y = 5.0f + mPosition.y;
	param.mPosition.z = boundingSphere.mPosition.z;

	if (isAlive()) {
		if (mBounceTriangle) {
			if (isEvent(1, EB2_Earthquake)) {
				param.mBoundingSphere.mRadius = 75.0f;
			} else {
				param.mBoundingSphere.mRadius = 50.0f;
			}
		} else {
			if (getStateID() == Frog::FROG_JumpWait) {
				param.mPosition.y -= 17.5f;
			} else if (getStateID() == SMOKYFROG_Stay || SMOKYFROG_Appear) {
				// set shadow position to min ground level, the + 0.5f is to prevent z-fighting
				param.mPosition.y = mapMgr->getMinY(param.mPosition) + 0.5f;
			}

			param.mBoundingSphere.mRadius = 0.75f * static_cast<Frog::Parms*>(mParms)->mProperParms.mJumpSpeed.mValue;
		}
	} else {
		param.mBoundingSphere.mRadius = 22.5f;
	}

	param.mBoundingSphere.mPosition = Vector3f(0.0f, 1.0f, 0.0f);
	param.mSize                     = 17.5f * static_cast<Frog::Parms*>(mParms)->mProperParms.mScaleMult.mValue * mShadowScale;
}

void Obj::getLODCylinder(Sys::Cylinder& cylinder)
{
	// have to do some really dumb stuff to keep the cylinder visible on the ground here (sourced from your local pikhacker)
	cylinder.mCenter   = mCurLodSphere.mPosition;
	cylinder.mCenter.y = mapMgr->getMinY(cylinder.mCenter) + (SMOKYFROG_START_HEIGHT * 0.5f);

	cylinder.mRadius = mCurLodSphere.mRadius;
	cylinder.mLength = SMOKYFROG_START_HEIGHT;

	cylinder.mAxis = Vector3f(0.0f, 1.0f, 0.0f);
}

bool Obj::needShadow()
{
	if (EnemyBase::needShadow())
		return true;

	if (mShadowScale > 0.0f) {
		int state = getStateID();
		if (state == SMOKYFROG_Stay || state == SMOKYFROG_Appear) {
			return true;
		}
	}

	return false;
}

void Obj::doUpdateCommon()
{
	EnemyBase::doUpdateCommon();
	updateBossBGM();
	if (mAttackTimer < 1.0f) {
		mAttackTimer += sys->mDeltaTime;
		interactGasAttack();
	}
}

void Obj::attackNaviPosition()
{
	Iterator<Navi> iter(naviMgr);

	CI_LOOP(iter)
	{
		Navi* navi = (*iter);
		if (navi->isAlive()) {
			f32 minAttackRange = static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMaxAttackAngle.mValue;
			f32 maxAttackRange = static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMaxAttackRange.mValue;

			f32 angledist = getCreatureViewAngle(navi);

			if (isTargetAttackable(navi, angledist, maxAttackRange, minAttackRange)) {
				mTargetPosition = Vector3f(navi->getPosition());
			}
		}
	}
}

void Obj::createEffect() { mEfxPota = new efx::TFrogPota; }

void Obj::createGas()
{
	mAttackTimer = 0.0f;
	efx::ArgScale fxArg(mPosition, GAS_CLOUD_MODIFIER * static_cast<Frog::Parms*>(mParms)->mProperParms.mScaleMult.mValue);
	efx::TSmokyFrog gasFX;
	gasFX.create(&fxArg);
	getJAIObject()->startSound(PSSE_EN_OTAKARA_ATK_GAS, 0);
}

void Obj::interactGasAttack()
{
	f32 maxRange = mPosition.y + static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMaxAttackRange.mValue;
	f32 minRange = mPosition.y - static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMaxAttackAngle.mValue;
	f32 radius   = static_cast<Frog::Parms*>(mParms)->mProperParms.mScaleMult.mValue * 50.0f;

	f32 dist = SQUARE(radius);

	Sys::Sphere sphere;
	sphere.mPosition = mPosition;
	sphere.mRadius   = radius;

	CellIteratorArg iterArg(sphere);
	iterArg.mOptimise = true;
	CellIterator iter(iterArg);

	CI_LOOP(iter)
	{
		Creature* target = static_cast<Creature*>(*iter);
		if (target->isAlive() && (target->isNavi() || target->isPiki())) {
			Vector3f targetPos = target->getPosition();
			if (maxRange > targetPos.y && minRange < targetPos.y && sqrDistanceXZ(mPosition, targetPos) < dist) {
				InteractGas gas(this, static_cast<EnemyParmsBase*>(mParms)->mGeneral.mAttackDamage.mValue);
				target->stimulate(gas);
			}
		}
	}
}

bool Obj::addShadowScale()
{
	if (mShadowScale < 1.0f) {
		mShadowScale += 0.6f * sys->mDeltaTime;
		if (mShadowScale >= 1.0f) {
			mShadowScale = 1.0f;
			return true;
		}
	} else {
		return true;
	}

	// we haven't hit max scale (1) yet
	return false;
}

} // namespace SmokyFrog
} // namespace Game