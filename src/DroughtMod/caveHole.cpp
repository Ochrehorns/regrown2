#include "Game/Entities/ItemCave.h"
#include "Game/gamePlayData.h"

namespace Game {

namespace ItemCave {

#define POST_GAME_CAVE_ID 'l_04'

// moved out of itemCave.s, presumed previously matching
BaseItem* Mgr::generatorBirth(Vector3f& pos, Vector3f& angle, GenItemParm* parm)
{
	GenCaveParm* gparm = static_cast<GenCaveParm*>(parm);
	P2ASSERT(parm);

	// prevents the last cave from spawning before louie has been rescued
	if (gparm->mId == POST_GAME_CAVE_ID && !playData->isStoryFlag(STORY_LouieRescued)) {
		return nullptr;
	}

	Item* item          = static_cast<Item*>(birth());
	item->mCaveFilename = new char[strlen(gparm->mCaveFilename) + 1];
	item->_1E4          = new char[strlen(gparm->_24) + 1];
	strcpy(item->mCaveFilename, gparm->mCaveFilename);
	strcpy(item->_1E4, gparm->_24);
	item->mCaveID.setID(gparm->mId.getID());
	item->init(nullptr);
	item->mFaceDir = angle.y;
	item->setPosition(pos, false);
	item->mFogParm = gparm->mFogParm;
	return item;
}

} // namespace ItemCave

} // namespace Game
