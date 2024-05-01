#include "TreasureGetScreen.h"
#include "Game/gamePlayData.h"
#include "ebi/E2DGraph.h"
#include "PSSystem/PSSystemIF.h"
#include "PSSystem/PSGame.h"
#include "JSystem/J2D/J2DAnmLoader.h"
#include "og/newScreen/ogUtil.h"

// Global open accessor because effort
void OpenTreasureScreen(u32 treasureID)
{
	if (sTreasureGetInstance) {
		sTreasureGetInstance->open(treasureID);
	}
}

void TreasureGetScreen::onCreate(JKRArchive* arc)
{
	// important panes
	mTextBox  = static_cast<J2DTextBox*>(search('mg_demo_'));
	mShipPic  = static_cast<J2DPicture*>(search('ppod'));
	mRootPane = search('mgnull00');

	// message offset for treasure name from index
	u64 idtags[3] = { '0101_01', '0110_01', '0200_01' };
	mOffsetMgr    = new Morimura::TOffsetMsgSet(idtags, '0100_01', 3);
	mTextBox->setMsgID(mOffsetMgr->getMsgID(0));
	mDoShipSpeech = false;

	// reset screen scale
	mRootPane->updateScale(0.0f, 0.0f);

	ebi::E2DPane_setTreeCallBackMessage(this, this);
	bool payedDebt = Game::playData->isStoryFlag(Game::STORY_DebtPaid);
	// create pod screen, based on movieMessage.cpp version, needs different files for post-debt
	mPodScreen = new P2DScreen::Mgr_tuning;
	mPodScreen->set(payedDebt ? "gpod.blo" : "pod.blo", 0x40000, arc);
	void* file     = arc->getResource(payedDebt ? "anim/gpod.btp" : "anim/pod.btp");
	mAnmTexPattern = static_cast<J2DAnmTexPattern*>(J2DAnmLoaderDataBase::load(file));
	mPodScreen->setAnimation(mAnmTexPattern);

	file      = arc->getResource(payedDebt ? "anim/gpod.bck" : "anim/pod.bck");
	mAnmTrans = static_cast<J2DAnmTransform*>(J2DAnmLoaderDataBase::load(file));
	mPodScreen->setAnimation(mAnmTrans);

	file      = arc->getResource(payedDebt ? "anim/gpod.bpk" : "anim/pod.bpk");
	mAnmColor = static_cast<J2DAnmColor*>(J2DAnmLoaderDataBase::load(file));
	mPodScreen->setAnimation(mAnmColor);

	mAnmColorTimer                = 0.0f;
	mAnmTexPatternTimer           = 0.0f;
	mAnmTransTimer                = 0.0f;
	mAnmColor->mCurrentFrame      = 0.0f;
	mAnmTexPattern->mCurrentFrame = 0.0f;
	mAnmTrans->mCurrentFrame      = 0.0f;

	close();
	sTreasureGetInstance = this;
}

// opens the window, provide the id of the treasure
void TreasureGetScreen::open(u32 treasureID)
{
	PSSystem::spSysIF->playSystemSe(PSSE_MP_SHIP_CALLING_01, 0);
	mCurrentScale = 0.0f;
	mOpenTimer    = 0.0f;
	mTextBox->setMsgID(mOffsetMgr->getMsgID(treasureID));
	mState = 1;
	// play the voice for 20 frames
	mShipSpeechTimer = 20;

	mAnmColorTimer                = 0.0f;
	mAnmTexPatternTimer           = 0.0f;
	mAnmTransTimer                = 0.0f;
	mAnmColor->mCurrentFrame      = 0.0f;
	mAnmTexPattern->mCurrentFrame = 0.0f;
	mAnmTrans->mCurrentFrame      = 0.0f;
}

// force closes the window
void TreasureGetScreen::close()
{
	mCurrentScale = 0.0f;
	mOpenTimer    = 0.0f;
	mState        = 0;

	mAnmColorTimer                = 0.0f;
	mAnmTexPatternTimer           = 0.0f;
	mAnmTransTimer                = 0.0f;
	mAnmColor->mCurrentFrame      = 0.0f;
	mAnmTexPattern->mCurrentFrame = 0.0f;
	mAnmTrans->mCurrentFrame      = 0.0f;
}

// general update function
void TreasureGetScreen::update()
{
	P2DScreen::Mgr::update();

	if (mState && og::newScreen::checkMovieActive()) {
		close();
		return;
	}

	switch (mState) {
	case 0: // inactive
		return;

	case 1: // opening

		if (mCurrentScale < 1.0f) {
			mCurrentScale += sys->mDeltaTime * 5.0f;
		} else {
			mCurrentScale = 1.0f;
			mState        = 2;
			mDoShipSpeech = true;
		}
		break;

	case 2: // opened
		PSSystem::spSysIF->playSystemSe(PSSE_POD_PC, 0);
		if (mOpenTimer < 3.0f) {
			mOpenTimer += sys->mDeltaTime;
		} else {
			mState = 3;
			PSSystem::spSysIF->playSystemSe(PSSE_MP_SHIP_PERIOD_01, 0);
		}
		break;

	case 3: // closing
		mDoShipSpeech = false;
		if (mCurrentScale > 0.0f) {
			mCurrentScale -= sys->mDeltaTime * 5.0f;
		} else {
			mCurrentScale = 0.0f;
			mState        = 0;
		}
		break;
	}

	// update animations for the pod screen
	mAnmTexPatternTimer += 1.0f;
	if (mAnmTexPatternTimer >= mAnmTexPattern->mFrameLength) {
		mAnmTexPatternTimer -= mAnmTexPattern->mFrameLength;
	}
	mAnmTexPattern->mCurrentFrame = mAnmTexPatternTimer;

	mAnmTransTimer += 1.0f;
	if (mAnmTransTimer >= mAnmTrans->mFrameLength) {
		mAnmTransTimer -= mAnmTrans->mFrameLength;
	}
	mAnmTrans->mCurrentFrame = mAnmTransTimer;

	mAnmColorTimer += 1.0f;
	if (mAnmColorTimer >= mAnmColor->mFrameLength) {
		mAnmColorTimer -= mAnmColor->mFrameLength;
	}
	mAnmColor->mCurrentFrame = mAnmColorTimer;
	mPodScreen->update();
	mPodScreen->animation();

	// manage ship voice, taken from ObjKantei
	if (mDoShipSpeech) {
		mShipSpeechTimer--;
		if (mShipSpeechTimer <= 0) {
			mDoShipSpeech        = false;
			PSGame::SeMgr* seMgr = PSSystem::getSeMgrInstance();
			seMgr->playMessageVoice(PSSE_MP_VOX_FOOT_A_UP, false);
		} else {
			PSGame::SeMgr* seMgr = PSSystem::getSeMgrInstance();
			seMgr->playMessageVoice(PSSE_MP_VOX_BODY_MN, false);
		}
	}

	mRootPane->updateScale(0.4f, mCurrentScale * 0.4f);

	// offset/scale the pod after its animation has processed already
	mPodScreen->search('Npod_1')->move(-125.0f, -75.0f);
	mPodScreen->search('Npod_2')->updateScale(1.0f, mCurrentScale);
}

// override so pod screen can draw
void TreasureGetScreen::draw(Graphics& gfx, J2DGrafContext& context)
{
	P2DScreen::Mgr_tuning::draw(gfx, context);

	if (mState != 0) {
		mPodScreen->draw(gfx, context);
	}
}
