#include "Game/Entities/Truth.h"
#include "Game/Entities/Puddle.h"
#include "Game/EnemyAnimKeyEvent.h"
#include "Game/generalEnemyMgr.h"
#include "Game/EnemyFunc.h"
#include "Game/SingleGame.h"
#include "Radar.h"
#include "Game/CameraMgr.h"
#include "Game/rumble.h"
#include "Dolphin/rand.h"
#include "efx/Arg.h"

namespace Game {
namespace Truth {

Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "TruthMgr";
}

void Mgr::doAlloc() { init(new Parms); }

void Mgr::createObj(int count) { mObj = new Obj[count]; }

void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);

	disableEvent(0, EB_LeaveCarcass);
	enableEvent(0, EB_BitterImmune);
	mWaitTimer = 0.0f;

	EnemyMgrBase* mgr = generalEnemyMgr->getEnemyMgr(EnemyTypeID::EnemyID_Puddle);
	if (mgr) {
		EnemyBirthArg args;
		args.mFaceDir  = mFaceDir;
		args.mPosition = mPosition;
		EnemyBase* man = mgr->birth(args);
		if (man) {
			man->init(nullptr);
		}
		mPuddle = (Puddle::Obj*)man;
	}
	JUT_ASSERT(mPuddle, "failed to spawn puddle!\n");

	mFsm->start(this, TRUTH_Spawn, nullptr);
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

void Obj::doUpdate()
{
	mFsm->exec(this);

	if (mPuddle) {
		mPuddle->mPosition = getPosition();
		mPuddle->mFaceDir  = getFaceDir();
	}
}

void Obj::doDirectDraw(Graphics&) { }

void Obj::doDebugDraw(Graphics& gfx) { EnemyBase::doDebugDraw(gfx); }

void Obj::getShadowParam(ShadowParam& shadowParam) { }

void ProperAnimator::setAnimMgr(SysShape::AnimMgr* mgr) { mAnimator.mAnimMgr = mgr; }

SysShape::Animator& ProperAnimator::getAnimator(int) { return mAnimator; }

void FSM::init(Game::EnemyBase* enemy)
{
	create(TRUTH_Count);
	registerState(new StateSpawn(TRUTH_Spawn));
	registerState(new StateWait(TRUTH_Wait));
	registerState(new StateMove(TRUTH_Move));
	registerState(new StateHide(TRUTH_Hide));
	registerState(new StateHideMove(TRUTH_HideMove));
	registerState(new StateAppear(TRUTH_Appear));
	registerState(new StateHurt(TRUTH_Hurt));
	registerState(new StateDead(TRUTH_Dead));
	registerState(new StateAttack(TRUTH_Attack));
	registerState(new StateRoar(TRUTH_Roar));
	registerState(new StateUlt(TRUTH_Ult));
}

void StateSpawn::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("start spawn\n");
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	// start spawn anim, dont actually animate yet
	boss->mTargetCreature = nullptr;
	boss->mTargetVelocity = Vector3f(0.0f);
	boss->enableEvent(0, EB_Invulnerable);
	boss->enableEvent(0, EB_Untargetable);
	boss->startMotion(8, nullptr); // spawn.bca
	boss->stopMotion();

	// make puddle visible in this state
	boss->mPuddle->move();
}

void StateSpawn::exec(EnemyBase* enemy)
{
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	// search for a target, if its too far, move toward the target, appear once its close enough
	enemy->mTargetCreature = (Creature*)EnemyFunc::getNearestNavi(enemy, 360.0f, parms->mGeneral.mSightRadius(), nullptr, nullptr);

	if (enemy->mTargetCreature) {
		if (enemy->mTargetCreature->getDistanceTo(enemy) < parms->mGeneral.mHomeRadius()) {
			transit(enemy, TRUTH_Appear, nullptr);
		} else {
			EnemyFunc::walkToTarget(enemy, enemy->mTargetCreature, parms->mGeneral.mMoveSpeed(), parms->mGeneral.mTurnSpeed(),
			                        parms->mGeneral.mMaxTurnAngle());
		}
	} else {
		boss->mTargetVelocity = 0.0f;
	}
}

void StateWait::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("start wait\n");
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	boss->mTargetCreature = nullptr;
	boss->mTargetVelocity = Vector3f(0.0f);
	boss->startMotion(5, nullptr); // idle.bca
	boss->mWaitTimer = 0.0f;
}

void StateWait::exec(EnemyBase* enemy)
{
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	if (boss->mHealth <= 0.0f) {
		transit(boss, TRUTH_Dead, nullptr);
	}

	// search for a target, if its too far, move toward the target, appear once its close enough
	enemy->mTargetCreature = (Creature*)EnemyFunc::getNearestNavi(enemy, 360.0f, parms->mGeneral.mSightRadius(), nullptr, nullptr);

	if (enemy->mTargetCreature) {
		enemy->changeFaceDir(enemy->mTargetCreature);
	} else {
		boss->mWaitTimer += sys->mDeltaTime;
		if (boss->mWaitTimer > 7.0f) {
			transit(boss, TRUTH_Hide, nullptr);
		}
	}
}

void StateMove::init(EnemyBase* enemy, StateArg* stateArg) { }
void StateMove::exec(EnemyBase* enemy) { }

void StateHide::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("start hide\n");
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	boss->mTargetCreature = nullptr;
	boss->mTargetVelocity = Vector3f(0.0f);
	boss->startMotion(0, nullptr); // death.bca
	boss->mWaitTimer = 0.0f;

	boss->mPuddle->appear();
}

void StateHide::exec(EnemyBase* enemy)
{
	if (enemy->mCurAnim->mType == KEYEVENT_END && enemy->mCurAnim->mIsPlaying) {
		transit(enemy, TRUTH_HideMove, 0);
	}
}

void StateHideMove::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("start hide move\n");
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	boss->mTargetCreature = nullptr;
	boss->mTargetVelocity = Vector3f(0.0f);
	boss->mWaitTimer      = 0.0f;

	boss->mPuddle->move();
}

void StateHideMove::exec(EnemyBase* enemy)
{
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	// search for a target, if its too far, move toward the target, appear once its close enough
	enemy->mTargetCreature = (Creature*)EnemyFunc::getNearestNavi(enemy, 360.0f, parms->mGeneral.mSightRadius(), nullptr, nullptr);

	if (enemy->mTargetCreature) {
		if (enemy->mTargetCreature->getDistanceTo(enemy) < parms->mGeneral.mHomeRadius()) {
			transit(enemy, TRUTH_Appear, nullptr);
		} else {
			EnemyFunc::walkToTarget(enemy, enemy->mTargetCreature, parms->mGeneral.mMoveSpeed(), parms->mGeneral.mTurnSpeed(),
			                        parms->mGeneral.mMaxTurnAngle());
		}
	} else {
		boss->mTargetVelocity = 0.0f;
	}
}

void StateAppear::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("start appear\n");
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	// play spawn anim in full
	boss->mTargetCreature = nullptr;
	boss->mTargetVelocity = Vector3f(0.0f);
	boss->disableEvent(0, EB_Untargetable);
	boss->startMotion(8, nullptr); // spawn.bca

	// make puddle hidden in this state
	boss->mPuddle->leave();

	// when appearing, always face directly towards target
	if (boss->mTargetCreature) {
		boss->mFaceDir
		    = atan2(boss->mPosition.x - boss->mTargetCreature->getPosition().x, boss->mPosition.z - boss->mTargetCreature->getPosition().z);
	}

	cameraMgr->startVibration(0x1a, boss->mPosition, 2);
	rumbleMgr->startRumble(3, boss->mPosition, 2);
}

void StateAppear::exec(EnemyBase* enemy)
{
	if (enemy->mCurAnim->mType == KEYEVENT_END && enemy->mCurAnim->mIsPlaying) {
		transit(enemy, TRUTH_Wait, 0);
	}
}

void StateHurt::init(EnemyBase* enemy, StateArg* stateArg) { }
void StateHurt::exec(EnemyBase* enemy) { }

void StateDead::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("start dead\n");
	Obj* boss    = OBJ(enemy);
	Parms* parms = CG_PARMS(enemy);

	boss->mTargetCreature = nullptr;
	boss->mTargetVelocity = Vector3f(0.0f);
	boss->startMotion(0, nullptr); // death.bca

	EnemyFunc::flickStickPikmin(boss, parms->mGeneral.mShakeChance(), parms->mGeneral.mShakeKnockback(), parms->mGeneral.mShakeDamage(),
	                            -1000.0f, 0);
	boss->deathProcedure();
}

void StateDead::exec(EnemyBase* enemy)
{
	if (enemy->mCurAnim->mIsPlaying && enemy->mCurAnim->mType == KEYEVENT_END) {
		enemy->kill(nullptr);
	}
}

void StateAttack::init(EnemyBase* enemy, StateArg* stateArg) { }
void StateAttack::exec(EnemyBase* enemy) { }

void StateRoar::init(EnemyBase* enemy, StateArg* stateArg) { }
void StateRoar::exec(EnemyBase* enemy) { }

void StateUlt::init(EnemyBase* enemy, StateArg* stateArg) { }
void StateUlt::exec(EnemyBase* enemy) { }

} // namespace Truth
} // namespace Game
