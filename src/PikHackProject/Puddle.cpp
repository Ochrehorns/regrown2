#include "Game/Entities/Puddle.h"
#include "Game/EnemyAnimKeyEvent.h"
#include "Game/generalEnemyMgr.h"
#include "Game/EnemyFunc.h"
#include "Game/SingleGame.h"
#include "Radar.h"
#include "Dolphin/rand.h"
#include "efx/Arg.h"

namespace Game {
namespace Puddle {

Mgr::Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "PuddleMgr";
}

void Mgr::doAlloc() { init(new Parms); }

void Mgr::createObj(int count) { mObj = new Obj[count]; }

void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);

	disableEvent(0, EB_LeaveCarcass);
	setEmotionNone();
	enableEvent(0, EB_BitterImmune);
	enableEvent(0, EB_Invulnerable);
	enableEvent(0, EB_Untargetable);

	mFsm->start(this, PUDDLE_Normal, nullptr);
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

void Obj::getShadowParam(ShadowParam& shadowParam) { }

void Obj::appear() { mCurrentLifecycleState->transit(this, PUDDLE_Spawn, nullptr); }
void Obj::move() { mCurrentLifecycleState->transit(this, PUDDLE_Normal, nullptr); }
void Obj::leave() { mCurrentLifecycleState->transit(this, PUDDLE_Leave, nullptr); }

void ProperAnimator::setAnimMgr(SysShape::AnimMgr* mgr) { mAnimator.mAnimMgr = mgr; }

SysShape::Animator& ProperAnimator::getAnimator(int) { return mAnimator; }

void FSM::init(Game::EnemyBase* enemy)
{
	create(PUDDLE_Count);
	registerState(new StateSpawn(PUDDLE_Spawn));
	registerState(new StateNormal(PUDDLE_Normal));
	registerState(new StateLeave(PUDDLE_Leave));
}

void StateSpawn::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("puddle appear\n");
	enemy->startMotion(1, nullptr);
}
void StateSpawn::exec(EnemyBase* enemy) { }

void StateNormal::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("puddle normal\n");
	enemy->startMotion(0, nullptr);
}
void StateNormal::exec(EnemyBase* enemy) { }

void StateLeave::init(EnemyBase* enemy, StateArg* stateArg)
{
	OSReport("puddle leave\n");
	enemy->startMotion(2, nullptr);
}
void StateLeave::exec(EnemyBase* enemy) { }

} // namespace Puddle
} // namespace Game
