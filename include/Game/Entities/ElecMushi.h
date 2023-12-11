#ifndef _GAME_ENTITIES_ELECMUSHI_H
#define _GAME_ENTITIES_ELECMUSHI_H

#include "Game/Entities/DangoMushi.h"

/**
 * --Header for Hallowed Crawbster (ElecMushi)--
 */

namespace Game {
namespace ElecMushi {
struct FSM;

struct Obj : public DangoMushi::Obj {
	Obj();

	//////////////// VTABLE
	virtual ~Obj() { }                                    // _1BC (weak)
	virtual f32 getDownSmokeScale() { return 1.3f; }      // _2EC (weak)
	virtual f32 getDamageCoeStoneState() { return 0.2f; } // _2AC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()
	{
		return EnemyTypeID::EnemyID_ElecMushi;
	} // _258 (weak)
	  //////////////// VTABLE END

	// _00 		= VTBL
	// _00-_300	= DangoMushi::Obj
	// _308 = PelletView
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	// virtual ~Mgr();                                     // _58 (weak)
	virtual void createObj(int);                       // _A0
	virtual EnemyBase* getEnemy(int idx);              // _A4
	virtual void doAlloc();                            // _A8
	virtual SysShape::Model* createModel();            // _B0
	virtual void loadModelData();                      // _C8
	virtual void loadTexData();                        // _D0
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _AC (weak)
	{
		return EnemyTypeID::EnemyID_ElecMushi;
	}
	virtual J3DModelData* doLoadBmd(void* filename) // _D4 (weak)
	{
		return J3DModelLoaderDataBase::load(filename, 0x01240030);
	}

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Sys::MatTevRegAnimation* mTevRegAnimation; // _44
	Obj* mObj;                                 // _48, array of Objs
};
} // namespace ElecMushi
} // namespace Game

#endif
