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
#include "Game/CameraMgr.h"
#include "Game/Interaction.h"
#include "Game/AIConstants.h"
#include "Game/EnemyBase.h"
#include "Game/Entities/ItemPikihead.h"
#include "Game/Entities/ItemHoney.h"
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
		_18       = blowArg->_00;
		_24       = blowArg->_0C;
		mIsLethal = blowArg->mIsLethal;
		_2A       = blowArg->_12;
		_14       = blowArg->_14;
	}

	mIsWhistled = false;
	piki->startMotion(IPikiAnims::JHIT, IPikiAnims::JHIT, piki, nullptr);
	mState = 0;

	piki->mSimVelocity.y = _18.y * (0.1f * randFloat() + 1.0f);
	piki->mFaceDir       = roundAng(JMath::atanTable_.atan2_(_18.x, _18.z) + PI);

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
} // namespace Game
