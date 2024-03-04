#ifndef _GAME_ENTITIES_PEARL_H
#define _GAME_ENTITIES_PEARL_H

#include "Game/EnemyStateMachine.h"
#include "Game/EnemyAnimatorBase.h"
#include "Game/EnemyParmsBase.h"
#include "Game/EnemyMgrBase.h"
#include "Game/EnemyBase.h"
#include "Game/PlatInstance.h"
#include "Collinfo.h"
#include "Platform.h"
#include "efx/TClam.h"

namespace Game {

namespace Clam {
struct Obj;
} // namespace Clam

namespace Pearl {
struct FSM;

struct Obj : public EnemyBase {
	Obj();

	//////////////// VTABLE
	virtual void onInit(CreatureInitArg* settings);                                       // _30
	virtual void doDirectDraw(Graphics& gfx);                                             // _50
	virtual void bounceCallback(Sys::Triangle* tri);                                      // _E8
	virtual void getShadowParam(ShadowParam& settings);                                   // _134
	virtual ~Obj() { }                                                                    // _1BC (weak)
	virtual void setInitialSetting(EnemyInitialParamBase* params);                        // _1C4
	virtual void doUpdate();                                                              // _1CC
	virtual void doDebugDraw(Graphics& gfx);                                              // _1EC
	virtual void changeMaterial();                                                        // _200
	virtual bool earthquakeCallBack(Creature* source, f32 bounceFactor) { return false; } // _28C
	virtual void startCarcassMotion();                                                    // _2C4
	virtual f32 getDownSmokeScale() { return 0.3f; }                                      // _2EC (weak)
	virtual void setFSM(FSM* fsm);                                                        // _2F8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()                                    // _258 (weak)
	{
		return EnemyTypeID::EnemyID_Pearl;
	}
	virtual bool damageCallBack(Creature* source, f32 damage, CollPart* part);
	//////////////// VTABLE END

	// _00 		= VTBL
	// _00-_2BC	= EnemyBase
	FSM* mFsm; // _2BC
	bool mIsAlive;
	Clam::Obj* mOwnerClam;
	Sys::MatLoopAnimator* mMatAnim;
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	// virtual ~Mgr();                                     // _58 (weak)
	virtual void doAlloc();                            // _A8
	virtual void createObj(int);                       // _A0
	virtual EnemyBase* getEnemy(int idx);              // _A4
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _AC (weak)
	{
		return EnemyTypeID::EnemyID_Pearl;
	}
	virtual SysShape::Model* createModel();         // _B0
	virtual void loadModelData();                   // _C8
	virtual void loadTexData();                     // _D0
	virtual J3DModelData* doLoadBmd(void* filename) // _D4 (weak)
	{
		return J3DModelLoaderDataBase::load(filename, 0x30);
	}

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj;                       // _44, array of Objs
	Sys::MatBaseAnimation* mMatAnim; // _48
};

struct Parms : public EnemyParmsBase {
	struct ProperParms : public Parameters {
		ProperParms()
		    : Parameters(nullptr, "EnemyParmsBase")
		{
		}
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

enum StateID {
	PearlState_Null = -1,

	// Grounded states
	PearlState_Idle = 0,
	PearlState_Dead = 1,

	PearlState_Count = 2,
};

struct FSM : public EnemyStateMachine {
	virtual void init(EnemyBase*); // _08

	// _00		= VTBL
	// _00-_1C	= EnemyStateMachine
};

struct State : public EnemyFSMState {
	inline State(int stateID, char* name)
	    : EnemyFSMState(stateID)
	{
		mName = name;
	}

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateIdle : public State {
	inline StateIdle()
	    : State(PearlState_Idle, "idle")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateDead : public State {
	inline StateDead()
	    : State(PearlState_Dead, "dead")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

/////////////////////////////////////////////////////////////////
} // namespace Pearl
} // namespace Game

#endif
