#include "Game/IllustratedBook.h"
#include "Game/GameSystem.h"
#include "Game/SingleGame.h"
#include "nans.h"

namespace Game {
namespace IllustratedBook {

static const int unusedIllustratedBookArray[] = { 0, 0, 0 };

#define COMPLEMENT(src, dest, proportion) (proportion) * ((f32)(dest) - (f32)(src)) + (f32)(src)

/*
 * --INFO--
 * Address:	80130B9C
 * Size:	0001D0
 */
ColorSetting::ColorSetting()
    : CNode("êFê›íË")
{
	_18[0][0] = Color4(88, 91, 153, 255);
	_18[0][1] = Color4(64, 64, 64, 255);
	_18[1][0] = Color4(166, 232, 253, 255);
	_18[1][1] = Color4(255, 255, 255, 255);
	_18[2][0] = Color4(166, 232, 253, 255);
	_18[2][1] = Color4(255, 255, 255, 255);
	_18[3][0] = Color4(255, 192, 128, 255);
	_18[3][1] = Color4(255, 255, 143, 255);
	_18[4][0] = Color4(255, 255, 255, 255);
	_18[4][1] = Color4(255, 255, 255, 255);

	_40[0] = Color4(73, 73, 73, 255);
	_40[1] = Color4(8, 8, 8, 255);
	_40[2] = Color4(50, 50, 40, 255);
	_40[3] = Color4(52, 32, 10, 255);
	_40[4] = Color4(32, 32, 10, 255);
}

/*
 * --INFO--
 * Address:	80130D70
 * Size:	000098
 */
void ColorSetting::read(Stream& stream)
{
	for (int i = 0; i < 4; i++) {
		_18[i][0].read(stream);
		_18[i][1].read(stream);
	}

	for (int i = 0; i < 4; i++) {
		_40[i].read(stream);
	}
}

/*
 * --INFO--
 * Address:	801316D4
 * Size:	000148
 */
PositionParms::PositionParms()
    : CNode(mEnemyName)
{
	sprintf(mEnemyName, "èoåªèÍèäñº"); // 'appearance location name'
}

/*
 * --INFO--
 * Address:	8013181C
 * Size:	000050
 */
void PositionParms::read(Stream& stream)
{
	stream.readString(mEnemyName, sizeof(mEnemyName));
	mParms.read(stream);
}

/*
 * --INFO--
 * Address:	........
 * Size:	0000BC
 */
PositionParmsList::PositionParmsList()
    : CNode("èoåªà íuÉäÉXÉg")
{
	for (u32 i = 0; i < 10; i++) {
		add(&mParms[i]);
	}
}

/*
 * --INFO--
 * Address:	801318CC
 * Size:	000068
 */
void PositionParmsList::read(Stream& stream)
{
	for (u32 i = 0; i < 10; i++) {
		mParms[i].read(stream);
	}
}

/*
 * --INFO--
 * Address:	80131934
 * Size:	000020
 */
void CameraParms::read(Stream& stream) { mParms.read(stream); }

/*
 * --INFO--
 * Address:	80131954
 * Size:	000130
 */
EnemyParms::EnemyParms()
    : CNode("")
{
}

/*
 * --INFO--
 * Address:	80131DA8
 * Size:	000064
 */
void EnemyParms::read(Stream& stream)
{
	mGroupID = stream.readByte();
	mParms.read(stream);
	mCameraParms.read(stream);
}

/*
 * --INFO--
 * Address:	........
 * Size:	0000D0
 */
EnemyModeParms::EnemyModeParms(PositionParmsList* list)
    : CNode("ìGê}ä”") // 'enemy encyclopedia'
{
	for (int i = 0; i < EnemyTypeID::EnemyID_COUNT; i++) {
		mEnemyParms[i].mName         = EnemyInfoFunc::getEnemyName(i, 0xFFFF);
		mEnemyParms[i].mPosParmsList = list;
		add(&mEnemyParms[i]);
	}
}

/*
 * --INFO--
 * Address:	80131E6C
 * Size:	000084
 */
void EnemyModeParms::read(Stream& stream)
{
	int max = stream.readInt();

	for (u32 i = 0; i < max; i++) {
		mEnemyParms[i].read(stream);
	}
}

/*
 * --INFO--
 * Address:	80131EF0
 * Size:	000148
 */
ItemParms::ItemParms()
    : CNode("ê›íË") // 'setting'
{
	mIndex = -1;
}

/*
 * --INFO--
 * Address:	80132038
 * Size:	000064
 */
void ItemParms::read(Stream& stream)
{
	mGroupID = stream.readByte();
	mParms.read(stream);
	mCameraParms.read(stream);
}

/*
 * --INFO--
 * Address:	........
 * Size:	000118
 */
ItemModeParms::ItemModeParms(PositionParmsList* list)
    : CNode("Ç®ïÛê}ä”") // 'treasure book'
{
	mItemCount = SingleGame::ZukanState::getMaxPelletID();
	mItemParms = new ItemParms[mItemCount];

	for (int i = 0; i < mItemCount; i++) {
		PelletConfig* config        = SingleGame::ZukanState::getCurrentPelletConfig(i);
		mItemParms[i].mIndex        = i;
		mItemParms[i].mName         = config->mParams.mName.mData;
		mItemParms[i].mPosParmsList = list;
		add(&mItemParms[i]);
	}
}

/*
 * --INFO--
 * Address:	801320FC
 * Size:	000080
 */
void ItemModeParms::read(Stream& stream)
{
	for (int i = 0; i < mItemCount; i++) {
		mItemParms[i].read(stream);
	}
}

/*
 * --INFO--
 * Address:	8013217C
 * Size:	00029C
 */
Parms::Parms()
    : CNode("ê}ä”ê›íË") // 'picture book setting'
    , mEnemyParms(&mPosParmsList)
    , mItemParms(&mPosParmsList)
{
	add(&mColorSetting);
	add(&mPosParmsList);
	add(&mEnemyParms);
	add(&mItemParms);
}

/*
 * --INFO--
 * Address:	801325D0
 * Size:	000090
 */
void Parms::read(Stream& stream)
{
	mColorSetting.read(stream);
	mPosParmsList.read(stream);
	mEnemyParms.read(stream);
	mItemParms.read(stream);
}

/*
 * --INFO--
 * Address:	80132660
 * Size:	0000D0
 */
void Parms::loadFile(JKRArchive* archive)
{
	P2ASSERTLINE(734, archive);
	void* resource = archive->getResource("setting.ini");
	P2ASSERTLINE(736, resource);
	RamStream stream(resource, -1);
	stream.resetPosition(true, true);
	read(stream);
}

// these are from Parms::loadFile(const char* fileName), which is stripped:
static const char unusedLoadFileStr1[] = "load error.[%s]\n";
static const char unusedLoadFileStr2[] = "no fileName";

} // namespace IllustratedBook
} // namespace Game
