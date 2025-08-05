#include "Game/Entities/BigEgg.h"
#include "Game/EnemyAnimKeyEvent.h"
#include "Game/generalEnemyMgr.h"
#include "Game/EnemyFunc.h"
#include "Game/SingleGame.h"
#include "Radar.h"
#include "Dolphin/rand.h"
#include "efx/Arg.h"

namespace Game {
namespace BigEgg {

Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "BigEggMgr";
}

void Mgr::doAlloc() { init(new Parms); }

void Mgr::createObj(int count) { mObj = new Obj[count]; }

void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);

	// this is all copied from normal egg
	disableEvent(0, EB_LeaveCarcass);
	disableEvent(0, EB_DamageAnimEnabled);
	disableEvent(0, EB_DeathEffectEnabled);
	setEmotionNone();
	enableEvent(0, EB_BitterImmune);
	mFsm->start(this, 0, nullptr);

	// big egg specific stuff
	hardConstraintOn();
	mDoMakeProgg = false;
	mIsDead      = 0;
	mBreakTimer  = 0.0f;
}

Obj::Obj()
{
	mFsm      = nullptr;
	mAnimator = new ProperAnimator;
	setFSM(new FSM);
}

void Obj::setFSM(FSM* fsm)
{
	mFsm = fsm;
	mFsm->init(this);
	mCurrentLifecycleState = nullptr;
}

void Obj::setInitialSetting(EnemyInitialParamBase*) { }

void Obj::doUpdate() { mFsm->exec(this); }

void Obj::doDirectDraw(Graphics&) { }

void Obj::doDebugDraw(Graphics& gfx) { EnemyBase::doDebugDraw(gfx); }

void Obj::getShadowParam(ShadowParam& shadowParam)
{
	shadowParam.mPosition                 = mPosition;
	shadowParam.mPosition.y               = mPosition.y + 3.0f;
	shadowParam.mBoundingSphere.mPosition = Vector3f(0.0f, 1.0f, 0.0f);
	shadowParam.mBoundingSphere.mRadius   = 40.0f;
	shadowParam.mSize                     = 40.0f;
}

EnemyBase* Obj::genProgg()
{
	EnemyMgrBase* mgr = generalEnemyMgr->getEnemyMgr(EnemyTypeID::EnemyID_Dororo);
	if (mgr) {
		EnemyBirthArg args;
		args.mFaceDir    = mFaceDir;
		args.mPosition   = mPosition;
		args.mPelletInfo = mPelletInfo;

		EnemyBase* man = mgr->birth(args);
		if (man) {
			man->init(nullptr);

			mPelletInfo.mMaxPellets = 0;
			mPelletInfo.mMinPellets = 0;

			man->mPelletDropCode.mValue = mPelletDropCode.mValue;
			mPelletDropCode.mValue      = 0;

			// pass the eggs gen object onto progg, so if progg doesnt die, the egg will come back
			if (mGenerator) {
				man->mGenerator = mGenerator;
				mGenerator      = nullptr;
			}

			// in piklopedia, change the camera to target the progg instead of the egg
			if (gameSystem->isZukanMode()) {
				SingleGame::ZukanState* state = (SingleGame::ZukanState*)((SingleGameSection*)gameSystem->mSection)->getCurrState();
				state->mCamera->setTarget(man);
			}
		}
	}
}

void ProperAnimator::setAnimMgr(SysShape::AnimMgr* mgr) { mAnimator.mAnimMgr = mgr; }

SysShape::Animator& ProperAnimator::getAnimator(int) { return mAnimator; }

void FSM::init(Game::EnemyBase* enemy)
{
	create(EGG_Count);
	registerState(new StateWait(EGG_Wait));
}

StateWait::StateWait(int stateID)
    : State(stateID)
{
	mName = "wait";
}

void StateWait::init(EnemyBase* enemy, StateArg* stateArg)
{
	enemy->startMotion(0, nullptr);
	enemy->stopMotion();
}

void StateWait::exec(EnemyBase* enemy)
{
	Obj* egg     = OBJ(enemy);
	Parms* parms = CG_PARMS(egg);
	if (!egg->mIsDead) {

		Creature* piki = EnemyFunc::getNearestPikminOrNavi(egg, 360.0f, parms->mGeneral.mSightRadius(), 0, 0, 0);
		if (egg->mHealth <= 0.0f) {
			egg->mDoMakeProgg = false;
			egg->mIsDead      = true;
			egg->startMotion(0, 0);
		} else if (piki) {
			f32 dist = piki->getPosition().distance(egg->mPosition);
			if (dist < parms->mGeneral.mHomeRadius()) {
				egg->mDoMakeProgg = 1;
			}
		}

		// if egg is triggered, increase its timer until 10.5 seconds
		if (egg->mDoMakeProgg) {
			if (egg->mBreakTimer < parms->mProperParms.mWaitTime()) {
				egg->mBreakTimer += sys->mDeltaTime;
			} else {
				egg->startMotion(0, 0);
				egg->mBreakTimer = 0.0f;
				EnemyFunc::flickStickPikmin(egg, parms->mGeneral.mShakeChance(), parms->mGeneral.mShakeKnockback(),
				                            parms->mGeneral.mShakeDamage(), -1000.0f, 0);
				EnemyFunc::flickNearbyPikmin(egg, parms->mGeneral.mShakeRange(), parms->mGeneral.mShakeKnockback(),
				                             parms->mGeneral.mShakeDamage(), -1000.0f, 0);
				EnemyFunc::flickNearbyNavi(egg, parms->mGeneral.mShakeRange(), parms->mGeneral.mShakeKnockback(),
				                           parms->mGeneral.mShakeDamage(), -1000.0f, 0);
			}
		}
	}

	if (egg->mCurAnim->mIsPlaying && egg->mCurAnim->mType == 3) {
		efx::TSimple2 spawnefx(707, 709);
		efx::Arg arg(egg->mPosition);
		spawnefx.create(&arg);

		if (egg->mDoMakeProgg) {
			egg->genProgg();
			// move held treasure to progg
			Radar::Mgr::exit(egg);
		} else {
			efx::TSimple3 spawnefx(717, 716, 708);
			efx::Arg efxarg(egg->mPosition);
			spawnefx.create(&efxarg);
		}

		egg->deathProcedure();
	}

	if (egg->mCurAnim->mType == KEYEVENT_END) {
		egg->kill(0);
	}
}

} // namespace BigEgg
} // namespace Game
