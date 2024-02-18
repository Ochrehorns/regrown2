#ifndef _GAME_ENTITIES_SMOKYFROG_H
#define _GAME_ENTITIES_SMOKYFROG_H

#include "Game/Entities/MaroFrog.h"

/**
 * --Header for Smokywog (SmokyFrog)--
 */

namespace Game {
namespace SmokyFrog {
struct Obj : public MaroFrog::Obj {
	Obj();

	virtual void onInit(CreatureInitArg* settings);     // _30
	virtual void getShadowParam(ShadowParam& settings); // _134
	virtual void doUpdateCommon();                      // _1D0
	virtual void setParameters();                       // _228
	virtual ~Obj() { }                                  // _1BC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()  // _258 (weak)
	{
		return EnemyTypeID::EnemyID_SmokyFrog;
	}
	virtual void attackNaviPosition(); // _300
	virtual void changeMaterial() { mMatLoopAnimator->animate(30.0f); }

	void createEffect();
	void createGas();
	void interactGasAttack();

	// _00 		= VTBL
	// _00-_2E0	= Frog::Obj
	f32 mAttackTimer; // _2E0
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
} // namespace SmokyFrog
} // namespace Game

#endif
