#ifndef _GAME_ENTITIES_SMOKYFROG_H
#define _GAME_ENTITIES_SMOKYFROG_H

#include "Game/Entities/MaroFrog.h"

/**
 * --Header for Smokywog (SmokyFrog)--
 */

namespace Game {
namespace SmokyFrog {

#define SMOKYFROG_START_HEIGHT (2000.0f)
#define SMOKYFROG_APPEAR_VEL   (-500.0f)

enum StateID {
	SMOKYFROG_Stay	 = 10,
	SMOKYFROG_Appear = 11,
	SMOKYFROG_StateCount, // 12 
};

struct Obj : public MaroFrog::Obj {
	Obj();

	virtual void constructor();
	virtual void onInit(CreatureInitArg* settings);     // _30
	virtual void getShadowParam(ShadowParam& settings); // _134
	virtual void getLODCylinder(Sys::Cylinder& cylinder); // needed for shadow to draw 
	virtual bool needShadow();
	virtual void doUpdateCommon();                      // _1D0
	virtual void setParameters();                       // _228
	virtual ~Obj() { }                                  // _1BC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()  // _258 (weak)
	{
		return EnemyTypeID::EnemyID_SmokyFrog;
	}
	virtual void attackNaviPosition(); // _300
	virtual void changeMaterial() 
	{ 
		if (isAlive()) mMatLoopAnimator->animate(30.0f); 
	}

	void createEffect();
	void createGas();
	void interactGasAttack();
	bool addShadowScale();

	void resetBossAppearBGM();
	void setBossAppearBGM();
	void startBossAttackBGM();
	void startBossFlickBGM();
	void updateBossBGM();

	// _00 		= VTBL
	// _00-_2E0	= Frog::Obj
	f32 mAttackTimer; // _2E0
	f32 mShadowScale;
	Sys::MatLoopAnimator* mMatLoopAnimator;
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	// virtual ~Mgr();                                     // _58 (weak)
	virtual void createObj(int);                       // _A0
	virtual EnemyBase* getEnemy(int idx);              // _A4
	virtual void doAlloc();                            // _A8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _AC (weak)
	{
		return EnemyTypeID::EnemyID_SmokyFrog;
	}
	virtual SysShape::Model* createModel();
	virtual void loadTexData();

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj; // _44, array of Objs
	Sys::MatTexAnimation* mTexAnimation;
};

/////////////////////////////////////////////////////////////////
// STATE MACHINE DEFINITIONS

struct FSM : public Game::Frog::FSM {
	virtual void init(EnemyBase* enemy);
};

struct StateStay : public Frog::State {
	inline StateStay()
		: State(SMOKYFROG_Stay, "stay")
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* arg);
	virtual void exec(EnemyBase* enemy);
	virtual void cleanup(EnemyBase* enemy);
};

struct StateAppear : public Frog::State {
	inline StateAppear()
		: State(SMOKYFROG_Appear, "appear")
	{
	}

	virtual void init(EnemyBase* enemy, StateArg* arg);
	virtual void exec(EnemyBase* enemy);
	virtual void cleanup(EnemyBase* enemy);
};

} // namespace SmokyFrog
} // namespace Game

#endif
