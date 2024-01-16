#include "Game/Entities/SmokyFrog.h"
#include "Game/EnemyFunc.h"
#include "Game/CameraMgr.h"
#include "Game/rumble.h"
#include "Game/Navi.h"
#include "efx/TFrog.h"
#include "efx/TSmokyFrog.h"
#include "PS.h"

#define GAS_CLOUD_MODIFIER (1.0f)

namespace Game {
namespace SmokyFrog {

Obj::Obj() { createEffect(); }

void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);
	mAlertTimer  = 128.0f;
	mAirTimer    = 0.0f;
	mAttackTimer = 128.0f;
	mNextState   = Frog::FROG_NULL;
	mIsInAir     = false;
	mIsFalling   = false;
	setupEffect();
	mFsm->start(this, Frog::FROG_Wait, nullptr);
}

void Obj::setParameters()
{
	EnemyBase::setParameters();

	setScale(static_cast<Frog::Parms*>(mParms)->mProperParms.mScaleMult.mValue); // don't assign parm shit in the ctor
	mCurLodSphere.mRadius = mScaleModifier * static_cast<Frog::Parms*>(mParms)->mGeneral.mOffCameraRadius.mValue;
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
			}

			param.mBoundingSphere.mRadius = 0.75f * static_cast<Frog::Parms*>(mParms)->mProperParms.mJumpSpeed.mValue;
		}
	} else {
		param.mBoundingSphere.mRadius = 22.5f;
	}

	param.mBoundingSphere.mPosition = Vector3f(0.0f, 1.0f, 0.0f);
	param.mSize                     = 17.5f * static_cast<Frog::Parms*>(mParms)->mProperParms.mScaleMult.mValue;
}

void Obj::doUpdateCommon()
{
	EnemyBase::doUpdateCommon();
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
			f32 minAttackRange = static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMinAttackRange.mValue;
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
	f32 minRange = mPosition.y - static_cast<EnemyParmsBase*>(mParms)->mGeneral.mMinAttackRange.mValue;
	f32 radius   = static_cast<Frog::Parms*>(mParms)->mProperParms.mScaleMult.mValue * 50.0f;

	f32 dist = SQUARE(radius);

	Sys::Sphere sphere;
	sphere.mPosition = mPosition;
	sphere.mRadius   = radius;

	CellIteratorArg iterArg(sphere);
	iterArg.mIsSphereCollisionDisabled = true;
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

} // namespace SmokyFrog
} // namespace Game
