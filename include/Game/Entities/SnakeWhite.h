#ifndef _GAME_ENTITIES_SNAKEWHITE_H
#define _GAME_ENTITIES_SNAKEWHITE_H

#include "Game/Entities/SnakeCrow.h"

/**
 * --Header for Burrowing Snarrow (SNAKEWHITE)--
 */

namespace Game {
namespace SnakeWhite {
struct Obj : public SnakeCrow::Obj {
	virtual ~Obj() { }                                 // _1BC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() // _258 (weak)
	{
		return EnemyTypeID::EnemyID_SnakeWhite;
	}
};

struct Mgr : public EnemyMgrBase {
	Mgr(int objLimit, u8 modelType)
    : EnemyMgrBase(objLimit, modelType)
{
	mName = "SnakeWhiteMgr"; // snake white manager
}

	// virtual ~Mgr();                                     // _58 (weak)
	virtual void createObj(int count) { mObj = new Obj[count]; }    // _A0
	virtual EnemyBase* getEnemy(int index) { return &mObj[index]; } // _A4
	virtual void doAlloc() { init(new SnakeCrow::Parms); }          // _A8
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID()              // _AC (weak)
	{
		return EnemyTypeID::EnemyID_SnakeWhite;
	}
	virtual void loadModelData()
	{
		EnemyMgrBase::loadModelData();
		J3DShape* shape;
		for (u16 j = 0; j < mModelData->getShapeNum(); j++) {
			shape = mModelData->mShapeTable.mItems[j];
			shape->setTexMtxLoadType(0x2000);
		}
	}
	virtual J3DModelData* doLoadBmd(void* filename) // _D4 (weak)
	{
		return J3DModelLoaderDataBase::load(filename, 0x21240030);
	}

	// _00 		= VTBL
	// _00-_44	= EnemyMgrBase
	Obj* mObj; // _44, array of Objs
};
} // namespace SnakeWhite
} // namespace Game

#endif
