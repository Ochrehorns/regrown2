#include "Game/PikiState.h"
#include "Game/Piki.h"
#include "Game/PikiParms.h"
#include "Game/PikiMgr.h"
#include "Game/Navi.h"
#include "Game/NaviParms.h"
#include "Game/NaviState.h"
#include "Game/MapMgr.h"
#include "Game/rumble.h"
#include "Game/MoviePlayer.h"
#include "Game/DeathMgr.h"
#include "Game/Data.h"
#include "Game/CameraMgr.h"
#include "Game/Interaction.h"
#include "Game/AIConstants.h"
#include "Game/EnemyBase.h"
#include "Game/Entities/ItemPikihead.h"
#include "Game/Entities/ItemHoney.h"
#include "Game/Entities/ItemOnyon.h"
#include "Game/CPlate.h"
#include "efx/TPiku.h"
#include "efx/TPk.h"
#include "efx/TEnemyDive.h"
#include "JSystem/JUtility/JUTException.h"
#include "KandoLib/Choice.h"
#include "PSM/Navi.h"
#include "Dolphin/rand.h"
#include "trig.h"
#include "System.h"
#include "PikiAI.h"

namespace Game {
void PikiBlowState::init(Piki* piki, StateArg* stateArg)
{
	BlowStateArg* blowArg = static_cast<BlowStateArg*>(stateArg);
	if (!blowArg) {
		JUT_PANICLINE(3223, "flick needs PikiBlowInitArg !\n");
	} else {
		mBlowDirection = blowArg->mBlowDirection;
		mChanceToLeaf  = blowArg->mChanceToLeaf;
		mIsLethal      = blowArg->mIsLethal;
		_2A            = blowArg->_12;
		mHeldNavi      = blowArg->mHeldNavi;
	}

	mIsWhistled = false;
	piki->startMotion(IPikiAnims::JHIT, IPikiAnims::JHIT, piki, nullptr);
	mState = 0;

	piki->mSimVelocity.y = mBlowDirection.y * (0.1f * randFloat() + 1.0f);
	piki->mFaceDir       = roundAng(JMath::atanTable_.atan2_(mBlowDirection.x, mBlowDirection.z) + PI);

	if (_2A & 0x4) {
		if ((int)piki->mHappaKind != Leaf) {
			efx::TPkEffect* effectObj = piki->mEffectsObj;
			efx::createSimpleChiru(*effectObj->_0C, effectObj->mPikiColor);
			piki->startSound(PSSE_PK_FLOWER_FALL_VOICE, true);
			piki->mHappaKind--;
		}
	} else {
		piki->startSound(PSSE_PK_VC_SCATTERED, true);
	}

	if (piki->isStickTo()) {
		piki->endStick();
	}
}

void autopluck(NaviWalkState* walkstate, Navi* captain)
// performs Pikmin 3-style autopluck of pikmin seeds
{
	captain->procActionButton();
	walkstate->execAI(captain);
}

bool isBattery(Pellet* pelt) { return !strcmp(pelt->mConfig->mParams.mName.mData, "denchi_1_red"); }

bool PelletGoalState::checkMovie(Pellet* pelt)
{
	bool isGot = false;
	// For treasure, upgrades, and corpses, only check for a cutscene if the pellet was collected for the first time. (only berries and
	// number pellets dont check) This leads to a bug where the first corpse cutscene wont play for enemies youve already collected at an
	// onion above ground
	if (gameSystem->isStoryMode()) {
		isGot = playData->firstCarryPellet(pelt);
	}
	if (gameSystem->isStoryMode() && pelt->getKind() == PELTYPE_TREASURE && gameSystem->mIsInCave) {
		isGot = false; // no common treasure cutscenes in caves
	}
	if (pelt->getKind() == PELTYPE_BERRY) {
		isGot = true;
	}
	if (pelt->getKind() == PELTYPE_NUMBER) {
		isGot = true;
	}

	if (!strcmp(pelt->mConfig->mParams.mName.mData, "key")) {
		if (!gameSystem->isChallengeMode()) {
			sys->getPlayCommonData()->enableChallengeGame();
			sys->mPlayData->mChallengeOpen = true;
		}
	} else {
		if (gameSystem->isChallengeMode()) {
			return false;
		}
	}

	if (!strcmp(pelt->mConfig->mParams.mName.mData, "loozy")) {
		sys->getPlayCommonData()->enableLouieRescue();
		sys->mPlayData->mChallengeOpen = true;
		playData->mStoryFlags |= STORY_LouieRescued;
	}

	bool draw2d = false;
	bool doPlay = false;
	if (isGot) {
		Onyon* onyon = nullptr;
		if ((mOnyon->mObjectTypeID == OBJTYPE_Ufo || mOnyon->mObjectTypeID == OBJTYPE_Onyon)) {
			onyon = static_cast<Onyon*>(mOnyon);
		}

		// The pellet was carried to the ship, if not in a cave, play a cutscene based on pellet type
		if (onyon && onyon->mOnyonType == ONYON_TYPE_SHIP) {
			if (gameSystem->mSection->getCurrentCourseInfo()) {
				// for berries, check if a kind was collected for the first time, or 10 of the berry have been collected
				if (pelt->getKind() == PELTYPE_BERRY) {
					int type = pelt->mPelletColor;
					// Spicy berries.
					if ((int)pelt->mPelletColor == SPRAY_TYPE_SPICY) {
						playData->getDopeFruitCount(type);
						playData->isDemoFlag(DEMO_First_Spicy_Berry);
						playData->isDemoFlag(DEMO_First_Spicy_Spray_Made);
						int dope0 = playData->getDopeFruitCount(type);
						playData->addDopeFruit(type);
						playData->getDopeFruitCount(0);
						playData->getDopeFruitCount(1);
						if (!playData->isDemoFlag(DEMO_First_Spicy_Berry)) {
							playData->setDemoFlag(DEMO_First_Spicy_Berry);
							gameSystem->mSection->setDraw2DCreature(pelt);
							BaseGameSection* section = gameSystem->mSection;
							MoviePlayArg arg("s11_dope_first_r", const_cast<char*>(section->getCurrentCourseInfo()->mName),
							                 section->mMovieFinishCallback, 0);
							moviePlayer->play(arg);
							doPlay = true;

						} else if (!playData->isDemoFlag(DEMO_First_Spicy_Spray_Made)) {
							playData->getDopeFruitCount(type);
							if (dope0 + 1 >= _aiConstants->mDopeCount.mData) {
								playData->setDemoFlag(DEMO_First_Spicy_Spray_Made);
								BaseItem* item = ItemHoney::mgr->birth();
								ItemHoney::InitArg arg(1, 1);
								item->init(&arg);
								Vector3f pos(0.0f, FLOAT_DIST_MAX, 0.0f);
								item->setPosition(pos, false);
								item->movie_begin(false);
								draw2d = true;
								gameSystem->mSection->setDraw2DCreature(item);

								BaseGameSection* section = gameSystem->mSection;
								MoviePlayArg moviearg("s11_dopebin_first_r", const_cast<char*>(section->getCurrentCourseInfo()->mName),
								                      section->mMovieFinishCallback, 0);
								moviePlayer->play(moviearg);
								doPlay = true;
							}
						}

						// Bitter berries.
					} else {
						playData->getDopeFruitCount(type);
						playData->isDemoFlag(DEMO_First_Bitter_Berry);
						playData->isDemoFlag(DEMO_First_Bitter_Spray_Made);
						int dope0 = playData->getDopeFruitCount(type);
						playData->addDopeFruit(type);
						playData->getDopeFruitCount(0);
						playData->getDopeFruitCount(1);
						if (!playData->isDemoFlag(DEMO_First_Bitter_Berry)) {
							playData->setDemoFlag(DEMO_First_Bitter_Berry);
							gameSystem->mSection->setDraw2DCreature(pelt);
							BaseGameSection* section = gameSystem->mSection;
							MoviePlayArg arg("s11_dope_first_b", const_cast<char*>(section->getCurrentCourseInfo()->mName),
							                 section->mMovieFinishCallback, 0);
							moviePlayer->play(arg);
							doPlay = true;
						} else if (!playData->isDemoFlag(DEMO_First_Bitter_Spray_Made)) {
							playData->getDopeFruitCount(type);
							if (dope0 + 1 >= _aiConstants->mDopeCount.mData) {
								playData->setDemoFlag(DEMO_First_Bitter_Spray_Made);
								BaseItem* item = ItemHoney::mgr->birth();
								ItemHoney::InitArg arg(2, 1);
								item->init(&arg);
								Vector3f pos(0.0f, FLOAT_DIST_MAX, 0.0f);
								item->setPosition(pos, false);
								item->movie_begin(false);
								draw2d = true;
								gameSystem->mSection->setDraw2DCreature(item);

								BaseGameSection* section = gameSystem->mSection;
								MoviePlayArg moviearg("s11_dopebin_first_b", const_cast<char*>(section->getCurrentCourseInfo()->mName),
								                      section->mMovieFinishCallback, 0);
								moviePlayer->play(moviearg);
								doPlay = true;
							}
						}
					}

				} else if (pelt->getKind() == PELTYPE_TREASURE) {
					// Treasure carried to the ship (assume above ground)
					gameSystem->mSection->setDraw2DCreature(pelt);
					BaseGameSection* section = gameSystem->mSection;
					MoviePlayArg moviearg("s10_suck_treasure", const_cast<char*>(section->getCurrentCourseInfo()->mName),
					                      section->mMovieFinishCallback, 0);
					moviearg.mPelletName = pelt->mConfig->mParams.mName.mData;
					moviePlayer->play(moviearg);
					doPlay = true;

				} else if (pelt->getKind() == PELTYPE_UPGRADE) {
					// Upgrade carried to the ship (this only appears with the globe in AW normally)
					// strangely, upgrades with an ID of 8 or more use a different theme
					gameSystem->mSection->setDraw2DCreature(pelt);
					BaseGameSection* section = gameSystem->mSection;
					MoviePlayArg moviearg("s17_suck_equipment", const_cast<char*>(section->getCurrentCourseInfo()->mName),
					                      section->mMovieFinishCallback, 0);
					moviearg.mPelletName = pelt->mConfig->mParams.mName.mData;
					moviearg.mStreamID   = 0xc001100b;
					if (pelt->mConfig->mParams.mIndex >= 8) {
						moviearg.mStreamID = 0xc001100a;
					}
					moviePlayer->play(moviearg);
					doPlay = true;
				} else {
					// A completely unused cutscene, in theory this would play if you carried a corpse or number pellet to the ship
					// but even if you did, this cutscene doesnt exist in the files, so nothing happens
					BaseGameSection* section = gameSystem->mSection;
					MoviePlayArg moviearg("suck_ufo", const_cast<char*>(section->getCurrentCourseInfo()->mName),
					                      section->mMovieFinishCallback, 0);
					moviePlayer->play(moviearg);
					doPlay = true;
				}
			}
		} else if (onyon && onyon->mOnyonType == ONYON_TYPE_POD) {
			if (pelt->getKind() == PELTYPE_TREASURE) {
				// Treasure carried to the cave pod
				gameSystem->mSection->setDraw2DCreature(pelt);
				MoviePlayArg moviearg("s22_cv_suck_treasure", nullptr, gameSystem->mSection->mMovieFinishCallback, 0);
				moviearg.mOrigin        = mOnyon->getPosition();
				moviearg.mAngle         = mOnyon->getFaceDir();
				moviearg.mDelegateStart = gameSystem->mSection->mMovieStartCallback;
				moviearg.mDelegateEnd   = gameSystem->mSection->mMovieFinishCallback;
				moviearg.mPelletName    = pelt->mConfig->mParams.mName.mData;
				moviePlayer->play(moviearg);
				doPlay = true;
			} else if (pelt->getKind() == PELTYPE_UPGRADE) {
				// Upgrade carried to the cave pod
				gameSystem->mSection->setDraw2DCreature(pelt);
				BaseGameSection* section = gameSystem->mSection;
				MoviePlayArg moviearg("s22_cv_suck_equipment", nullptr, section->mMovieFinishCallback, 0);
				moviearg.mPelletName    = pelt->mConfig->mParams.mName.mData;
				moviearg.mDelegateStart = section->mMovieStartCallback;
				moviearg.mOrigin        = mOnyon->getPosition();
				moviearg.mAngle         = mOnyon->getFaceDir();
				moviearg.mStreamID      = 0xc001100b;
				if (pelt->mConfig->mParams.mIndex >= 8) {
					moviearg.mStreamID = 0xc001100a;
				}
				moviePlayer->play(moviearg);
				doPlay = true;
			} else if (pelt->getKind() == PELTYPE_CARCASS && pelt->mPelletFlag != Pellet::FLAG_NAVI_NAPSACK
			           && !playData->isDemoFlag(DEMO_First_Corpse_In_Cave)) {
				// first corpse collected in cave
				playData->setDemoFlag(DEMO_First_Corpse_In_Cave);
				BaseGameSection* section = gameSystem->mSection;
				MoviePlayArg moviearg("x08_cv_suck_carcass", nullptr, section->mMovieFinishCallback, 0);
				moviearg.mPelletName    = pelt->mConfig->mParams.mName.mData;
				moviearg.mDelegateStart = section->mMovieStartCallback;
				moviearg.mOrigin        = mOnyon->getPosition();
				moviearg.mAngle         = mOnyon->getFaceDir();
				moviePlayer->play(moviearg);
				doPlay = true;
			}
		} else if (onyon && onyon->mOnyonType <= ONYON_TYPE_YELLOW) {
			if (pelt->getKind() == PELTYPE_NUMBER && !playData->isDemoFlag(DEMO_First_Number_Pellet)) {
				playData->setDemoFlag(DEMO_First_Number_Pellet);
				BaseGameSection* section = gameSystem->mSection;
				MoviePlayArg moviearg("x18_exp_pellet", nullptr, section->mMovieFinishCallback, 0);
				moviearg.mPelletName    = pelt->mConfig->mParams.mName.mData;
				moviearg.mDelegateStart = section->mMovieStartCallback;
				moviearg.mOrigin        = mOnyon->getPosition();
				moviearg.mAngle         = mOnyon->getFaceDir();
				moviePlayer->play(moviearg);
				doPlay = true;
			}
		}
	}

	if (doPlay) {
		Pellet* pelt2 = nullptr;
		if (pelt->getKind() == PELTYPE_CARCASS) {
			pelt->mPelletView->mCreature->movie_begin(false);
		} else if (pelt->getKind() == PELTYPE_NUMBER) {
			pelt->movie_begin(false);
		} else {
			pelt2 = pelt;
			pelt->getCreatureName();
			pelt->getCreatureID();
			if (pelt->mPelletView) {
				pelt->mPelletView->viewGetShape();
			}
		}
		if (!draw2d) {
			gameSystem->mSection->setDraw2DCreature(pelt2);
		}
		pelt->movie_begin(false);
	}
	return doPlay;
}

} // namespace Game
