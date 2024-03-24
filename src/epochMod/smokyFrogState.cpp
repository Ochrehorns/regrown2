#include "Game/Entities/SmokyFrog.h"
#include "Game/EnemyFunc.h"
#include "Game/EnemyAnimKeyEvent.h"

namespace Game {
namespace SmokyFrog {

void FSM::init(EnemyBase* enemy)
{
	create(SMOKYFROG_StateCount);
	registerState(new Frog::StateDead);
	registerState(new Frog::StateWait);
	registerState(new Frog::StateTurn);
	registerState(new Frog::StateJump);
	registerState(new Frog::StateJumpWait);
	registerState(new Frog::StateFall);
	registerState(new Frog::StateAttack);
	registerState(new Frog::StateFail);
	registerState(new Frog::StateTurnToHome);
	registerState(new Frog::StateGoHome);
	registerState(new StateStay);
	registerState(new StateAppear);
}

void StateStay::init(EnemyBase* enemy, StateArg* arg)
{
	Obj* frog = OBJ(enemy);
	frog->enableEvent(0, EB_Invulnerable);
	frog->enableEvent(0, EB_BitterImmune);
	frog->disableEvent(0, EB_Cullable);
	frog->hardConstraintOn();
	frog->enableEvent(0, EB_ModelHidden);

	frog->setTargetVelocity(Vector3f::zero);

	frog->setEmotionNone();
	frog->mNextState = Frog::FROG_NULL;

	frog->mShadowScale         = 0.0f;
	frog->mLodParm.mIsCylinder = true;

	frog->mPosition.y += SMOKYFROG_START_HEIGHT;

	frog->startMotion(Frog::FROGANIM_Fall, nullptr);
}

void StateStay::exec(EnemyBase* enemy)
{
	Obj* frog = OBJ(enemy);
	if (frog->mShadowScale > 0.0f) {
		if (frog->addShadowScale() && frog->mBounceTriangle) {
			transit(frog, SMOKYFROG_Appear, nullptr);
		}
		return;
	}

	// the sight range and search angle are sort of arbitrary here,
	// just used to allow the frog to see targets from all the way up above
	Creature* target = EnemyFunc::getNearestPikminOrNavi(frog, 180.0f, 100.0f, nullptr, nullptr, nullptr);
	if (target == nullptr) {
		return;
	}

	frog->disableEvent(0, EB_ModelHidden);
	frog->hardConstraintOff();
	frog->mCurrentVelocity = Vector3f(0.0f, SMOKYFROG_APPEAR_VEL, 0.0f);

	shadowMgr->addShadow(frog);
	frog->getJAIObject()->startSound(PSSE_EN_DANGO_FALL, 0);

	if (frog->addShadowScale()) {
		transit(frog, SMOKYFROG_Appear, nullptr);
	}
}

void StateStay::cleanup(EnemyBase* enemy) { }

void StateAppear::init(EnemyBase* enemy, StateArg* arg)
{
	Obj* frog                  = OBJ(enemy);
	frog->mLodParm.mIsCylinder = false;

	frog->enableEvent(0, EB_BitterImmune);
	frog->disableEvent(0, EB_Invulnerable);

	frog->setTargetVelocity(Vector3f::zero);
	frog->setVelocity(Vector3f::zero);

	frog->setBossAppearBGM();
	frog->setEmotionExcitement();

	frog->pressOnGround();

	frog->startMotion(Frog::FROGANIM_Attack, nullptr);
}

void StateAppear::exec(EnemyBase* enemy)
{
	if (enemy->mCurAnim->mIsPlaying && enemy->mCurAnim->mType == KEYEVENT_END) {
		transit(enemy, Frog::FROG_Wait, nullptr);
	}
}

void StateAppear::cleanup(EnemyBase* enemy) { enemy->disableEvent(0, EB_BitterImmune); }

} // namespace SmokyFrog
} // namespace Game
