#ifndef _GAME_ENTITIES_BIGEGG_H
#define _GAME_ENTITIES_BIGEGG_H

#include "Game/EnemyStateMachine.h"
#include "Game/EnemyAnimatorBase.h"
#include "Game/EnemyParmsBase.h"
#include "Game/EnemyMgrBase.h"
#include "Game/EnemyBase.h"
#include "Game/PlatInstance.h"
#include "Collinfo.h"
#include "Platform.h"

namespace Game {
namespace BigEgg {

struct FSM;

struct Obj : public EnemyBase {
	Obj();

	//////////////// VTABLE
	virtual void onInit(CreatureInitArg* settings);                                       // _30
	virtual void doDirectDraw(Graphics& gfx);                                             // _50
	virtual void getShadowParam(ShadowParam& settings);                                   // _134
	virtual ~Obj() { }                                                                    // _1BC (weak)
	virtual void setInitialSetting(EnemyInitialParamBase* params);                        // _1C4
	virtual void doUpdate();                                                              // _1CC
	virtual void doDebugDraw(Graphics& gfx);                                              // _1EC
	virtual bool earthquakeCallBack(Creature* source, f32 bounceFactor) { return false; } // _28C
	virtual f32 getDownSmokeScale() { return 0.8f; }                                      // _2EC (weak)
	virtual void setFSM(FSM* fsm);                                                        // _2F8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()                                    // _258 (weak)
	{
		return EnemyTypeID::EnemyID_BigEgg;
	}
	//////////////// VTABLE END

	EnemyBase* genProgg();

	// _00 		= VTBL
	// _00-_2BC	= EnemyBase
	FSM* mFsm; // _2BC
	bool mDoMakeProgg;
	bool mIsDead;
	f32 mBreakTimer;
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	virtual void doAlloc();                                         // _A8
	virtual void createObj(int count);                              // _A0
	virtual EnemyBase* getEnemy(int index) { return &mObj[index]; } // _A4
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()                   // _AC (weak)
	{
		return EnemyTypeID::EnemyID_BigEgg;
	}

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj; // _44, array of Objs
};

struct Parms : public EnemyParmsBase {
	struct ProperParms : public Parameters {
		ProperParms()
		    : Parameters(nullptr, "EnemyParmsBase")
		    , mWaitTime(this, 'fp01', "Wait Before Break", 1.0f, 10.0f, 20.0f) // 'base factor'

		{
		}

		Parm<f32> mWaitTime; // _804, fp01
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
	EGG_Wait = 0,
	EGG_Count,
};

struct State : public EnemyFSMState {
	inline State(int stateID)
	    : EnemyFSMState(stateID)
	{
	}

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateWait : public State {
	StateWait(int);

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

/////////////////////////////////////////////////////////////////
} // namespace BigEgg
} // namespace Game

#endif
