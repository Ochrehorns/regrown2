#ifndef _GAME_ENTITIES_HYDRANT_H
#define _GAME_ENTITIES_HYDRANT_H

#include "Game/Entities/Houdai.h"

/**
 * --Header for Water MAL (Hydrant)--
 */

namespace Game {
namespace Hydrant {
struct Obj : public Houdai::Obj {
	Obj();

	virtual ~Obj() { }                                 // _1BC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _258 (weak)
	{
		return EnemyTypeID::EnemyID_Hydrant;
	}

	void doInteractBubble(Creature* target);


	// _00 		= VTBL
	// _00-_420	= Houdai::Obj
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType);

	// virtual ~Mgr();                                     // _58 (weak)
	virtual void createObj(int);                       // _A0
	virtual EnemyBase* getEnemy(int idx);              // _A4
	virtual void doAlloc();                            // _A8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _AC (weak)
	{
		return EnemyTypeID::EnemyID_Hydrant;
	}
	virtual void loadModelData();
	virtual J3DModelData* doLoadBmd(void* filename) { return J3DModelLoaderDataBase::load(filename, 0x21240030); }

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj; // _44, array of Objs
};

} // namespace Hydrant
} // namespace Game

#endif
