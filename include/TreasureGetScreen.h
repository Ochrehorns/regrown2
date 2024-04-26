#ifndef _TREASURE_GET_SCREEN_H
#define _TREASURE_GET_SCREEN_H

#include "types.h"
#include "P2DScreen.h"
#include "Morimura/mrUtil.h"

struct TreasureGetScreen;

void OpenTreasureScreen(u32);
static TreasureGetScreen* sTreasureGetInstance;

struct TreasureGetScreen : public P2DScreen::Mgr_tuning {
	TreasureGetScreen() { }

	virtual ~TreasureGetScreen() { sTreasureGetInstance = nullptr; } // _08 (weak)
	virtual void update();                                           // _30
	virtual void draw(Graphics& gfx, J2DGrafContext& context);       // _9C

	void onCreate(JKRArchive*);
	void open(u32);
	void close();

	// _00      = VTBL
	// _00-_148 = P2DScreen::Mgr_tuning
	f32 mOpenTimer;                      // _148
	f32 mCurrentScale;                   // _14C
	J2DTextBox* mTextBox;                // _150
	J2DPane* mRootPane;                  // _154
	J2DPicture* mShipPic;                // _158
	int mState;                          // _15C
	Morimura::TOffsetMsgSet* mOffsetMgr; // _160
	bool mDoShipSpeech;                  // _164
	u8 mShipSpeechTimer;                 // _165

	P2DScreen::Mgr* mPodScreen;
	J2DAnmColor* mAnmColor;           // _168
	f32 mAnmColorTimer;               // _16C
	J2DAnmTransform* mAnmTrans;       // _170
	f32 mAnmTransTimer;               // _174
	J2DAnmTexPattern* mAnmTexPattern; // _178
	f32 mAnmTexPatternTimer;          // _17C
};

#endif
