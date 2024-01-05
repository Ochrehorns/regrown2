#ifndef _GAME_ENTITIES_HALLOWMUSHI_H
#define _GAME_ENTITIES_HALLOWMUSHI_H

#include "Game/Entities/DangoMushi.h"
#include "efx/THallow.h"

namespace Game {

namespace HallowMushi {
// 15 (time elpased rolling) / 0.3 (trail in-between time)
#define HALLOW_MAX_TRAIL_COUNT   (50)
#define HALLOW_TRAIL_LINGER_TIME (10.0f)
#define TRAIL_RADIUS_SIZE        (15.0f)
#define TRAIL_HEIGHT             (5.0f)

struct Trail {
	void create(Vector3f& position, f32 scale, f32 time);
	void fade();
	void update(EnemyBase*);

	bool mActive;
	f32 mTimer;
	Vector3f mPosition;
	efx::THallow* mEffect;
};

struct Obj : public DangoMushi::Obj {
	Obj();

	//////////////// VTABLE
	virtual ~Obj() { }                                    // _1BC (weak)
	virtual f32 getDownSmokeScale() { return 1.3f; }      // _2EC (weak)
	virtual f32 getDamageCoeStoneState() { return 0.2f; } // _2AC (weak)
	virtual EnemyTypeID::EEnemyTypeID getEnemyTypeID() { return EnemyTypeID::EnemyID_HallowMushi; }
	virtual bool bombCallBack(Creature* source, Vector3f& direction, f32 damage);

	void flickHandCollision(Creature*);
	void setBodyCollision(bool check);
	void createCrashEnemy();
	void updateTrails();
	void createEffect();
	void addTrail(Vector3f& pos);

	virtual void doUpdate();

	virtual void onInit(CreatureInitArg* settings);

	int mTrailCount;
	Trail* mTrailArray;
	f32 mLastTrailTimer;
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
		return EnemyTypeID::EnemyID_HallowMushi;
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

struct Parms : public DangoMushi::Parms {

	struct ProperParms : public Parameters {
		inline ProperParms()
		    : Parameters(nullptr, "EnemyParmsBase")
		    , mTrailLingerTime(this, 'fp11', "lingering trail decay time", 10.0f, 0.3f, 30.0f)
		    , mTrailRadiusSize(this, 'fp12', "lingering trail size", 15.0f, 1.0f, 100.0f)
		    , mTrailHeight(this, 'fp13', "lingering trail interaction height", 10.0f, 0.0f, 360.0f)
		    , mTrailPeriod(this, 'fp14', "lingering trail spawn time peroid", 0.3f, 0.001f, 15.0f)
		    , mFirstTrailDelay(this, 'fp15', "delay before spawning the first trail in a roll", 0.5f, 0.001f, 15.0f)
		{
		}

		Parm<f32> mTrailLingerTime;
		Parm<f32> mTrailRadiusSize;
		Parm<f32> mTrailHeight;
		Parm<f32> mTrailPeriod;
		Parm<f32> mFirstTrailDelay;
	};

	Parms() { }

	virtual void read(Stream& stream) // _08 (weak)
	{
		DangoMushi::Parms::read(stream);
		mProperHallowParms.read(stream);
	}

	// _00-_7F8	= EnemyParmsBase
	ProperParms mProperHallowParms; // _7F8
};

} // namespace HallowMushi

} // namespace Game

#endif