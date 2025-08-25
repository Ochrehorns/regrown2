#ifndef _GAME_ENTITIES_TRUTH_H
#define _GAME_ENTITIES_TRUTH_H

#include "Game/EnemyStateMachine.h"
#include "Game/EnemyAnimatorBase.h"
#include "Game/EnemyParmsBase.h"
#include "Game/EnemyMgrBase.h"
#include "Game/EnemyBase.h"
#include "Game/PlatInstance.h"
#include "Game/Entities/Puddle.h"
#include "Collinfo.h"
#include "Platform.h"

namespace Game {
namespace Truth {

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
	virtual f32 getDownSmokeScale() { return 1.4f; }                                      // _2EC (weak)
	virtual void setFSM(FSM* fsm);                                                        // _2F8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()                                    // _258 (weak)
	{
		return EnemyTypeID::EnemyID_Truth;
	}
	virtual bool needShadow() { return false; }
	//////////////// VTABLE END

	// _00 		= VTBL
	// _00-_2BC	= EnemyBase
	FSM* mFsm; // _2BC
	Puddle::Obj* mPuddle;
	f32 mWaitTimer;
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	virtual void doAlloc();                                         // _A8
	virtual void createObj(int count);                              // _A0
	virtual EnemyBase* getEnemy(int index) { return &mObj[index]; } // _A4
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()              // _AC (weak)
	{
		return EnemyTypeID::EnemyID_Truth;
	}
	virtual J3DModelData* doLoadBmd(void* filename) // _D4 (weak)
	{
		return J3DModelLoaderDataBase::load(filename, 0x21020030);
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
	TRUTH_Spawn    = 0,
	TRUTH_Wait     = 1,
	TRUTH_Move     = 2,
	TRUTH_Hide     = 3,
	TRUTH_HideMove = 4,
	TRUTH_Appear   = 5,
	TRUTH_Hurt     = 6,
	TRUTH_Dead     = 7,
	TRUTH_Attack   = 8,
	TRUTH_Roar     = 9,
	TRUTH_Ult      = 10,
	TRUTH_Count,
};

struct State : public EnemyFSMState {
	inline State(int stateID)
	    : EnemyFSMState(stateID)
	{
	}
};

struct StateSpawn : public State {
	StateSpawn(int stateID)
	    : State(stateID)
	{
		mName = "spawn";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateWait : public State {
	StateWait(int stateID)
	    : State(stateID)
	{
		mName = "wait";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateMove : public State {
	StateMove(int stateID)
	    : State(stateID)
	{
		mName = "move";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateHide : public State {
	StateHide(int stateID)
	    : State(stateID)
	{
		mName = "hide";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateHideMove : public State {
	StateHideMove(int stateID)
	    : State(stateID)
	{
		mName = "hidemove";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateAppear : public State {
	StateAppear(int stateID)
	    : State(stateID)
	{
		mName = "appear";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateHurt : public State {
	StateHurt(int stateID)
	    : State(stateID)
	{
		mName = "hurt";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateDead : public State {
	StateDead(int stateID)
	    : State(stateID)
	{
		mName = "dead";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateAttack : public State {
	StateAttack(int stateID)
	    : State(stateID)
	{
		mName = "attack";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateRoar : public State {
	StateRoar(int stateID)
	    : State(stateID)
	{
		mName = "roar";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

struct StateUlt : public State {
	StateUlt(int stateID)
	    : State(stateID)
	{
		mName = "ultimate";
	}

	virtual void init(EnemyBase* enemy, StateArg* settings); // _00
	virtual void exec(EnemyBase* enemy);                     // _04
};

/////////////////////////////////////////////////////////////////
} // namespace Truth
} // namespace Game

#endif
