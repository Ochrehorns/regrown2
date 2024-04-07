#include "Game/Entities/BlackMan.h"
#include "efx/TPlasm.h"
#include "PS.h"


namespace Game {

namespace BlackMan {

Obj::Obj()
    : mPostFlickState(-1)
    , _2E4(0)
    , mRouteFindTimer(0)
    , mStepTimer(0)
    , mStepPhase(0)
    , mEscapeTimer(0)
    , mRouteFindCooldownTimer(0)
    , mIsSameWaypoint(false)
    , mWraithFallTimer(10.0f)
    , mCurrentWaypointIndex(-1)
    , mPreviousWaypointIndex(-1)
    , mNextWaypointIndex(-1)
    , mPathFindingHandle(0)
    , mFoundPath(0)
{
	mMatLoopAnimator = nullptr;
	mPath            = nullptr;
	mFSM             = nullptr;
	mTyre            = nullptr;
	mFadeTimer       = 1.0f;
	_37C             = nullptr;
	mEfxMove         = nullptr;
	mEfxRun          = nullptr;
	mEfxTyreup       = nullptr;
	mEfxDead         = nullptr;
	_3A8             = 0;
	_3A9             = 0;
	_3AA             = 0;
	mIsMoviePlaying  = 0;

	mAnimator        = new ProperAnimator;
	mMatLoopAnimator = new Sys::MatLoopAnimator;

	setFSM(new FSM);

	bool gold = isGold();

	mEfxMove       = new efx::TKageMove(&mChestJointPosition, &mFaceDir, gold);
	mEfxRun        = new efx::TKageRun(&mChestJointPosition, &mFaceDir);
	mEfxDead       = new efx::TKageDead1(gold);
	mEfxTyreup     = new efx::TKageTyreup(&mLandPosition, &mFaceDir);
	mEfxFrontFlick = new efx::TKageFlick(&mHandPositions[0], gold);
	mEfxBackFlick  = new efx::TKageFlick(&mHandPositions[1], gold);

	SetupPlasmEffects();
}

void Obj::onKill(Game::CreatureKillArg* arg)
{
	EnemyBase::onKill(arg);
	releasePathFinder();
	fadeFlickEffect();
	KillPlasmEffects();
}

efx::TPlasmSparkle* sPlasmSparkle;

void Obj::SetupPlasmEffects()
{
	if (isGold()) {
		sPlasmSparkle       = new efx::TPlasmSparkle;
		sPlasmSparkle->mMtx = &mBaseTrMatrix;
	}
}

void Obj::StartPlasmEffects()
{
	if (isGold())
		sPlasmSparkle->create(nullptr);
}

void Obj::KillPlasmEffects()
{
	if (isGold())
		sPlasmSparkle->fade();
}

void BlackMan::Obj::deadEffect()
{
	EnemyBase::createDeadBombEffect();
	throwupItem();
	PSStartEnemyFatalHitSE(this, 0.0f);
	efx::TKageDead2 deadFX(isGold());
	efx::ArgPrmColor fxArg(mChestJointPosition);
	deadFX.create(&fxArg);
}


} // namespace BlackMan

} // namespace Game
