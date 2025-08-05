#ifndef _GAME_ENTITIES_PROGG_H
#define _GAME_ENTITIES_PROGG_H

#include "Game/EnemyStateMachine.h"
#include "Game/EnemyAnimatorBase.h"
#include "Game/EnemyParmsBase.h"
#include "Game/EnemyMgrBase.h"
#include "Game/EnemyBase.h"
#include "Game/PlatInstance.h"
#include "Collinfo.h"
#include "Game/WalkSmokeEffect.h"
#include "Platform.h"
#include "efx/TClam.h"

namespace Game {

struct WayPoint;

namespace Dororo {

struct FSM;

struct Parms : public EnemyParmsBase {
	struct ProperParms : public Parameters {
		ProperParms()
		    : Parameters(nullptr, "EnemyParmsBase")
		    , mWaypointSize(this, 'fp01', "Waypoint size", 1.0f, 10.0f, 20.0f)
		    , mWaitTime(this, 'fp02', "Wait Time", 1.0f, 10.0f, 20.0f)

		{
		}

		Parm<f32> mWaypointSize; // fp01
		Parm<f32> mWaitTime;     // fp02
	};

	Parms() { }

	virtual void read(Stream& stream) // _08 (weak)
	{
		CreatureParms::read(stream);
		mGeneral.read(stream);
		mProperParms.read(stream);
	}

	// _00-_7F8	= EnemyParmsBase
	ProperParms mProperParms; // _7F8
};

struct Obj : public EnemyBase {
	Obj();

	//////////////// VTABLE
	virtual void onInit(CreatureInitArg* settings);                                       // _30
	virtual void onKill(CreatureKillArg* settings);                                       // _34
	virtual void doDirectDraw(Graphics& gfx);                                             // _50
	virtual bool isFlying() { return true; }                                              // _CC (weak)
	virtual void collisionCallback(CollEvent& event);                                     // _EC
	virtual void getShadowParam(ShadowParam& settings);                                   // _134
	virtual ~Obj() { }                                                                    // _1BC (weak)
	virtual void setInitialSetting(EnemyInitialParamBase* params);                        // _1C4
	virtual void doUpdate();                                                              // _1CC
	virtual void doDebugDraw(Graphics& gfx);                                              // _1EC
	virtual void changeMaterial();                                                        // _200
	virtual void initWalkSmokeEffect();                                                   // _230
	virtual WalkSmokeEffect::Mgr* getWalkSmokeEffectMgr() { return &mWalkSmoke; }         // _234 (weak)
	virtual bool earthquakeCallBack(Creature* source, f32 bounceFactor) { return false; } // _28C
	virtual void startCarcassMotion();                                                    // _2C4
	virtual f32 getDownSmokeScale() { return 1.3f; }                                      // _2EC (weak)
	virtual void setFSM(FSM* fsm);                                                        // _2F8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()                                    // _258 (weak)
	{
		return EnemyTypeID::EnemyID_Dororo;
	}
	virtual void doStartMovie(); // _2F0
	virtual void doEndMovie();   // _2F4
	virtual bool damageCallBack(Creature* source, f32 damage, CollPart* part);
	//////////////// VTABLE END

	EnemyBase* genProgg();
	void createEffect();
	void setupEffect();
	void updateSplashEfx();

	// _00 		= VTBL
	// _00-_2BC	= EnemyBase
	FSM* mFsm;                       // _2BC
	efx::TChaseMtx4* mSmokeEfx;      // 2c4
	efx::TChaseMtx* mEyeEfx1;        // 2c8
	efx::TChaseMtx* mEfxCore;        // 2cc
	int mNextState;                  // 2d0
	f32 mRoarTimer;                  // 2d4
	Piki* mPikiTarget;               // 2d8
	Sys::MatLoopAnimator* mMatAnim;  // 2dc
	short mPathfindID;               // 2e0
	u8 mHasPath;                     // 2e2
	u8 mIsBackupRoute;               // 2e3
	PathNode* mPathNode;             // 2e4
	WayPoint* mGoalWP;               // 2e8
	WayPoint* mStartWP;              // 2ec
	WalkSmokeEffect::Mgr mWalkSmoke; // 2f0
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType)
	    : EnemyMgrBase(objLimit, modelType)
	{
		mName = "DororoMgr";
	}

	virtual void doAlloc() { init(new Parms); }                     // _A8
	virtual void createObj(int count) { mObj = new Obj[count]; }    // _A0
	virtual EnemyBase* getEnemy(int index) { return &mObj[index]; } // _A4
	virtual SysShape::Model* createModel();                         // _B0
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()              // _AC (weak)
	{
		return EnemyTypeID::EnemyID_Dororo;
	}
	virtual void loadTexData();                     // _D0
	virtual J3DModelData* doLoadBmd(void* filename) // _D4 (weak)
	{
		return J3DModelLoaderDataBase::load(filename, 0x20240010);
	}

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Sys::MatTexAnimation* mTexAnimation; // _44
	Obj* mObj;                           // _48, array of Objs
};

struct ProperAnimator : public EnemyAnimatorBase {
	virtual ~ProperAnimator() { }                                   // _08 (weak)
	virtual void setAnimMgr(SysShape::AnimMgr* mgr);                // _0C
	virtual SysShape::Animator& getAnimator() { return mAnimator; } // _10 (weak)
	virtual SysShape::Animator& getAnimator(int idx);               // _14

	// _00 		= VTBL
	// _00-_10	= EnemyAnimatorBase
	SysShape::Animator mAnimator; // _10
};

struct FSM : public EnemyStateMachine {
	virtual void init(EnemyBase* enemy); // _08

	// _00		= VTBL
	// _00-_1C	= EnemyStateMachine
};

/////////////////////////////////////////////////////////////////
// STATE MACHINE DEFINITIONS
enum StateID {
	PROGG_Born  = 0,
	PROGG_Chase = 1,
	PROGG_Dead  = 2,
	PROGG_Flick = 3,
	PROGG_Roar  = 4,
	PROGG_Wait  = 5,
	PROGG_Look  = 6,
	PROGG_Path  = 7,
	PROGG_Count,
};

struct State : public EnemyFSMState {
	inline State(int stateID)
	    : EnemyFSMState(stateID)
	{
	}

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateBorn : public State {
	inline StateBorn()
	    : State(PROGG_Born)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateChase : public State {
	inline StateChase()
	    : State(PROGG_Chase)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};
struct StateDead : public State {
	inline StateDead()
	    : State(PROGG_Dead)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateFlick : public State {
	inline StateFlick()
	    : State(PROGG_Flick)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateRoar : public State {
	inline StateRoar()
	    : State(PROGG_Roar)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateWait : public State {
	inline StateWait()
	    : State(PROGG_Wait)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateLook : public State {
	inline StateLook()
	    : State(PROGG_Look)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StatePath : public State {
	inline StatePath()
	    : State(PROGG_Path)
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _08
	virtual void exec(EnemyBase* enemy);                     // _0C
	virtual void cleanup(EnemyBase* enemy);                  // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

/////////////////////////////////////////////////////////////////
} // namespace Dororo
} // namespace Game

#endif
