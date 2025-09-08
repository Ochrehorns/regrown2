#include "efx/TNaviEffect.h"
#include "Game/gamePlayData.h"

// functions in files that cant be linked

namespace efx {
/**
 * @note Address: 0x803B7E24
 * @note Size: 0xD8
 */
void TNaviEffect::setNaviType(enumNaviType type)
{
	if (type == NAVITYPE_Louie && Game::playData->isStoryFlag(Game::STORY_DebtPaid)) {
		type = NAVITYPE_President;
	}

	switch (type) {
	case NAVITYPE_Olimar:
		mCursor.mContextNum           = WHISTLE_CONTEXT_NUM;
		mCursor.mAngleSpeed           = TCursor::kAngleSpeed;
		mCursor.mOneEmitter.mEffectID = PID_Cursor_Olimar;
		mLight.mNaviType              = 0;
		mLightAct.mNaviType           = 0;
		break;
	case NAVITYPE_Louie:
		mCursor.mContextNum           = WHISTLE_CONTEXT_NUM;
		mCursor.mAngleSpeed           = TCursor::kAngleSpeed;
		mCursor.mOneEmitter.mEffectID = PID_Cursor_Louie;
		mLight.mNaviType              = 1;
		mLightAct.mNaviType           = 1;
		break;
	case NAVITYPE_President:
		mCursor.mContextNum           = WHISTLE_CONTEXT_NUM;
		mCursor.mAngleSpeed           = TCursor::kAngleSpeed;
		mCursor.mOneEmitter.mEffectID = PID_Cursor_President;
		mLight.mNaviType              = 2;
		mLightAct.mNaviType           = 2;
		break;
	case 3:
		mCursor.mContextNum           = WHISTLE_CONTEXT_NUM;
		mCursor.mAngleSpeed           = TCursor::kAngleSpeed;
		mCursor.mOneEmitter.mEffectID = PID_Cursor_Louie;
		mLight.mNaviType              = 1;
		mLightAct.mNaviType           = 1;
		break;
	}
}

/**
 * @note Address: 0x803B6980
 * @note Size: 0x60
 */
bool TOrimaLight::create(Arg* arg)
{
	switch (mNaviType) {
	case 0:
		mItems[0].mEffectID = PID_OrimaLight_Orima_1;
		mItems[1].mEffectID = PID_OrimaLight_Orima_2;
		break;
	case 1:
		mItems[0].mEffectID = PID_OrimaLight_Loozy_1;
		mItems[1].mEffectID = PID_OrimaLight_Loozy_2;
		break;
	case 2:
		mItems[0].mEffectID = 756;
		mItems[1].mEffectID = 753;
		break;
	}
	return TSyncGroup2::create(arg);
}

/**
 * @note Address: 0x803B69E0
 * @note Size: 0x60
 */
bool TOrimaLightAct::create(Arg* arg)
{
	switch (mNaviType) {
	case 0:
		mItems[0].mEffectID = PID_OrimaLightAct_Orima_1;
		mItems[1].mEffectID = PID_OrimaLightAct_Orima_2;
		break;
	case 1:
		mItems[0].mEffectID = PID_OrimaLightAct_Loozy_1;
		mItems[1].mEffectID = PID_OrimaLightAct_Loozy_2;
		break;
	case 2:
		mItems[0].mEffectID = 754;
		mItems[1].mEffectID = 755;
		break;
	}
	return TSyncGroup2::create(arg);
}

}; // namespace efx