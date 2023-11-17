#ifndef _GAME_ENTITIES_SleepyBulbear_H
#define _GAME_ENTITIES_SleepyBulbear_H

#include "Game/EnemyMgrBase.h"
#include "Game/EnemyBase.h"
#include "Game/Entities/ChappyBase.h"

/**
 * --Header for Sleepy Bulbear (SleepyBulbear)--
 */

namespace Game {
namespace SleepyBulbear {
struct Obj : public ChappyBase::Obj {
	Obj();

	//////////////// VTABLE
	virtual ~Obj() { }                               // _1BC (weak)
	virtual void changeMaterial() { }                // _200 (weak)
	virtual f32 getDownSmokeScale() { return 1.0f; } // _2EC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()
	{
		return EnemyTypeID::EnemyID_SleepyBulbear;
	} // _258 (weak)
	  //////////////// VTABLE END

	// _00 		= VTBL
	// _00-_2E4	= ChappyBase::Obj
	// _2E4 = PelletView
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	//////////////// VTABLE
	// virtual ~Mgr() { }                                  // _58 (weak)
	virtual void createObj(int);                       // _A0
	virtual EnemyBase* getEnemy(int idx);              // _A4
	virtual void doAlloc();                            // _A8
	virtual void loadModelData();                      // _C8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _AC (weak)
	{
		return EnemyTypeID::EnemyID_SleepyBulbear;
	}
	//////////////// VTABLE END

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj; // _44, an array of Objs
};

} // namespace SleepyBulbear
} // namespace Game

#endif
