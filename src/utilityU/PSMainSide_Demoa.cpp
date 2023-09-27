#include "PSM/Demo.h"
#include "SoundID.h"
#include "utilityU.h"
#include "PSM/Scene.h"
#include "PSSystem/PSGame.h"
#include "PSM/ObjCalc.h"

static inline bool streq(const char* a, const char* b)
{
	if (a == nullptr || b == nullptr)
		return false;
	if (strcmp(a, b) == 0)
		return true;
	else
		return false;
}

namespace PSM {

inline DemoInitiator::DemoInitiator(u8* pByte, const char* pName, u32& rAST_ID, char* pBuffer)
    : mAST_ID(rAST_ID)
{
	P2ASSERTLINE(256, pName);

	mName   = pName;
	mAST_ID = rAST_ID;
	b2      = pBuffer;
	mByte   = pByte;

	mSongChosen = false;
}

inline void DemoInitiator::setDefaultValues()
{
	P2ASSERTLINE(41, isInitialized());

	mAST_ID = -1;
	strcpy(b2, "");
	*mByte = -1;
}

inline bool DemoInitiator::isInitialized() { return mByte && mName; }

/*
 * --INFO--
 * Address:	........
 * Size:	000074
 */
void DemoInitiator::equalSetStream(const char* buffer, u32 AST_ID, u8 byte)
{
	mSongChosen = false;
	bool check  = false;
	if (mByte && mName) {
		check = true;
	}
	P2ASSERTLINE(41, check);

	mAST_ID = AST_ID;
	strcpy((char*)buffer, "");
	*mByte = byte;
}

/*
 * --INFO--
 * Address:	........
 * Size:	00004C
 */

inline void DemoInitiator::setDefault(const char* comp, u32 id, bool extraCondition)
{
	if (extraCondition && !mSongChosen && strcmp(mName, comp) == 0) {
		mSongChosen = true;
		mAST_ID     = id;
		*mByte      = -1;
		// OSReport("demo_arg is value: %lu\n", demo_arg);
	}
}

/*
 * --INFO--
 * Address:	80463FE8
 * Size:	00005C
 */
Demo::Demo()
{
	_18               = 0;
	mDoStartWithAudio = 1;
	mSoundID          = (SoundID)-1;
	mSystemSE         = (SoundID)-1;
	mFuncptr          = nullptr;
};

/*
 * --INFO--
 * Address:	80464384
 * Size:	001DA4
 */
PSSystem::BgmSeq* Demo::initiate(DemoArg demoArg, u8* unk)
{
	PSSystem::BgmSeq* seq;
	u32 AST_ID;
	char buffer[32];
	JAInter::SoundInfo audio_info = { 0x00000000, 0x7F030000, 0x3F800000, 0x3C000000 };

	seq       = nullptr;
	buffer[0] = '\0';
	AST_ID    = -1;

	DemoInitiator init = DemoInitiator(unk, demoArg.mName, AST_ID, buffer);
	init.setDefaultValues();

	init.setDefault("s02_dayend_result", 0xc0011012);
	init.setDefault("s01_dayend", demoArg.mBgmID);
	init.setDefault("s03_orimadown", 0xc0011005);
	init.setDefault("s05_pikminzero", 0xc0011006);
	init.setDefault("s11_dope_first_b", 0xc0011001);
	init.setDefault("s11_dope_first_r", 0xc0011001);
	init.setDefault("s11_dopebin_first_b", 0xc0011008);
	init.setDefault("s11_dopebin_first_r", 0xc0011008);

	if (!streq("s01_dayend", init.mName)) {
		bool is_key = demoArg.mName && streq(demoArg.mName, "key");

		init.setDefault("s17_suck_equipment", demoArg.mBgmID, !is_key);
		init.setDefault("s17_suck_equipment", 0xc0011007, is_key);

		init.setDefault("s22_cv_suck_equipment", demoArg.mBgmID, !is_key);
		init.setDefault("s22_cv_suck_equipment", 0xc0011007, is_key);

		init.setDefault("s10_suck_treasure", 0xc0011001, !is_key);
		init.setDefault("s10_suck_treasure", 0xc0011007, is_key);

		init.setDefault("s22_cv_suck_treasure", 0xc0011007, is_key);
		init.setDefault("s22_cv_suck_treasure", 0xc0011001, !is_key);
	}

	init.setDefault("s04_dayend_orimadown", 0xc001101d);
	init.setDefault("s06_dayend_pikminzero", 0xc001101d);

	init.setDefault("g1A_red_doping", 0xc0011009);
	init.setDefault("g1A_red_doping", 0xc0011009);
	init.setDefault("g1B_black_doping", 0xc0011047);
	init.setDefault("g02_boot_onyonR", 0xc001100c);
	init.setDefault("g1E_boot_onyonY", 0xc001100c);
	init.setDefault("g20_boot_onyonB", 0xc001100c);
	init.setDefault("g03_meet_redpikmin", 0xc001100d);
	init.setDefault("g1F_meet_yellowpikmin", 0xc001100d);
	init.setDefault("g21_meet_bluepikmin", 0xc001100d);
	init.setDefault("g24_meet_blackpikmin", 0xc001100d);
	init.setDefault("g27_meet_whitepikmin", 0xc001100d);
	init.setDefault("g33_camera_demo", 0xc0011024);

	if (PSMGetGameSceneA()->isCave()) {
		init.setDefault("s09_holein", 0xc0011046);
	} else {
		init.setDefault("s09_holein", 0xc0011026);
	}

	init.setDefault("s16_find_item_00", 0xc0011027);
	init.setDefault("s16_find_item_01", 0xc0011027);
	init.setDefault("s16_find_item_02", 0xc0011027);
	init.setDefault("s16_find_item_03", 0xc0011027);
	init.setDefault("s16_find_item_04", 0xc0011027);
	init.setDefault("s16_find_item_05", 0xc0011027);
	init.setDefault("s16_find_item_06", 0xc0011027);
	init.setDefault("s16_find_item_07", 0xc0011027);
	init.setDefault("s16_find_item_08", 0xc0011027);
	init.setDefault("s16_find_item_09", 0xc0011027);
	init.setDefault("s16_find_item_10", 0xc0011027);
	init.setDefault("s16_find_item_11", 0xc0011027);
	init.setDefault("s16_find_item_12", 0xc0011027);

	init.setDefault("s0C_cv_escape", 0xc0011028);

	init.setDefault("g01_pick_me", 0xc0011029);

	init.setDefault("g04_find_treasure", 0xc0011027);

	init.setDefault("g05_find_cave_f_01", 0xc0011029);
	init.setDefault("g05_find_cave_f_02", 0xc0011029);
	init.setDefault("g05_find_cave_f_03", 0xc0011029);
	init.setDefault("g05_find_cave_f_04", 0xc0011029);
	init.setDefault("g05_find_cave_l_01", 0xc0011029);
	init.setDefault("g05_find_cave_l_02", 0xc0011029);
	init.setDefault("g05_find_cave_l_03", 0xc0011029);
	init.setDefault("g05_find_cave_l_04", 0xc0011029);
	init.setDefault("g05_find_cave_t_01", 0xc0011029);
	init.setDefault("g05_find_cave_t_02", 0xc0011029);
	init.setDefault("g05_find_cave_t_03", 0xc0011029);
	init.setDefault("g05_find_cave_t_04", 0xc0011029);
	init.setDefault("g05_find_cave_y_01", 0xc0011029);
	init.setDefault("g05_find_cave_y_02", 0xc0011029);
	init.setDefault("g05_find_cave_y_03", 0xc0011029);
	init.setDefault("g05_find_cave_y_04", 0xc0011029);

	init.setDefault("g0A_cv_find_hole", 0xc0011029);
	init.setDefault("g0B_cv_find_fountain", 0xc0011029);

	init.setDefault("g18_find_gate", 0xc001102a);
	init.setDefault("g19_find_rock", 0xc0011029);
	init.setDefault("g2D_red_extract", 0xc0011027);
	init.setDefault("g2E_black_extract", 0xc0011027);
	init.setDefault("g34_yellow_extract", 0xc001102b);
	init.setDefault("g37_get_louie", 0xc0011027);
	init.setDefault("g38_find_whitepom", 0xc0011029);
	init.setDefault("g39_find_blackpom", 0xc0011029);

	init.setDefault("x01_gamestart", 0xc001102c);
	init.setDefault("x02_watch_red_pikmin", 0xc001102d);
	init.setDefault("x06_join", 0xc001102e);
	init.setDefault("x12_drain_water", 0xc001102a);
	init.setDefault("x10_find_yellow_onyon", 0xc001102f);
	init.setDefault("x11_find_blue_onyon", 0xc0011030);
	init.setDefault("x16_hiba", 0xc0011031);
	init.setDefault("x13_exp_leafchappy", 0xc001102a);
	init.setDefault("x20_blackman", 0xc0011032);

	init.setDefault("s13_pikmin_supply", 0xc0011033);
	init.setDefault("s0B_cv_coursein", 0xc0011036);
	init.setDefault("s0E_return_cave", 0xc0011037);

	init.setDefault("e00_E3_cavestart", 0xc0011039);

	init.setDefault("g07_cv_gamestart", 0xc001103a);
	init.setDefault("g08_first_return", 0xc001103b);
	init.setDefault("g09_first_sunset", 0xc0011033);
	init.setDefault("g16_100_pikmin", 0xc0011033);
	init.setDefault("g16_95_pikmin", 0xc0011033);
	init.setDefault("g26_inout_black", 0xc001104a);
	init.setDefault("g29_inout_white", 0xc0011048);
	init.setDefault("g2B_white_poison", 0xc0011031);
	init.setDefault("g2C_inout_red", 0xc0011033);
	init.setDefault("g36_find_louie", 0xc0011031);

	init.setDefault("x01_coursein_forest", 0xc0011034);
	init.setDefault("x01_coursein_yakushima", 0xc0011034);
	init.setDefault("x01_coursein_last", 0xc0011034);
	init.setDefault("x04_exp_y", 0xc0011049);
	init.setDefault("x05_louiestart", 0xc0011035);
	init.setDefault("x07_first_recovery", 0xc0011049);
	init.setDefault("x08_cv_suck_carcass", 0xc0011033);
	init.setDefault("x09_exp_detector", 0xc0011033);
	init.setDefault("x14_white_dig", 0xc0011033);

	init.setDefault("g32_get_map", 0xc0011048);
	init.setDefault("g35_president_gamestart", 0xc0011033);

	init.setDefault("x15_exp_x", 0xc0011033);
	init.setDefault("x17_join_guide", 0xc0011033);
	init.setDefault("x18_exp_pellet", 0xc0011033);

	init.setDefault("x03_find_red_onyon", 0xc001103c);

	if (streq("s02_dayend_result", init.mName)) {
		audio_info.volume.v2 -= 0xf;
	}
	// clang-format off
  if (streq("s10_suck_treasure",        init.mName) || 
      streq("s11_dope_first_b",         init.mName) || 
      streq("s11_dope_first_r",         init.mName) || 
      streq("s11_dopebin_first_b",      init.mName) || 
      streq("s11_dopebin_first_r",      init.mName) || 
      streq("s17_suck_equipment",       init.mName) || 
      streq("s22_cv_suck_equipment",    init.mName) || 
      streq("s22_cv_suck_treasure",     init.mName) || 
      streq("x19_vs_bedama",            init.mName))
	// clang-format on
	{
		mDoStartWithAudio = false;
		mSoundID          = PSSE_EV_HOME_PELLET_BACUUM;
	} else if (streq("g2F_appear_hole", init.mName)) { // change BGM in challenge mode when hole/fountain appear
		mSoundID = PSSE_SY_WORK_FINISH;
		mFuncptr = PSChangeBgm_ChallengeGame;
	} else if (streq("g30_appear_fountain", init.mName)) {
		mFuncptr = PSChangeBgm_ChallengeGame;
	} else if (streq("x20_blackman", init.mName) || streq("x03_find_red_onyon", init.mName)) {
		mDoStartWithAudio = false;
	} else if (streq("s01_dayend", init.mName)) {
		switch ((u32)demoArg.mName) {
		case 1:
			mSoundID = PSSE_PL_DAYEND_KAISAN_LUI;
			break;
		case 2:
			mSoundID = PSSE_PL_DAYEND_KAISAN_SHA;
			break;
		default:
		case 0:
			mSoundID = PSSE_PL_DAYEND_KAISAN_ORI;
			break;
		}
	}

	if (streq("x20_blackman", init.mName)) {
		mSoundID = PSSE_EN_TIRE_FALL;
	}

	if (AST_ID != -1) {
		PSGame::PikSceneMgr* scene_mgr = (PSGame::PikSceneMgr*)PSSystem::getSceneMgr();
		PSSystem::checkSceneMgr(scene_mgr);

		seq  = scene_mgr->newStreamBgm(AST_ID, audio_info);
		*unk = -1;
	} else if (buffer[0] != '\0') {
		PSGame::PikSceneMgr* scene_mgr = (PSGame::PikSceneMgr*)PSSystem::getSceneMgr();
		PSSystem::checkSceneMgr(scene_mgr);

		seq = scene_mgr->newBgmSeq(buffer, audio_info);
		P2ASSERTLINE(632, seq);
	}
	return seq;
}

} // namespace PSM
