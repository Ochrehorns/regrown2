#include "Dolphin/rand.h"
#include "Game/generalEnemyMgr.h"
#include "Game/Entities/Armor.h"
#include "Game/Entities/Baby.h"
#include "Game/Entities/BigFoot.h"
#include "Game/Entities/BigTreasure.h"
#include "Game/Entities/BlackMan.h"
#include "Game/Entities/BlueChappy.h"
#include "Game/Entities/BlueKochappy.h"
#include "Game/Entities/Bomb.h"
#include "Game/Entities/BombOtakara.h"
#include "Game/Entities/BombSarai.h"
#include "Game/Entities/Catfish.h"
#include "Game/Entities/Chappy.h"
#include "Game/Entities/Damagumo.h"
#include "Game/Entities/DangoMushi.h"
#include "Game/Entities/Demon.h"
#include "Game/Entities/Egg.h"
#include "Game/Entities/ElecBug.h"
#include "Game/Entities/ElecHiba.h"
#include "Game/Entities/ElecOtakara.h"
#include "Game/Entities/Fart.h"
#include "Game/Entities/FireChappy.h"
#include "Game/Entities/FireOtakara.h"
#include "Game/Entities/Frog.h"
#include "Game/Entities/Fuefuki.h"
#include "Game/Entities/GasHiba.h"
#include "Game/Entities/GasOtakara.h"
#include "Game/Entities/Hana.h"
#include "Game/Entities/Hanachirashi.h"
#include "Game/Entities/Hiba.h"
#include "Game/Entities/Houdai.h"
#include "Game/Entities/Imomushi.h"
#include "Game/Entities/Jigumo.h"
#include "Game/Entities/Kabuto.h"
#include "Game/Entities/KingChappy.h"
#include "Game/Entities/Kochappy.h"
#include "Game/Entities/Koganemushi.h"
#include "Game/Entities/KumaKochappy.h"
#include "Game/Entities/Kurage.h"
#include "Game/Entities/LeafChappy.h"
#include "Game/Entities/Mar.h"
#include "Game/Entities/MaroFrog.h"
#include "Game/Entities/MiniHoudai.h"
#include "Game/Entities/Miulin.h"
#include "Game/Entities/Nest.h"
#include "Game/Entities/OniKurage.h"
#include "Game/Entities/OoPanModoki.h"
#include "Game/Entities/PanModoki.h"
#include "Game/Entities/Pelplant.h"
#include "Game/Entities/Pom.h"
#include "Game/Entities/Queen.h"
#include "Game/Entities/Qurione.h"
#include "Game/Entities/Rock.h"
#include "Game/Entities/ShijimiChou.h"
#include "Game/Entities/SnakeCrow.h"
#include "Game/Entities/SnakeWhite.h"
#include "Game/Entities/SnakeWhole.h"
#include "Game/Entities/Sokkuri.h"
#include "Game/Entities/Tadpole.h"
#include "Game/Entities/TamagoMushi.h"
#include "Game/Entities/Tank.h"
#include "Game/Entities/Tobi.h"
#include "Game/Entities/Tyre.h"
#include "Game/Entities/Ujia.h"
#include "Game/Entities/Ujib.h"
#include "Game/Entities/UmiMushi.h"
#include "Game/Entities/WaterOtakara.h"
#include "Game/Entities/Wealthy.h"
#include "Game/Entities/YellowChappy.h"
#include "Game/Entities/YellowKochappy.h"
#include "Game/plantsMgr.h"
#include "Game/gamePlayData.h"
#include "Game/MapMgr.h"
#include "Game/Interaction.h"
#include "LoadResource.h"

#include "Game/Entities/HallowMushi.h"
#include "Game/Entities/SleepyBulbear.h"
#include "Game/Entities/Usuba.h"
#include "Game/Entities/Hydrant.h"
#include "Game/Entities/SmokyFrog.h"
#include "Game/Entities/Clam.h"
#include "Game/Entities/Pearl.h"
#include "Game/Entities/BigEgg.h"
#include "Game/Entities/Progg.h"
#include "Game/Entities/Puddle.h"
#include "Game/Entities/Truth.h"

static const char matchText[] = "enemyBase";

namespace Game {

GeneralEnemyMgr* generalEnemyMgr;
int GeneralEnemyMgr::mCullCount;
int GeneralEnemyMgr::mTotalCount;

/**
 * @note Address: 0x8010BD3C
 * @note Size: 0xFAC
 */
void GeneralEnemyMgr::createEnemyMgr(u8 type, int enemyID, int limit)
{
	// int limit = objLimit;
	EnemyInfoFunc::getEnemyInfo(enemyID, 0xFFFF);
	char* name = getEnemyName(enemyID, 0xFFFF);
	sys->heapStatusStart(name, nullptr);

	EnemyMgrBase* mgr;

	switch (enemyID) {
	case EnemyTypeID::EnemyID_Pelplant:
		mgr = new Pelplant::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Kochappy:
		mgr = new Kochappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BlueKochappy:
		mgr = new BlueKochappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_YellowKochappy:
		mgr = new YellowKochappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Chappy:
		mgr = new Chappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BlueChappy:
		mgr = new BlueChappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_YellowChappy:
		mgr = new YellowChappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Pom:
		mgr = new Pom::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Frog:
		mgr = new Frog::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Kogane:
		mgr = new Koganemushi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Wealthy:
		mgr = new Wealthy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Fart:
		mgr = new Fart::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Qurione:
		mgr = new Qurione::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_MaroFrog:
		mgr = new MaroFrog::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Rock:
		mgr = new Rock::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_UjiA:
		mgr = new Ujia::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_UjiB:
		mgr = new Ujib::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Tobi:
		mgr = new Tobi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Armor:
		mgr = new Armor::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Hiba:
		mgr = new Hiba::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_GasHiba:
		mgr = new GasHiba::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_ElecHiba:
		mgr = new ElecHiba::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Sarai:
		mgr = new Sarai::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Tank:
		mgr = new Ftank::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Wtank:
		mgr = new Wtank::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Ptank:
		mgr = new Ptank::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Catfish:
		mgr = new Catfish::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Tadpole:
		mgr = new Tadpole::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_ElecBug:
		mgr = new ElecBug::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Mar:
		mgr = new Mar::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Queen:
		mgr = new Queen::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Baby:
		mgr = new Baby::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Demon:
		mgr = new Demon::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_FireChappy:
		mgr = new FireChappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_SnakeCrow:
		mgr = new SnakeCrow::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_KumaChappy:
		mgr = new KumaChappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Bomb:
		mgr = new Bomb::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Egg:
		mgr = new Egg::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_PanModoki:
		mgr = new PanModoki::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_OoPanModoki:
		mgr = new OoPanModoki::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_PanHouse:
		mgr = new Nest::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Fuefuki:
		mgr = new Fuefuki::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Tanpopo:
		mgr = new Tanpopo::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Clover:
		mgr = new Clover::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_HikariKinoko:
		mgr = new HikariKinoko::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Ooinu_s:
		mgr = new Ooinu_s::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_KareOoinu_s:
		mgr = new KareOoinu_s::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Ooinu_l:
		mgr = new Ooinu_l::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_KareOoinu_l:
		mgr = new KareOoinu_l::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Wakame_s:
		mgr = new Wakame_s::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Wakame_l:
		mgr = new Wakame_l::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Tukushi:
		mgr = new Tukushi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Watage:
		mgr = new Watage::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_DaiodoRed:
		mgr = new DiodeRed::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_DaiodoGreen:
		mgr = new DiodeGreen::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Magaret:
		mgr = new Margaret::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Nekojarashi:
		mgr = new Nekojarashi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Chiyogami:
		mgr = new Chiyogami::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Zenmai:
		mgr = new Zenmai::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_KingChappy:
		mgr = new KingChappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Miulin:
		mgr = new Miulin::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Hanachirashi:
		mgr = new Hanachirashi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Damagumo:
		mgr = new Damagumo::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Kurage:
		mgr = new Kurage::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BombSarai:
		mgr = new BombSarai::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_FireOtakara:
		mgr = new FireOtakara::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_WaterOtakara:
		mgr = new WaterOtakara::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_GasOtakara:
		mgr = new GasOtakara::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_ElecOtakara:
		mgr = new ElecOtakara::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BombOtakara:
		mgr = new BombOtakara::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Jigumo:
		mgr = new Jigumo::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Imomushi:
		mgr = new Imomushi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Houdai:
		mgr = new Houdai::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_LeafChappy:
		mgr = new LeafChappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_TamagoMushi:
		if ((gameSystem != nullptr) && (gameSystem->mIsInCave == 0)) {
			limit = 10;
		} else {
			limit = 30;
		}
		mgr = new TamagoMushi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BigFoot:
		mgr = new BigFoot::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_SnakeWhole:
		mgr = new SnakeWhole::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_UmiMushiBase:
		mgr = new UmiMushi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_OniKurage:
		mgr = new OniKurage::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BigTreasure:
		mgr = new BigTreasure::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Kabuto:
		mgr = new GreenKabuto::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Rkabuto:
		mgr = new RedKabuto::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Fkabuto:
		mgr = new FixKabuto::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_KumaKochappy:
		mgr = new KumaKochappy::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_ShijimiChou:
		if ((gameSystem != nullptr) && (gameSystem->mIsInCave == 0)) {
			limit = 10;
		} else {
			limit = 25;
		}
		mgr = new ShijimiChou::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_MiniHoudai:
		mgr = new NormMiniHoudai::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_FminiHoudai:
		mgr = new FixMiniHoudai::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Sokkuri:
		mgr = new Sokkuri::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Hana:
		mgr = new Hana::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BlackMan:
		mgr = new BlackMan::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Tyre:
		mgr = new Tyre::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_DangoMushi:
		mgr = new DangoMushi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_SmokyFrog:
		mgr = new SmokyFrog::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_SleepyBulbear:
		mgr = new SleepyBulbear::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Usuba:
		mgr = new Usuba::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_HallowMushi:
		mgr = new HallowMushi::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Hydrant:
		mgr = new Hydrant::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_SnakeWhite:
		mgr = new SnakeWhite::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Clam:
		mgr = new Clam::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Pearl:
		mgr = new Pearl::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Bkabuto:
		mgr = new BlackKabuto::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_BigEgg:
		mgr = new BigEgg::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Dororo:
		mgr = new Dororo::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Puddle:
		mgr = new Puddle::Mgr(limit, type);
		break;
	case EnemyTypeID::EnemyID_Truth:
		mgr = new Truth::Mgr(limit, type);
		break;
	}

	mgr->alloc();
	mEnemyMgrNode.add(new EnemyMgrNode(enemyID, name, mgr));
	add(mgr);
	sys->heapStatusEnd(name);
}

/**
 * @note Address: 0x8010CCE8
 * @note Size: 0x1BC
 */
GeneralEnemyMgr::GeneralEnemyMgr()
    : _1C(0)
    , mEnemyNumInfo()
    , mHeap(nullptr)
{
	sys->heapStatusStart("GeneralEnemyMgr", nullptr);
	mName = "�G�}�l�[�W��"; // enemy manager

	mEnemyNumInfo.init();

	sys->heapStatusEnd("GeneralEnemyMgr");
	resetEnemyNum();
	mFlags.clear();
	mFlags.set(1);
	mFlags.set(2);
}

/**
 * @note Address: 0x8010CF0C
 * @note Size: 0x78
 */
void GeneralEnemyMgr::killAll()
{
	EnemyKillArg killArg(CKILL_NULL);
	killArg.setFlag(CKILL_Unk29 | CKILL_Unk30 | CKILL_Unk31);

	EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
	for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
		childNode->killAll(&killArg);
	}
}

/**
 * @note Address: 0x8010CFB4
 * @note Size: 0x4C
 */
void GeneralEnemyMgr::setupSoundViewerAndBas()
{
	EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
	for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
		childNode->setupSoundViewerAndBas();
	}
}

/**
 * @note Address: 0x8010D030
 * @note Size: 0x8C
 */
void GeneralEnemyMgr::doAnimation()
{
	mCullCount  = 0;
	mTotalCount = 0;
	sys->mTimers->_start("doaTEKI", true);
	if (mFlags.isSet(1)) {
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->doAnimation();
		}
	}
	sys->mTimers->_stop("doaTEKI");
}

/**
 * @note Address: 0x8010D0EC
 * @note Size: 0x58
 */
void GeneralEnemyMgr::doEntry()
{
	if (mFlags.isSet(2)) {
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->doEntry();
		}
	}
}

/**
 * @note Address: 0x8010D174
 * @note Size: 0x68
 */
void GeneralEnemyMgr::doSetView(int viewportNumber)
{
	if (mFlags.isSet(2)) {
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->doSetView(viewportNumber);
		}
	}
}

/**
 * @note Address: 0x8010D20C
 * @note Size: 0x58
 */
void GeneralEnemyMgr::doViewCalc()
{
	if (mFlags.isSet(2)) {
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->doViewCalc();
		}
	}
}

/**
 * @note Address: 0x8010D294
 * @note Size: 0x68
 */
void GeneralEnemyMgr::doSimulation(f32 constraint)
{
	if (mFlags.isSet(1)) {
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->doSimulation(constraint);
		}
	}
}

/**
 * @note Address: 0x8010D32C
 * @note Size: 0x68
 */
void GeneralEnemyMgr::doDirectDraw(Graphics& gfx)
{
	if (mFlags.isSet(2)) {
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->doDirectDraw(gfx);
		}
	}
}

/**
 * @note Address: 0x8010D3C4
 * @note Size: 0x80
 */
void GeneralEnemyMgr::doSimpleDraw(Viewport* viewport)
{
	if (mFlags.isSet(2)) {
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->doSimpleDraw(viewport);
		}
		mStoneMgr.draw(viewport);
	}
}

/**
 * @note Address: 0x8010D474
 * @note Size: 0x4C
 */
J3DModelData* GeneralEnemyMgr::getJ3DModelData(int idx)
{
	J3DModelData* modelData = nullptr;
	IEnemyMgrBase* base     = getIEnemyMgrBase(idx);
	if (base) {
		modelData = base->getJ3DModelData();
	}

	return modelData;
}

/**
 * @note Address: 0x8010D4C0
 * @note Size: 0xBC
 */
EnemyBase* GeneralEnemyMgr::birth(int enemyID, EnemyBirthArg& birthArg)
{
	EnemyBase* enemy = nullptr;
	int idx          = getEnemyMgrID(enemyID);

	IEnemyMgrBase* base = getIEnemyMgrBase(idx);
	if (base) {
		birthArg.mTypeID = (EnemyTypeID::EEnemyTypeID)enemyID;
		enemy            = base->birth(birthArg);
	}

	return enemy;
}

/**
 * @note Address: 0x8010D57C
 * @note Size: 0x28
 */
char* GeneralEnemyMgr::getEnemyName(int p1, int p2) { return EnemyInfoFunc::getEnemyName(p1, p2); }

/**
 * @note Address: 0x8010D5A4
 * @note Size: 0x28
 */
int GeneralEnemyMgr::getEnemyID(char* name, int p1) { return EnemyInfoFunc::getEnemyID(name, p1); }

/**
 * @note Address: 0x8010D5CC
 * @note Size: 0x2C
 */
IEnemyMgrBase* GeneralEnemyMgr::getIEnemyMgrBase(int enemyID)
{
	IEnemyMgrBase* base = nullptr;

	EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
	for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
		if (childNode->mEnemyID == enemyID) {
			base = childNode->mMgr;
		}
	}

	return base;
}

/**
 * @note Address: 0x8010D5F8
 * @note Size: 0x21C
 */
void GeneralEnemyMgr::allocateEnemys(u8 type, int heapSize)
{
	// for regrowns overworlds, make enemy heap size depend on the map
	if (gameSystem->isStoryMode() && !gameSystem->mIsInCave) {
		switch (gameSystem->mSection->getCurrentCourseInfo()->mCourseIndex) {
		case 0: // valley of repose
		case 1: // awakening wood
			break;
		case 2: // perplexing pool (might need adjustment for later days idk)
			heapSize = 0x1c0800;
			break;
		case 3: // wistful wild
			heapSize = 0x180800;
			break;
		}
	}

	if (heapSize < 0) {
		mHeapSize = 0x00200800;
		if (gameSystem) {
			if (gameSystem->isChallengeMode()) {
				mHeapSize = 0x00177000;
			} else if (gameSystem->isVersusMode()) {
				mHeapSize = 0x001C2000;
			}
		}
	} else {
		mHeapSize = heapSize;
	}

	sys->heapStatusStart("enemyHeap", nullptr);
	JKRHeap* currentHeap = JKRGetCurrentHeap();
	mHeap                = JKRSolidHeap::create(mHeapSize, currentHeap, true);
	mHeap->becomeCurrentHeap();

	mStoneMgr.loadResource();

	P2ASSERTLINE(1844, currentHeap->getHeapType() == 'EXPH');

	LoadResource::ArgAramOnly arg("/enemy/parm/enemyParms.szs");
	arg.mHeap     = currentHeap;
	arg.mAllocDir = JKRDvdRipper::ALLOC_DIR_BOTTOM;

	LoadResource::Node* resourceNode = gLoadResourceMgr->mountArchive(arg);
	gParmArc                         = resourceNode->mArchive;

	sys->heapStatusStart("allocateEnemys", nullptr);
	sys->heapStatusIndividual();

	for (int i = 0; i < gEnemyInfoNum; i++) {
		int enemyNum = getEnemyNum(gEnemyInfo[i].mId, true);
		OSReport("%i test(id)%i %x\n", enemyNum, gEnemyInfo[i].mId, mHeap->getFreeSize());
		if ((gEnemyInfo[i].mFlags & 0x1) && (enemyNum > 0)) {
			createEnemyMgr(type, gEnemyInfo[i].mId, enemyNum);
		}
	}

	sys->heapStatusNormal();
	sys->heapStatusEnd("allocateEnemys");
	currentHeap->becomeCurrentHeap();
	mHeap->mInitFlag = 1;
	delete resourceNode;

	gParmArc = nullptr;
	sys->heapStatusEnd("enemyHeap");
}

/**
 * @note Address: 0x8010D814
 * @note Size: 0x40
 */
void GeneralEnemyMgr::resetEnemyNum() { mEnemyNumInfo.resetEnemyNum(); }

/**
 * @note Address: 0x8010D854
 * @note Size: 0x22C
 */
#pragma dont_inline on
void GeneralEnemyMgr::addEnemyNum(int enemyID, u8 max, GenObjectEnemy* genObj)
{
	if (enemyID != -1) {
		mEnemyNumInfo.addEnemyNum(enemyID, max * getEnemyMember(enemyID, 0xFFFF));

		for (int i = 0; i < max; i++) {
			switch (enemyID) {
				// check if we're dealing with a plant that can spawn spectralids
			case EnemyTypeID::EnemyID_Ooinu_l:
			case EnemyTypeID::EnemyID_Tanpopo:
			case EnemyTypeID::EnemyID_Magaret:
				if (genObj) {
					EnemyPelletInfo pelletInfo;
					pelletInfo = genObj->mPelletInfo;

					if (pelletInfo.mColor == PELCOLOR_SPECTRALID && pelletInfo.mSize == PELSIZE_SPECTRALID) {
						EnemyInfo* info = EnemyInfoFunc::getEnemyInfo(enemyID, 0xFFFF);
						addEnemyNum(info->mChildID, info->mChildNum, nullptr);
					}
				}
				break;

			default:
				EnemyInfo* info = EnemyInfoFunc::getEnemyInfo(enemyID, 0xFFFF);
				addEnemyNum(info->mChildID, info->mChildNum, nullptr);
				// check if we're dealing with crawbster, since we need to handle falling rock and egg spawns
				if (enemyID == EnemyTypeID::EnemyID_DangoMushi) {
					if (getEnemyNum(EnemyTypeID::EnemyID_Egg, true) < 10) {
						addEnemyNum(EnemyTypeID::EnemyID_Egg, 10, nullptr);
					}
					if (getEnemyNum(EnemyTypeID::EnemyID_Rock, true) < 30) {
						addEnemyNum(EnemyTypeID::EnemyID_Rock, 30, nullptr);
					}

					// check if we're dealing with empress, since we need to handle falling rock spawns
				} else if ((enemyID == EnemyTypeID::EnemyID_Queen) && (getEnemyNum(EnemyTypeID::EnemyID_Rock, true) < 10)) {
					addEnemyNum(EnemyTypeID::EnemyID_Rock, 10, nullptr);
				} else if (enemyID == EnemyTypeID::EnemyID_HallowMushi) {
					if (getEnemyNum(EnemyTypeID::EnemyID_Bomb, true) < 30) {
						addEnemyNum(EnemyTypeID::EnemyID_Bomb, 30, nullptr);
					}
					if (getEnemyNum(EnemyTypeID::EnemyID_BombOtakara, true) < 5) {
						addEnemyNum(EnemyTypeID::EnemyID_BombOtakara, 5, nullptr);
					}
				} else if (enemyID == EnemyTypeID::EnemyID_Truth) {
					if (getEnemyNum(EnemyTypeID::EnemyID_Rock, true) < 10) {
						addEnemyNum(EnemyTypeID::EnemyID_Rock, 10, nullptr);
					}
				}
				break;
			}
		}
	}
}
#pragma dont_inline reset
/**
 * @note Address: 0x8010DA80
 * @note Size: 0x170
 */

/*
u8 GeneralEnemyMgr::getEnemyNum(int enemyID, bool doFullCount)
{
    u8 count = 0;

    if (doFullCount) {
        if (mEnemyNumInfo.mEnemyNumList) {
            // ISSUE IN HERE
            count = getEnemyCount(enemyID, getEnemyMgrID(enemyID));
        }
    } else {
        // ISSUE IN HERE
        count = mEnemyNumInfo.getEnemyNumData(enemyID);
    }

    return count;
}
*/

/**
 * @note Address: 0x8010DBF0
 * @note Size: 0xB4
 */
JKRHeap* GeneralEnemyMgr::useHeap()
{
	killAll();

	if (mHeap != 0) {
		mHeap->freeAll();
		mEnemyMgrNode.clearRelations();
	}
	mChild = nullptr;
	return mHeap;
}

/**
 * @note Address: 0x8010DCA4
 * @note Size: 0x38
 */
EnemyMgrBase* GeneralEnemyMgr::getEnemyMgr(int enemyID)
{
	EnemyMgrBase* base = static_cast<EnemyMgrBase*>(getIEnemyMgrBase(enemyID));
	if (base) {
		return base;
	}
	return nullptr;
}

/**
 * @note Address: 0x8010DCDC
 * @note Size: 0x98
 */
void GeneralEnemyMgr::setMovieDraw(bool isEndMovie)
{
	if (!isEndMovie) {
		_1C |= 0x1;
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->startMovie();
		}
	} else {
		_1C &= ~0x1;
		EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
			childNode->endMovie();
		}
	}
}

/**
 * @note Address: 0x8010DDD4
 * @note Size: 0x11C
 */
void GeneralEnemyMgr::prepareDayendEnemies()
{
	EnemyMgrNode* childNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
	for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {

		EnemyInfo* info = EnemyInfoFunc::getEnemyInfo(childNode->mEnemyID, 0xFFFF);

		EnemyMgrNode* otherNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
		for (otherNode; otherNode != nullptr; otherNode = static_cast<EnemyMgrNode*>(otherNode->mNext)) { // ?? not sure why this is here
		}

		if (info->mFlags & 0x10) {
			EnemyKillArg killArg(CKILL_NULL);
			killArg.setFlag(CKILL_Unk29 | CKILL_Unk30 | CKILL_Unk31);
			childNode->killAll(&killArg);
		}
	}

	GeneralMgrIterator<EnemyBase> iterator(this);

	iterator.first();

	while (iterator.mContainer) {
		EnemyBase* enemy     = iterator.getObject();
		enemy->mInPiklopedia = false;
		iterator.next();
	}
}

/**
 * @note Address: 0x8010E4BC
 * @note Size: 0xC84
 */
void GeneralEnemyMgr::createDayendEnemies(Sys::Sphere& sphere)
{
	if (mEnemyMgrNode.getChildCount() != 0) {
		int i = 0;
		while (i < 10) {
			int randIdx = randFloat() * mEnemyMgrNode.getChildCount();

			EnemyMgrNode* startNode  = static_cast<EnemyMgrNode*>(mEnemyMgrNode.getChildAt(randIdx));
			EnemyMgrNode* randomNode = startNode;
			EnemyMgrBase* mgr;
			EnemyBase* enemy;
			do {

				EnemyInfo* randomInfo              = EnemyInfoFunc::getEnemyInfo(randomNode->mEnemyID, 0xFFFF);
				EnemyTypeID::EEnemyTypeID randomID = randomNode->mEnemyID;
				EnemyMgrNode* childNode            = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
				mgr                                = nullptr;

				for (childNode; childNode != nullptr; childNode = static_cast<EnemyMgrNode*>(childNode->mNext)) {
					if (childNode->mEnemyID == randomID) {
						mgr = childNode->mMgr;
					}
				}

				TekiStat::Info* tekiInfo = playData->mTekiStatMgr.getTekiInfo(randomID);
				P2ASSERTLINE(2203, tekiInfo != nullptr);

				if ((randomInfo->mFlags & 0x10) && (tekiInfo->mState.isSet(1))) {
					EnemyBirthArg birthArg;
					birthArg.mIsInPiklopedia = 0;

					u16 infoFlags = randomInfo->mFlags;

					if (infoFlags & 0x20) {
						birthArg.mPosition = sphere.mPosition;

						birthArg.mFaceDir = TAU * randFloat();

						int searchID = randomNode->mEnemyID;
						enemy        = nullptr;
						int mgrID    = getEnemyMgrID(searchID);

						EnemyMgrNode* anotherNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
						EnemyMgrBase* anotherMgr  = nullptr;
						for (anotherNode; anotherNode != nullptr; anotherNode = static_cast<EnemyMgrNode*>(anotherNode->mNext)) {
							if (anotherNode->mEnemyID == mgrID) {
								anotherMgr = anotherNode->mMgr;
							}
						}

						if (anotherMgr) {
							birthArg.mTypeID = (EnemyTypeID::EEnemyTypeID)searchID;
							enemy            = anotherMgr->birth(birthArg);
						}

						if (enemy) {
							enemy->init(nullptr);
							InteractAttack attack(enemy, 0.0f, nullptr);
							enemy->stimulate(attack);
							enemy->movie_begin(false);
						}

						i += 10;
						goto noadd;
						// break;
					} else if (infoFlags & 0x40) {
						f32 randAngle     = TAU * randFloat();
						birthArg.mFaceDir = _angXZ(sphere.mPosition.x, sphere.mPosition.z, birthArg.mPosition.x, birthArg.mPosition.z);

						// From here
						int searchID = randomNode->mEnemyID;
						enemy        = nullptr;
						int mgrID    = getEnemyMgrID(searchID);

						EnemyMgrNode* anotherNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
						EnemyMgrBase* anotherMgr  = nullptr;
						for (anotherNode; anotherNode != nullptr; anotherNode = static_cast<EnemyMgrNode*>(anotherNode->mNext)) {
							if (anotherNode->mEnemyID == mgrID) {
								anotherMgr = anotherNode->mMgr;
							}
						}

						if (anotherMgr) {
							birthArg.mTypeID = (EnemyTypeID::EEnemyTypeID)searchID;
							enemy            = anotherMgr->birth(birthArg);
						}
						// To here is an inline

						if (enemy) {
							Sys::Sphere boundingSphere;
							enemy->getBoundingSphere(boundingSphere);
							f32 radDiff = sphere.mRadius - boundingSphere.mRadius;
							if (radDiff < 0.0f) {
								radDiff = 0.0f;
							}

							Vector3f pos(radDiff * sinf(randAngle) + sphere.mPosition.x, 0.0f,
							             radDiff * cosf(randAngle) + sphere.mPosition.z);
							pos.y = mapMgr->getMinY(pos);
							enemy->setPosition(pos, false);
							enemy->mHomePosition = pos;

							enemy->init(nullptr);
							InteractAttack attack(enemy, 0.0f, nullptr);
							enemy->stimulate(attack);
							enemy->movie_begin(false);
						}

						i += 5;
						goto noadd;
					} else if (infoFlags & 0x80) {
						int maxObj    = mgr->getMaxObjects();
						int randLimit = randInt(7) + 2;

						if (maxObj > randLimit) {
							maxObj = randLimit;
						}

						f32 randAngle = TAU * randFloat();
						f32 increment = TAU / (f32)maxObj;
						for (int j = 0; j < maxObj; j++) {

							rand();
							birthArg.mFaceDir = _angXZ(sphere.mPosition.x, sphere.mPosition.z, birthArg.mPosition.x, birthArg.mPosition.z);
							int searchID      = randomNode->mEnemyID;
							enemy             = nullptr;
							int mgrID         = getEnemyMgrID(searchID);

							EnemyMgrNode* anotherNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
							EnemyMgrBase* anotherMgr  = nullptr;
							for (anotherNode; anotherNode != nullptr; anotherNode = static_cast<EnemyMgrNode*>(anotherNode->mNext)) {
								if (anotherNode->mEnemyID == mgrID) {
									anotherMgr = anotherNode->mMgr;
								}
							}

							if (anotherMgr) {
								birthArg.mTypeID = (EnemyTypeID::EEnemyTypeID)searchID;
								enemy            = anotherMgr->birth(birthArg);
							}

							if (enemy) {
								Sys::Sphere boundingSphere;
								enemy->getBoundingSphere(boundingSphere);
								f32 radDiff = sphere.mRadius - boundingSphere.mRadius;
								if (radDiff < 0.0f) {
									radDiff = 0.0f;
								}

								f32 randomRad = (0.5f * radDiff) * randFloat() + (0.5f * radDiff);

								Vector3f pos(randomRad * sinf(randAngle) + sphere.mPosition.x, 0.0f,
								             randomRad * cosf(randAngle) + sphere.mPosition.z);
								pos.y = mapMgr->getMinY(pos);
								enemy->setPosition(pos, false);
								enemy->mHomePosition = pos;

								enemy->init(nullptr);
								InteractAttack attack(enemy, 0.0f, nullptr);
								enemy->stimulate(attack);
								enemy->movie_begin(false);
							}
							randAngle += increment;
						}

						i += 3;
						goto noadd;
					} else {
						int maxObj    = mgr->getMaxObjects();
						int randLimit = randInt(7) + 7;

						if (maxObj > randLimit) {
							maxObj = randLimit;
						}

						f32 randAngle = TAU * randFloat();
						f32 increment = TAU / (f32)maxObj;
						for (int j = 0; j < maxObj; j++) {

							rand();

							birthArg.mFaceDir = _angXZ(sphere.mPosition.x, sphere.mPosition.z, birthArg.mPosition.x, birthArg.mPosition.z);

							int searchID = randomNode->mEnemyID;
							enemy        = nullptr;
							int mgrID    = getEnemyMgrID(searchID);

							EnemyMgrNode* anotherNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
							EnemyMgrBase* anotherMgr  = nullptr;
							for (anotherNode; anotherNode != nullptr; anotherNode = static_cast<EnemyMgrNode*>(anotherNode->mNext)) {
								if (anotherNode->mEnemyID == mgrID) {
									anotherMgr = anotherNode->mMgr;
								}
							}

							if (anotherMgr) {
								birthArg.mTypeID = (EnemyTypeID::EEnemyTypeID)searchID;
								enemy            = anotherMgr->birth(birthArg);
							}

							if (enemy) {
								Sys::Sphere boundingSphere;
								enemy->getBoundingSphere(boundingSphere);
								f32 radDiff = sphere.mRadius - boundingSphere.mRadius;
								if (radDiff < 0.0f) {
									radDiff = 0.0f;
								}

								f32 randomRad = (0.5f * radDiff) * randFloat() + (0.5f * radDiff);

								Vector3f pos(randomRad * sinf(randAngle) + sphere.mPosition.x, 0.0f,
								             randomRad * cosf(randAngle) + sphere.mPosition.z);
								pos.y = mapMgr->getMinY(pos);
								enemy->setPosition(pos, false);
								enemy->mHomePosition = pos;

								enemy->init(nullptr);
								InteractAttack attack(enemy, 0.0f, nullptr);
								enemy->stimulate(attack);
								enemy->movie_begin(false);
							}

							randAngle += increment;
						}

						i += 1;
						goto noadd;
					}
				} else {
					randomNode = static_cast<EnemyMgrNode*>(randomNode->mNext);
					if (randomNode == nullptr) {
						randomNode = static_cast<EnemyMgrNode*>(mEnemyMgrNode.mChild);
					}
				}
			} while (randomNode != startNode);
			i += 10;
		noadd:;
		}
	}
}

/**
 * @note Address: N/A
 * @note Size: 0x5C
 */
void GeneralEnemyMgr::setParmsDebugNameAndID()
{
	// more than this but this is the essential bit for weak function ordering
	mEnemyMgrNode.setDebugParm(0);
}

/**
 * @note Address: N/A
 * @note Size: 0x50
 */
void GeneralEnemyMgr::resetParmsDebugNameAndID()
{
	// more than this but this is the essential bit for weak function ordering
	mEnemyMgrNode.resetDebugParm(0);
}
} // namespace Game
