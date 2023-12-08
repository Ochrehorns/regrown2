#ifndef _GAME_ENTITIES_USUBA_H
#define _GAME_ENTITIES_USUBA_H

#include "Game/EnemyStateMachine.h"
#include "Game/EnemyAnimatorBase.h"
#include "Game/EnemyParmsBase.h"
#include "Game/EnemyMgrBase.h"
#include "Game/EnemyBase.h"
#include "Collinfo.h"

/**
 * --Header for Usuba (Usuba)--
 */

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
	virtual void initMouthSlots();                                 // _22C
	virtual void doStartStoneState();                              // _2A4
	virtual void doFinishStoneState();                             // _2A8
	virtual void startCarcassMotion();                             // _2C4
	virtual f32 getDownSmokeScale() { return 0.65f; }              // _2EC (weak)
	virtual void setFSM(FSM* fsm);                                 // _2F8
	virtual int getStickPikminNum();                               // _2FC
	virtual FakePiki* getAttackableTarget();                       // _300
	virtual int catchTarget();                                     // _304
	virtual void resetAttackableTimer(f32) { }                     // _308 (weak)
	virtual MouthSlots* getMouthSlots() { return &mMouthSlots; }   // _25C (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()             // _258 (weak)
	{
		return EnemyTypeID::EnemyID_Usuba;
	}
	//////////////// VTABLE END

	f32 setHeightVelocity();
	void setRandTarget();
	void fallMeckGround();
	int getCatchTargetNum();
	int getNextStateOnHeight();
	void flickStickTarget();
	void createDownEffect();

	// _00 		= VTBL
	// _00-_2BC	= EnemyBase
	FSM* mFsm;              // _2BC
	f32 _2C0;               // _2C0
	MouthSlots mMouthSlots; // _2C4
	Vector3f mTargetPos;    // _2CC
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
		    , mFp01(this, 'fp01', "通常飛行高さ", 100.0f, 0.0f, 300.0f)    // 'normal flight height'
		    , mFp02(this, 'fp02', "掴み飛行高さ", 80.0f, 0.0f, 300.0f)     // 'grab flight height'
		    , mFp03(this, 'fp03', "状態遷移高さ", 50.0f, 0.0f, 300.0f)     // 'state transition height'
		    , mFp04(this, 'fp04', "通常移動速度", 100.0f, 0.0f, 300.0f)    // 'normal movement speed'
		    , mFp05(this, 'fp05', "掴み移動速度", 75.0f, 0.0f, 300.0f)     // 'grab movement speed'
		    , mFp06(this, 'fp06', "ウェイト時間", 3.0f, 0.0f, 10.0f)       // 'wait time'
		    , mFp11(this, 'fp11', "上昇係数(0)", 1.5f, 0.0f, 5.0f)         // 'climbing factor (0)'
		    , mFp12(this, 'fp12', "上昇係数(5)", 1.0f, 0.0f, 5.0f)         // 'climbing factor (5)'
		    , mFp21(this, 'fp21', "振払確率(1)", 0.1f, 0.0f, 1.0f)         // 'payoff probability (1)'
		    , mFp22(this, 'fp22', "振払確率(5)", 0.7f, 0.0f, 1.0f)         // 'payoff probability (5)'
		    , mFp23(this, 'fp23', "もがき時間", 3.0f, 0.0f, 10.0f)         // 'struggling time'
		    , mFp31(this, 'fp31', "ハント下降係数", 0.3f, 0.0f, 1.0f)      // 'hunt descent factor'
		    , mFp32(this, 'fp32', "ハント後減衰率", 0.95f, 0.0f, 1.0f)     // 'post-hunt decay rate'
		    , mFp41(this, 'fp41', "Fall Meck 速度", 200.0f, 0.0f, 1000.0f) // 'Fall Meck speed'
		{
		}

		Parm<f32> mFp01; // _804
		Parm<f32> mFp02; // _82C
		Parm<f32> mFp03; // _854
		Parm<f32> mFp04; // _87C
		Parm<f32> mFp05; // _8A4
		Parm<f32> mFp06; // _8CC
		Parm<f32> mFp11; // _8F4
		Parm<f32> mFp12; // _91C
		Parm<f32> mFp21; // _944
		Parm<f32> mFp22; // _96C
		Parm<f32> mFp23; // _994
		Parm<f32> mFp31; // _9BC
		Parm<f32> mFp32; // _9E4
		Parm<f32> mFp41; // _A0C
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
	USUBAANIM_Fly          = 0,  // 'fly' - seems similar to recover?
	USUBAANIM_AttackBreath = 1,  // 'attackBreath'
	USUBAANIM_AttackDive   = 2,  // 'attackDive'
	USUBAANIM_Flick        = 3,  // 'attackShake'
	USUBAANIM_Appear       = 4,  // 'entrance'
	USUBAANIM_Dead         = 5,  // 'death'
	USUBAANIM_Carry        = 6,  // 'deathCarry'
	USUBAANIM_Move         = 7,  // 'flyFwd'
	USUBAANIM_Wait         = 8,  // 'idle'
	USUBAANIM_Fall         = 9,  // 'stunFall'
	USUBAANIM_Damage       = 10, // 'stunHurt'
	USUBAANIM_Ground       = 11, // 'stunIdle'
	USUBAANIM_Recover      = 12, // 'stunRecover'
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
	USUBA_StateCount, // 11
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
