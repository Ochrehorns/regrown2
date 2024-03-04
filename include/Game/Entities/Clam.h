#ifndef _GAME_ENTITIES_CLAM_H
#define _GAME_ENTITIES_CLAM_H

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

namespace Pearl {
struct Obj;
};

namespace Clam {
struct FSM;

struct Obj : public EnemyBase {
	Obj();

	//////////////// VTABLE
	virtual void onInit(CreatureInitArg* settings);                // _30
	virtual void doDirectDraw(Graphics& gfx);                      // _50
	virtual bool isLivingThing() { return false; }                 // _D4 (weak)
	virtual void platCallback(PlatEvent& event);                   // _F0 (weak)
	virtual void getShadowParam(ShadowParam& settings);            // _134
	virtual ~Obj() { }                                             // _1BC (weak)
	virtual void setInitialSetting(EnemyInitialParamBase* params); // _1C4
	virtual void doUpdate();                                       // _1CC
	virtual f32 getDownSmokeScale() { return 1.2f; }               // _2EC (weak)
	virtual void setFSM(FSM* fsm);                                 // _2F8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()             // _258 (weak)
	{
		return EnemyTypeID::EnemyID_Clam;
	}

	virtual bool damageCallBack(Creature* source, f32 damage, CollPart* part) { return false; }
	//////////////// VTABLE END
	void createEffect();
	void birthPearl();

	// _00 		= VTBL
	// _00-_2BC	= EnemyBase
	FSM* mFsm; // _2BC
	PlatInstance* mPlatBottom;
	PlatInstance* mPlatTop;
	f32 mOpenTimer;
	bool mKilledPiki;
	bool mCanEat;
	efx::TClamSparkles* mSparkleEfx;
	Pearl::Obj* mPearl;
	Pellet* mTreasure;
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	// virtual ~Mgr();                                     // _58 (weak)
	virtual void doAlloc();                            // _A8
	virtual void createObj(int);                       // _A0
	virtual EnemyBase* getEnemy(int idx);              // _A4
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _AC (weak)
	{
		return EnemyTypeID::EnemyID_Clam;
	}
	virtual void loadModelData();                   // _C8
	virtual J3DModelData* doLoadBmd(void* filename) // _D4 (weak)
	{
		return J3DModelLoaderDataBase::load(filename, 0x1240030);
	}

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj;             // _44, array of Objs
	Platform* mPlatBottom; // _48
	Platform* mPlatTop;    // _4C
};

struct Parms : public EnemyParmsBase {
	struct ProperParms : public Parameters {
		ProperParms()
		    : Parameters(nullptr, "EnemyParmsBase")
		    , mAttackWaitTime(this, 'fp01', "attack wait", 1.0f, 0.0f, 3.0f)
		    , mPartScale(this, 'fp02', "part scale", 80.0f, 0.0f, 300.0f) // 'grab flight height'
		{
		}

		Parm<f32> mAttackWaitTime; // _804, fp01
		Parm<f32> mPartScale;      // _82C, fp02
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

/////////////////////////////////////////////////////////////////
// STATE MACHINE DEFINITIONS

enum StateID {
	ClamState_Null   = -1,
	ClamState_Wait   = 0,
	ClamState_Attack = 1,
	ClamState_Shut   = 2,
	ClamState_Eat    = 3,
	ClamState_Count,
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

struct StateWait : public State {
	inline StateWait()
	    : State(ClamState_Wait, "wait")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateAttack : public State {
	inline StateAttack()
	    : State(ClamState_Attack, "attack")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateShut : public State {
	inline StateShut()
	    : State(ClamState_Shut, "shut")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateEat : public State {
	inline StateEat()
	    : State(ClamState_Eat, "eat")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

/////////////////////////////////////////////////////////////////
} // namespace Clam
} // namespace Game

#endif
