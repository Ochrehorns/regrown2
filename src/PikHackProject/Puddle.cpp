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

	mFsm->start(this, 0, nullptr);
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
	shadowParam.mBoundingSphere.mRadius   = 0.0f;
	shadowParam.mSize                     = 0.0f;
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

void StateWait::exec(EnemyBase* enemy) { }

} // namespace Puddle
} // namespace Game
