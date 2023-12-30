#ifndef _GAME_ENTITIES_USUBA_H
#define _GAME_ENTITIES_USUBA_H

#include "Game/EnemyStateMachine.h"
#include "Game/EnemyAnimatorBase.h"
#include "Game/EnemyParmsBase.h"
#include "Game/EnemyMgrBase.h"
#include "Game/EnemyBase.h"
#include "efx/TUsuba.h"
#include "Collinfo.h"

/**
 * --Header for Usuba (Usuba)--
 */

#define USUBA_MAX_STICK_PIKI (5)

namespace Game {
namespace Usuba {
struct FSM;

struct Obj : public EnemyBase {
	Obj();

	//////////////// VTABLE
	virtual void onInit(CreatureInitArg* settings);                // _30
	virtual void doDirectDraw(Graphics& gfx);                      // _50
	virtual void inWaterCallback(WaterBox* wb) { }                 // _84 (weak)
	virtual void outWaterCallback() { }                            // _88 (weak)
	virtual void getShadowParam(ShadowParam& settings);            // _134
	virtual ~Obj() { }                                             // _1BC (weak)
	virtual void setInitialSetting(EnemyInitialParamBase* params); // _1C4
	virtual void doUpdate();                                       // _1CC
	virtual void doDebugDraw(Graphics& gfx);                       // _1EC
	virtual void doStartStoneState();                              // _2A4
	virtual void doFinishStoneState();                             // _2A8
	virtual void startCarcassMotion();                             // _2C4
	virtual f32 getDownSmokeScale() { return 0.65f; }              // _2EC (weak)
	virtual void setFSM(FSM* fsm);                                 // _2F8
	virtual int getStickPikminNum();                               // _2FC
	virtual FakePiki* getAttackableTarget();                       // _300
	virtual int catchTarget();                                     // _304
	virtual void resetAttackableTimer(f32) { }                     // _308 (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()             // _258 (weak)
	{
		return EnemyTypeID::EnemyID_Usuba;
	}
	//////////////// VTABLE END

	f32 setHeightVelocity();
	void setRandTarget();
	int getNextStateOnHeight();
	void createDownEffect();
	void createAppearEffect();
	void createEffect();
	void setupEffect();
	void createChargeSE();
	void createDischargeSE();
	void createFireEffect();
	bool attackTargets(bool doAttack);

	// _00 		= VTBL
	// _00-_2BC	= EnemyBase
	FSM* mFsm;              // _2BC
	f32 mStateTimer;        // _2C0
	Vector3f mTargetPos;    // _2C4
	bool mIsBreathingFire;  // _2D0
	efx::TUsubaEffect* mFireEfx; // _2D4
	                        // _2D8 = PelletView
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	// virtual ~Mgr();                                     // _58 (weak)
	virtual void doAlloc();                            // _A8
	virtual void createObj(int);                       // _A0
	virtual EnemyBase* getEnemy(int idx);              // _A4
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _AC (weak)
	{
		return EnemyTypeID::EnemyID_Usuba;
	}

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj; // _44, array of Objs
};

struct Parms : public EnemyParmsBase {
	struct ProperParms : public Parameters {
		ProperParms()
		    : Parameters(nullptr, "EnemyParmsBase")
		    , mFlightHeight(this, 'fp01', "flight height", 100.0f, 0.0f, 300.0f)
		    , mCarryHeight(this, 'fp02', "carry height", 80.0f, 0.0f, 300.0f)     // 'grab flight height'
		    , mTransitHeight(this, 'fp03', "transit height", 50.0f, 0.0f, 300.0f)     // 'state transition height'
		    , mFlySpeed(this, 'fp04', "fly speed", 100.0f, 0.0f, 300.0f)    // 'normal movement speed'
		    , mFp05(this, 'fp05', "掴み移動速度", 75.0f, 0.0f, 300.0f)     // 'grab movement speed'
		    , mWaitTime(this, 'fp06', "wait time", 3.0f, 0.0f, 10.0f)       // 'wait time'
		    , mUnladenClimbFactor(this, 'fp11', "unladen climb factor", 1.5f, 0.0f, 5.0f)         // 'climbing factor (0)'
		    , mMaxLadenClimbFactor(this, 'fp12', "max laden climb factor", 1.0f, 0.0f, 5.0f)         // 'climbing factor (5)'
		    , mFallChanceMinPiki(this, 'fp21', "fall chance (1 piki)", 0.1f, 0.0f, 1.0f)         // 'payoff probability (1)'
		    , mFallChanceMaxPiki(this, 'fp22', "fall chance (max piki)", 0.7f, 0.0f, 1.0f)         // 'payoff probability (5)'
		    , mGroundTime(this, 'fp23', "ground time", 3.0f, 0.0f, 10.0f)         // 'struggling time'
		    , mFp31(this, 'fp31', "ハント下降係数", 0.3f, 0.0f, 1.0f)      // 'hunt descent factor'
		    , mFp32(this, 'fp32', "ハント後減衰率", 0.95f, 0.0f, 1.0f)     // 'post-hunt decay rate'
		    , mFp41(this, 'fp41', "Fall Meck 速度", 200.0f, 0.0f, 1000.0f) // 'Fall Meck speed'
		{
		}

		Parm<f32> mFlightHeight; // _804, fp01
		Parm<f32> mCarryHeight; // _82C, fp02
		Parm<f32> mTransitHeight; // _854, fp03
		Parm<f32> mFlySpeed; // _87C, fp04
		Parm<f32> mFp05; // _8A4, fp05
		Parm<f32> mWaitTime; // _8CC, fp06
		Parm<f32> mUnladenClimbFactor; // _8F4, fp11
		Parm<f32> mMaxLadenClimbFactor; // _91C, fp12
		Parm<f32> mFallChanceMinPiki; // _944, fp21
		Parm<f32> mFallChanceMaxPiki; // _96C, fp22
		Parm<f32> mGroundTime; // _994, fp23
		Parm<f32> mFp31; // _9BC, fp31
		Parm<f32> mFp32; // _9E4, fp32
		Parm<f32> mFp41; // _A0C, fp41
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

enum AnimID {
	USUBAANIM_Damage       = 0,  // 'stunHurt'
	USUBAANIM_Ground       = 1,  // 'stunIdle'
	USUBAANIM_Recover      = 2,  // 'stunRecover'
	USUBAANIM_AttackBreath = 3,  // 'attackBreath'
	USUBAANIM_AttackDive   = 4,  // 'attackDive'
	USUBAANIM_Flick        = 5,  // 'attackShake'
	USUBAANIM_Dead         = 6,  // 'death'
	USUBAANIM_Carry        = 7,  // 'deathCarry'
	USUBAANIM_Appear       = 8,  // 'entrance'
	USUBAANIM_Fly          = 9,  // 'fly'
	USUBAANIM_Move         = 10, // 'flyFwd'
	USUBAANIM_Wait         = 11, // 'idle'
	USUBAANIM_Fall         = 12, // 'stunFall'
	USUBAANIM_AnimCount,         // 13
};

/////////////////////////////////////////////////////////////////
// STATE MACHINE DEFINITIONS

/*
 * Goal States:
 *
 * Dead = dying state
 * Stay = underground state before appearing
 * Appear = emerging from underground
 * Recover = take off from ground to flying
 * Wait = flying in place
 * Move = flying from point to point
 * Fall = from flying to ground
 * Ground = struggling on ground
 * Damage = taking damage on ground
 * AttackBreath = fire-breathing attack
 * AttackDive = diving attack (still necessary?)
 * Flick = shake off pikmin while flying
 *
 */
enum StateID {
	USUBA_NULL = -1,

	// Grounded states
	USUBA_Dead = 0,
	USUBA_Stay,
	USUBA_Appear,
	USUBA_Fall,
	USUBA_Ground,
	USUBA_Damage,

	// Airborne states
	USUBA_Recover,
	USUBA_Flick,
	USUBA_Wait,
	USUBA_Move,
	USUBA_AttackBreath,
	USUBA_AttackDive,
	USUBA_StateCount, // 12
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

struct StateAppear : public State {
	inline StateAppear()
	    : State(USUBA_Appear, "appear")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateAttackBreath : public State {
	inline StateAttackBreath()
	    : State(USUBA_AttackBreath, "attackbreath")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateAttackDive : public State {
	inline StateAttackDive()
	    : State(USUBA_AttackDive, "attackdive")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateDamage : public State {
	inline StateDamage()
	    : State(USUBA_Damage, "damage")
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
	    : State(USUBA_Dead, "dead")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateFall : public State {
	inline StateFall()
	    : State(USUBA_Fall, "fall")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateFlick : public State {
	inline StateFlick()
	    : State(USUBA_Flick, "flick")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateGround : public State {
	inline StateGround()
	    : State(USUBA_Ground, "ground")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateMove : public State {
	inline StateMove()
	    : State(USUBA_Move, "move")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateRecover : public State {
	inline StateRecover()
	    : State(USUBA_Recover, "recover")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateStay : public State {
	inline StateStay()
	    : State(USUBA_Stay, "stay")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};

struct StateWait : public State {
	inline StateWait()
	    : State(USUBA_Wait, "wait")
	{
	}

	virtual void init(EnemyBase*, StateArg*); // _08
	virtual void exec(EnemyBase*);            // _0C
	virtual void cleanup(EnemyBase*);         // _10

	// _00		= VTBL
	// _00-_10 	= EnemyFSMState
};
/////////////////////////////////////////////////////////////////
} // namespace Usuba
} // namespace Game

#endif
