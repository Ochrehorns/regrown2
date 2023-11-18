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

struct Parms : public EnemyParmsBase {
	struct ProperParms : public Parameters {
		inline ProperParms()
		    : Parameters(nullptr, "FrogParms")
		    , mAirTime(this, 'fp01', "空中時間", 1.5f, 0.0f, 5.0f)            // 'air time'
		    , mJumpSpeed(this, 'fp02', "ジャンプ速度", 400.0f, 0.0f, 1000.0f) // 'jump speed'
		    , mJumpFailChance(this, 'fp03', "失敗確率", 0.2f, 0.0f, 1.0f)     // 'probability of failure'
		    , mFallSpeed(this, 'fp04', "落下初速度", 300.0f, 0.0f, 500.0f)    // 'initial fall velocity'
		    , mScaleMult(this, 'fp05', "frog scale", 5.0f, 0.01f, 50.0f)     // 'frog scale'
		{
		}

		Parm<f32> mAirTime;        // _804, fp01
		Parm<f32> mJumpSpeed;      // _82C, fp02
		Parm<f32> mJumpFailChance; // _854, fp03
		Parm<f32> mFallSpeed;      // _87C, fp04
		Parm<f32> mScaleMult;      // _8A4, fp05
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

} // namespace SmokyFrog
} // namespace Game

#endif
