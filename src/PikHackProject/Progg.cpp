#include "Game/Entities/Progg.h"
#include "Game/EnemyAnimKeyEvent.h"
#include "Game/generalEnemyMgr.h"
#include "Game/EnemyFunc.h"
#include "LoadResource.h"
#include "Sys/MatBaseAnimation.h"
#include "Game/CameraMgr.h"
#include "Game/routeMgr.h"
#include "Game/MapMgr.h"
#include "Game/rumble.h"
#include "Game/pathfinder.h"
#include "Radar.h"
#include "Game/Entities/ItemOnyon.h"
#include "Game/ConditionNotStick.h"
#include "Dolphin/rand.h"

namespace Game {
namespace Dororo {

void Mgr::loadTexData()
{
	SysShape::Model::enableMaterialAnim(mModelData, 0);
	void* btkFile = nullptr;
	LoadResource::Arg loadArgBtk("/enemy/data/Dororo/dororo_model.btk");
	LoadResource::Node* resourceBtk = gLoadResourceMgr->load(loadArgBtk);
	if (resourceBtk) {
		btkFile = resourceBtk->mFile;
	}
	P2ASSERTLINE(83, btkFile);

	mTexAnimation = new Sys::MatTexAnimation;
	mTexAnimation->attachResource(btkFile, mModelData);
}

SysShape::Model* Mgr::createModel()
{
	SysShape::Model* model = new SysShape::Model(mModelData, 0x80000, mModelType);

	model->mJ3dModel->calc();
	model->mJ3dModel->calcMaterial();
	model->mJ3dModel->makeDL();
	return model;
}

void ProperAnimator::setAnimMgr(SysShape::AnimMgr* mgr) { mAnimator.mAnimMgr = mgr; }

SysShape::Animator& ProperAnimator::getAnimator(int) { return mAnimator; }

Obj::Obj()
{
	mFsm      = nullptr;
	mAnimator = new ProperAnimator;
	setFSM(new FSM);
	createEffect();
	mMatAnim = new Sys::MatLoopAnimator;
}

void Obj::setFSM(FSM* fsm)
{
	mFsm = fsm;
	mFsm->init(this);
	mCurrentLifecycleState = nullptr;
}

void Obj::setInitialSetting(EnemyInitialParamBase*) { }

void Obj::doUpdate() { mFsm->exec(this); 

// When the Progg is in these states, an ambient wind noise will play for it.
	if (getStateID() == PROGG_Born || getStateID() == PROGG_Chase || getStateID() == PROGG_Flick || getStateID() == PROGG_Roar || getStateID() == PROGG_Wait || getStateID() == PROGG_Look || getStateID() == PROGG_Path) {
		mSoundObj->startSound(0x519A, 0);
	}
}

void Obj::doDirectDraw(Graphics&) { }

void Obj::doDebugDraw(Graphics& gfx) { EnemyBase::doDebugDraw(gfx); }

void Obj::getShadowParam(ShadowParam& shadowParam)
{
	shadowParam.mPosition                 = mPosition;
	shadowParam.mPosition.y               = mPosition.y + 3.0f;
	shadowParam.mBoundingSphere.mPosition = Vector3f(0.0f, 1.0f, 0.0f);
	if (mPellet) {
		shadowParam.mBoundingSphere.mRadius = 9.0f;
		shadowParam.mSize                   = 9.0f;
	} else {
		shadowParam.mBoundingSphere.mRadius = 30.0f;
		shadowParam.mSize                   = 30.0f;
	}
}

bool Obj::damageCallBack(Creature* owner, f32 damage, CollPart* part)
{
	if (part == nullptr || part->mCurrentID.match('died', '*')) {
		return 0;
	}

	addDamage(damage, 1.0f);
	return (damage > 0.0f);
}

void Obj::doStartMovie()
{
	mSmokeEfx->startDemoDrawOff();
	mEyeEfx1->startDemoDrawOff();
	mEfxCore->startDemoDrawOff();
}

void Obj::doEndMovie()
{
	mSmokeEfx->endDemoDrawOn();
	mEyeEfx1->endDemoDrawOn();
	mEfxCore->endDemoDrawOn();
}

void Obj::createEffect()
{
	mSmokeEfx = new efx::TChaseMtx4(nullptr, 722, 723, 725, 726);
	mEyeEfx1  = new efx::TChaseMtx(721, nullptr);
	mEfxCore  = new efx::TChaseMtx(724, nullptr);
}

void Obj::onInit(CreatureInitArg* initArg)
{
	EnemyBase::onInit(initArg);
	mFsm->start(this, 0, 0);
	disableEvent(0, EB_Cullable);
	enableEvent(0, EB_BitterImmune);

	setupEffect();
	mRoarTimer = 0.0f;

	Mgr* mgr = (Mgr*)mMgr;
	mMatAnim->start(mgr->mTexAnimation);
	mPathfindID     = 0;
	mPathNode       = 0;
	mTargetCreature = nullptr;
}

void Obj::setupEffect()
{
	mSmokeEfx->create(nullptr);
	mEyeEfx1->create(nullptr);
	mEfxCore->create(nullptr);

	mSmokeEfx->setMtxptr(mModel->getJoint("root")->getWorldMatrix()->mMatrix.mtxView);
	mSmokeEfx->mItems[0].mEmitter->setScaleOnly(mScaleModifier);

	mEyeEfx1->mMtx = mModel->getJoint("head")->getWorldMatrix();
	mEyeEfx1->mEmitter->setScaleOnly(mScaleModifier);

	mEfxCore->mMtx = mModel->getJoint("head")->getWorldMatrix();
	mEfxCore->mEmitter->setScaleOnly(mScaleModifier);
}

void Obj::initWalkSmokeEffect()
{
	mWalkSmoke.alloc(2);
	mWalkSmoke.setup(0, mModel, "handR", 5.0f);
	mWalkSmoke.setup(1, mModel, "handL", 5.0f);
}

void Obj::onKill(CreatureKillArg* arg)
{
	EnemyBase::onKill(arg);
	mEyeEfx1->forceKill();
	mEfxCore->forceKill();
	mSmokeEfx->forceKill();
}

void Obj::changeMaterial()
{
	mMatAnim->animate(30.0f);
	mModel->mJ3dModel->calcMaterial();
	mModel->mJ3dModel->diff();
}

void Obj::updateSplashEfx()
{
	if (mCurAnim->mType == 3 && mCurAnim->mIsPlaying) {
		if (inWater()) {
			Matrixf* mtx = mModel->getJoint("handR")->getWorldMatrix();
			efx::TSimple2 diveefx(PID_EnemyDive_1, PID_EnemyDive_2);
			efx::Arg earg(mtx->getTranslation());
			diveefx.create(&earg);
			mSoundObj->startSound(0x59A2, 0);
		} else {
			mSoundObj->startSound(0x5999, 0);
		}
	}

	if (mCurAnim->mType == 2 && mCurAnim->mIsPlaying) {
		if (inWater()) {
			Matrixf* mtx = mModel->getJoint("handL")->getWorldMatrix();
			efx::TSimple2 diveefx(PID_EnemyDive_1, PID_EnemyDive_2);
			efx::Arg earg(mtx->getTranslation());
			diveefx.create(&earg);
			mSoundObj->startSound(0x59A2, 0);
		} else {
			mSoundObj->startSound(0x5999, 0);
		}
	}
}

void Obj::collisionCallback(CollEvent& coll)
{
	if (!gameSystem->isFlag(GAMESYS_IsGameWorldActive)) {
		return;
	}

	int state = getStateID();
	if (coll.mHitPart->mCurrentID.match('died', '*') && state != PROGG_Dead && state != PROGG_Born) {
		if (coll.mCollidingCreature->isPiki()) {
			InteractKill act(this, nullptr);
			coll.mCollidingCreature->stimulate(act);
		} else if (coll.mCollidingCreature->isNavi()) {
			InteractAttack act(this, C_PARMS->mGeneral.mAttackDamage(), coll.mHitPart);
			coll.mCollidingCreature->stimulate(act);
		}
		return;
	}

	EnemyBase::collisionCallback(coll);
}

void Obj::startCarcassMotion() { startMotion(5, 0); }

void FSM::init(EnemyBase* enemy)
{
	create(PROGG_Count);
	registerState(new StateBorn);
	registerState(new StateChase);
	registerState(new StateDead);
	registerState(new StateFlick);
	registerState(new StateRoar);
	registerState(new StateWait);
	registerState(new StateLook);
	registerState(new StatePath);
}

void StateBorn::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(0, 0);
	obj->hardConstraintOn();
	obj->setEmotionCaution();
}

void StateBorn::exec(EnemyBase* obj)
{
	if (obj->mCurAnim->mType == KEYEVENT_END) {
		transit(obj, PROGG_Path, nullptr); // path state
	}
}

void StateBorn::cleanup(EnemyBase* obj) { obj->hardConstraintOff(); }

void StateChase::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(7, 0);
	obj->setEmotionExcitement();
	obj->mTargetCreature = nullptr;
}

void StateChase::exec(EnemyBase* obj)
{
	Obj* progg = OBJ(obj);
	Vector3f goal(0.0f);

	ConditionNotStickClient condition(progg);

	progg->mTargetCreature = EnemyFunc::getNearestPikmin(progg, CG_PARMS(progg)->mGeneral.mViewAngle(),
	                                                     CG_PARMS(progg)->mGeneral.mSightRadius(), 0, &condition);

	if (progg->mTargetCreature) {
		goal = progg->mTargetCreature->getPosition();
	} else {
		progg->finishMotion();
		progg->mNextState = PROGG_Path;
	}

	EnemyFunc::walkToTarget(progg, goal, CG_PARMS(progg)->mGeneral.mMoveSpeed(), CG_PARMS(progg)->mGeneral.mTurnSpeed(),
	                        CG_PARMS(progg)->mGeneral.mMaxTurnAngle());

	progg->updateSplashEfx();

	if (progg->mHealth <= 0.0f) {
		progg->finishMotion();
		progg->mNextState = PROGG_Dead;
	}
	if (EnemyFunc::isStartFlick(progg, 1)) {
		progg->finishMotion();
		progg->mNextState = PROGG_Flick;
	}

	if (progg->mCurAnim->mType == KEYEVENT_END && progg->mNextState != -1) {
		transit(progg, progg->mNextState, 0);
	}
}

void StateChase::cleanup(EnemyBase* obj)
{
	obj->setEmotionCaution();
	obj->mTargetCreature = nullptr;
}

void StateDead::init(EnemyBase* obj, StateArg* arg)
{
	Obj* progg   = OBJ(obj);
	Parms* parms = CG_PARMS(obj);

	progg->startMotion(2, 0);
	progg->mTargetVelocity  = 0.0f;
	progg->mCurrentVelocity = 0.0f;
	progg->mSmokeEfx->fade();

	EnemyFunc::flickStickPikmin(obj, parms->mGeneral.mShakeChance(), parms->mGeneral.mShakeKnockback(), parms->mGeneral.mShakeDamage(),
	                            -1000.0f, 0);
	progg->deathProcedure();

	efx::TSimple2 deadefx(736, 737);
	efx::Arg earg(obj->mPosition);
	deadefx.create(&earg);
}

void StateDead::exec(EnemyBase* obj)
{
	if (obj->mCurAnim->mIsPlaying && obj->mCurAnim->mType == KEYEVENT_END) {
		obj->kill(nullptr);
	}
}

void StateDead::cleanup(EnemyBase* obj) { }

void StateFlick::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(3, 0);
	obj->mTargetVelocity  = 0.0f;
	obj->mCurrentVelocity = 0.0f;
	obj->mFlickTimer      = 0.0f;
	obj->setEmotionExcitement();
}

void StateFlick::exec(EnemyBase* obj)
{
	if (obj->mCurAnim->mType == 2 && obj->mCurAnim->mIsPlaying) {
		Parms* parms = CG_PARMS(obj);
		EnemyFunc::flickStickPikmin(obj, parms->mGeneral.mShakeChance(), parms->mGeneral.mShakeKnockback(), parms->mGeneral.mShakeDamage(),
		                            -1000.0f, 0);
		EnemyFunc::flickNearbyPikmin(obj, parms->mGeneral.mShakeRange(), parms->mGeneral.mShakeKnockback(), parms->mGeneral.mShakeDamage(),
		                             -1000.0f, 0);
		EnemyFunc::flickNearbyNavi(obj, parms->mGeneral.mShakeRange(), parms->mGeneral.mShakeKnockback(), parms->mGeneral.mShakeDamage(),
		                           -1000.0f, 0);
	}
	if (obj->mHealth <= 0.0f) {
		transit(obj, PROGG_Dead, 0);
	}
	if (obj->mCurAnim->mType == KEYEVENT_END && obj->mCurAnim->mIsPlaying) {
		transit(obj, PROGG_Chase, 0);
	}
}

void StateFlick::cleanup(EnemyBase* obj) { obj->setEmotionCaution(); }

void StateRoar::init(EnemyBase* obj, StateArg* arg)
{
	obj->startMotion(1, 0);
	obj->mTargetVelocity  = 0.0f;
	obj->mCurrentVelocity = 0.0f;
	obj->setEmotionExcitement();
}

void StateRoar::exec(EnemyBase* obj)
{
	if (obj->mCurAnim->mType == 2 && obj->mCurAnim->mIsPlaying) {
		cameraMgr->startVibration(0x1a, obj->mPosition, 2);
		rumbleMgr->startRumble(3, obj->mPosition, 2);

		Parms* parms = CG_PARMS(obj);

		EnemyFunc::flickStickPikmin(obj, parms->mGeneral.mShakeChance(), parms->mGeneral.mShakeKnockback(), parms->mGeneral.mShakeDamage(),
		                            -1000.0f, 0);

		efx::TSimple2 roarefx(751, 752);
		efx::Arg earg(obj->mPosition);
		earg.mPosition.y += 20.0f;
		roarefx.create(&earg);

		Sys::Sphere bounds(obj->mPosition, parms->mGeneral.mAttackRadius());
		CellIteratorArg carg(bounds);
		CellIterator it(carg);
		CI_LOOP(it)
		{
			Creature* piki = (Creature*)*it;

			if (piki->mObjectTypeID == OBJTYPE_Pikihead) {
				InteractFue act(obj, 0, 0);
				piki->stimulate(act);
			} else {
				// scare pikmin (idc if it doesnt in 1 + ratio)
				InteractAstonish act(obj, 200.0f);
				piki->stimulate(act);
			}
		}
	}

	if (obj->mCurAnim->mType == KEYEVENT_END && obj->mCurAnim->mIsPlaying) {
		transit(obj, PROGG_Wait, 0);
	}
}

void StateRoar::cleanup(EnemyBase* obj) { obj->setEmotionCaution(); }

void StateWait::init(EnemyBase* obj, StateArg* arg)
{
	Obj* progg   = OBJ(obj);
	Parms* parms = CG_PARMS(obj);

	progg->startMotion(6, 0);
	progg->mTargetVelocity  = 0.0f;
	progg->mCurrentVelocity = 0.0f;

	progg->mPikiTarget = nullptr;
	progg->mRoarTimer  = parms->mProperParms.mWaitTime();
	progg->mNextState  = PROGG_Wait;
}

void StateWait::exec(EnemyBase* obj)
{
	Obj* progg   = OBJ(obj);
	Parms* parms = CG_PARMS(obj);

	if (progg->mHealth <= 0.0f) {
		transit(progg, PROGG_Dead, 0);
	}

	progg->mRoarTimer -= sys->mDeltaTime;

	if (progg->mRoarTimer <= 0.0f) {
		ConditionNotStickClient condition(progg);

		Piki* piki = EnemyFunc::getNearestPikmin(progg, CG_PARMS(progg)->mGeneral.mViewAngle(), CG_PARMS(progg)->mGeneral.mSightRadius(), 0,
		                                         &condition);

		progg->finishMotion();
		if (piki) {
			progg->mNextState = PROGG_Chase;
		} else { // nothing, just look around all helplessly and stuff
			progg->mNextState = PROGG_Look;
		}
	}

	// If progg needs to flick, stop waiting early
	if (EnemyFunc::isStartFlick(progg, 1)) {
		progg->finishMotion();
		progg->mNextState = PROGG_Flick;
	}

	if (progg->mCurAnim->mType == KEYEVENT_END && progg->mCurAnim->mIsPlaying) {
		transit(progg, progg->mNextState, 0);
	}
}

void StateWait::cleanup(EnemyBase* obj)
{
	Obj* progg        = OBJ(obj);
	progg->mRoarTimer = 0.0f;
}

void StateLook::init(EnemyBase* obj, StateArg* arg)
{
	Obj* progg = OBJ(obj);

	progg->startMotion(4, 0); // serch.bck
	// Default to wait state if nothing happens
	progg->mNextState = PROGG_Wait;
}

void StateLook::exec(EnemyBase* obj)
{
	Obj* progg = OBJ(obj);

	// If progg needs to flick, stop standing still like a dummy
	if (EnemyFunc::isStartFlick(progg, 1)) {
		progg->mNextState = PROGG_Flick;
	}

	if (gameSystem->isZukanMode()) {
		progg->mNextState = PROGG_Path;
	}

	if (progg->mCurAnim->mType == KEYEVENT_END && progg->mCurAnim->mIsPlaying) {
		transit(progg, progg->mNextState, 0); // go to next state (either wait or flick)
	}
}

void StateLook::cleanup(EnemyBase* obj) { }

void StatePath::init(EnemyBase* obj, StateArg* arg)
{
	Obj* progg = OBJ(obj);

	progg->startMotion(7, 0);
	progg->setEmotionCaution();
	progg->mNextState      = -1;
	progg->mTargetCreature = nullptr;

	Vector3f goalPos = 0.0f;
	// this manager doesnt exist in the piklopedia
	if (ItemOnyon::mgr) {
		progg->mTargetCreature = ItemOnyon::mgr->getOnyon(rand() % 3);
		// if an onion cant be found, target the ship
		if (progg->mTargetCreature == nullptr) {
			progg->mTargetCreature = ItemOnyon::mgr->mUfo;
		}

		// if theres still nothing youre probably in a cave, so target the pod
		if (progg->mTargetCreature == nullptr) {
			progg->mTargetCreature = ItemOnyon::mgr->mPod;
		}

		if (progg->mTargetCreature) {
			goalPos = progg->mTargetCreature->getPosition();
		}
	}

	// if theres STILL nothing, just pick a random spot, i.e. for the piklopedia
	if (progg->mTargetCreature == nullptr) {
		goalPos = mapMgr->mRouteMgr->getWayPoint(randInt(mapMgr->mRouteMgr->mCount - 1))->mPosition;
		OSReport("goal is %f %f %f\n", goalPos.x, goalPos.y, goalPos.z);
	}

	Game::WPEdgeSearchArg searchArg(progg->mPosition);
	WayPoint* start;
	if (mapMgr->mRouteMgr->getNearestEdge(searchArg)) {
		start = searchArg.mWp2;
		if (!(searchArg.mWp1->mFlags & 1)) {
			start = searchArg.mWp1;
		}
	} else {
		start = searchArg.mWp1;
		if (searchArg.mWp2->mFlags & 1) {
			start = searchArg.mWp2;
		}
	}

	P2ASSERT(testPathfinder);
	P2ASSERT(mapMgr->mRouteMgr);

	if (progg->mPathfindID != 0) {
		testPathfinder->release(progg->mPathfindID);
		progg->mPathfindID = 0;
	}
	progg->mIsBackupRoute = 0;

	WPSearchArg wpSearchArg(goalPos, nullptr, false, 10.0f);
	WayPoint* end   = mapMgr->mRouteMgr->getNearestWayPoint(wpSearchArg);
	progg->mStartWP = start;
	progg->mGoalWP  = end;

	if (start && end) {
		OSReport("start wp %i end wp %i\n", start->mIndex, end->mIndex);
		PathfindRequest req(start->mIndex, end->mIndex, 0x80 | 2);
		progg->mPathfindID = testPathfinder->start(req);
	} else {
		transit(progg, PROGG_Roar, 0);
	}
	progg->mHasPath = 0;
}

void StatePath::exec(EnemyBase* obj)
{
	Obj* progg   = OBJ(obj);
	Parms* parms = CG_PARMS(obj);

	if (!progg->mHasPath) {
		int check = testPathfinder->check(progg->mPathfindID);
		// 0 means success, anything else means abort state
		if (progg->mIsBackupRoute) {
			if (check == 1) {
				transit(obj, PROGG_Wait, 0);
				OSReport("backup pathing broke\n");
			} else if (check == 0 && progg->mPathfindID) {
				testPathfinder->makepath(progg->mPathfindID, &progg->mPathNode);
				progg->mHasPath = 1;
				OSReport("backup pathing ok");
			}
		} else {
			if (check == 0 && progg->mPathfindID) {
				OSReport("making backup route\n");
				testPathfinder->makepath(progg->mPathfindID, &progg->mPathNode);
				progg->mHasPath = 1;
			} else if (check == 1) {
				testPathfinder->release(progg->mPathfindID);
				progg->mPathfindID    = 0;
				progg->mIsBackupRoute = 1;

				// path to base cant be found, try again with different flags?
				PathfindRequest req(progg->mStartWP->mIndex, progg->mGoalWP->mIndex, 0x80 | 3);
				progg->mPathfindID = testPathfinder->start(req);
				OSReport("start backup path\n");
			}
		}
		return;
	}

	if (progg->mPathNode) {
		Vector3f goalPos = 0.0f;
		WayPoint* point  = mapMgr->mRouteMgr->getWayPoint(progg->mPathNode->mWpIndex);
		if (point) {
			goalPos = point->mPosition;
		}
		if (goalPos.distance(progg->mPosition) < parms->mProperParms.mWaypointSize()) {
			progg->mPathNode = progg->mPathNode->mNext;
			if (progg->mPathNode == nullptr || point->mFlags & 1) {
				progg->finishMotion();
				progg->mNextState = PROGG_Roar;
			}
		} else {
			EnemyFunc::walkToTarget(progg, goalPos, CG_PARMS(progg)->mGeneral.mMoveSpeed(), CG_PARMS(progg)->mGeneral.mTurnSpeed(),
			                        CG_PARMS(progg)->mGeneral.mMaxTurnAngle());
		}
	}

	progg->updateSplashEfx();

	if (progg->mHealth <= 0.0f) {
		progg->finishMotion();
		progg->mNextState = PROGG_Dead;
	}

	if (EnemyFunc::isStartFlick(progg, 1)) {
		progg->finishMotion();
		progg->mNextState = PROGG_Flick;
	}

	if (progg->mCurAnim->mType == KEYEVENT_END && progg->mCurAnim->mIsPlaying) {
		transit(progg, progg->mNextState, 0);
	}
}

void StatePath::cleanup(EnemyBase* obj) { }

} // namespace Dororo
} // namespace Game
