#ifndef _GAME_ENTITIES_SMOKYFROG_H
#define _GAME_ENTITIES_SMOKYFROG_H

#include "Game/Entities/Frog.h"

/**
 * --Header for Smokywog (SmokyFrog)--
 */

namespace Game {
namespace SmokyFrog {
struct Obj : public Frog::Obj {
	Obj();

	virtual ~Obj() { }                                 // _1BC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _258 (weak)
	{
		return EnemyTypeID::EnemyID_SmokyFrog;
	}
	virtual void attackNaviPosition(); // _300

	void createEffect();

	// _00 		= VTBL
	// _00-_2E0	= Frog::Obj
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

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj; // _44, array of Objs
};
} // namespace SmokyFrog
} // namespace Game

#endif
