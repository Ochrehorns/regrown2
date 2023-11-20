#include "Game/Entities/SmokyFrog.h"
#include "Game/Navi.h"
#include "efx/TFrog.h"

namespace Game {
namespace SmokyFrog {

Obj::Obj() { createEffect(); }

void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);
	_2C4       = 128.0f;
	mAirTimer  = 0.0f;
	mNextState = Frog::FROG_NULL;
	mIsInAir   = false;
	mIsFalling = false;
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

			if (checkDistAndAngle(navi, angledist, maxAttackRange, minAttackRange)) {
				mTargetPosition = Vector3f(navi->getPosition());
			}
		}
	}
}

void Obj::createEffect() { mEfxPota = new efx::TFrogPota; }

} // namespace SmokyFrog
} // namespace Game
