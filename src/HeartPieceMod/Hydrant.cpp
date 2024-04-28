#include "Game/Entities/Hydrant.h"
#include "Game/Entities/Houdai.h"

namespace Game {
namespace Hydrant {

Obj::Obj() { }

void Obj::doInteractBubble(Creature* target)
{
	if (target->isPiki() && static_cast<Piki*>(target)->isPikmin()) {
		InteractBubble bubble(this, static_cast<Houdai::Parms*>(mParms)->mGeneral.mAttackDamage.mValue);
		target->stimulate(bubble);
		return;
	} else if (target->isNavi()) {
		InteractFlick bubble(this, 0.0f, static_cast<Houdai::Parms*>(mParms)->mGeneral.mAttackDamage.mValue, 0.0f);
		target->stimulate(bubble);
		return;
	}

	if (target->isTeki() && target != this) {
		InteractBubble bubble(this, 500.0f);
		target->stimulate(bubble);
	}
}

} // namespace Hydrant

// WaterHitParticle__4GameFPQ23efx5TBaseRQ23efx3ArgPQ34Game6Houdai17HoudaiShotGunNode
void WaterHitParticle(efx::TBase* original, efx::Arg& arg, Houdai::HoudaiShotGunNode* obj)
{
	// OSReport("WaterHitParticle\n");
	if (obj->mOwner->isHydrant()) {
		efx::THydrantHit hit;
		hit.create(&arg);
	} else {
		original->create(&arg);
	}
}

namespace Houdai {

static HoudaiShotGunMgr* sHoudaiShotGunMgr;

HoudaiShotGunMgr::HoudaiShotGunMgr(Obj* houdai)
    : mOwner(houdai)
    , mIsShotGunRotation(false)
    , mIsShotGunLockedOn(false)
    , mIsShotGunFinished(false)
{
	// OSReport("HoudaiShotGunMgr\n");

	mPitch         = 0.0f;
	mYaw           = 0.0f;
	mActiveNodes   = new HoudaiShotGunNode(mOwner);
	mInactiveNodes = new HoudaiShotGunNode(mOwner);

	for (int i = 0; i < 10; i++) {
		HoudaiShotGunNode* node = new HoudaiShotGunNode(mOwner);
		if (houdai->isHydrant()) {
			// OSReport("HydantShell\n");
			node->mEfxShell = (efx::THdamaShell*)(new efx::THydrantShell);
		} else {
			node->mEfxShell = new efx::THdamaShell;
		}
		node->mPosition = Vector3f::zero;
		node->mVelocity = Vector3f::zero;

		mInactiveNodes->add(node);
	}

	mEfxSight = new efx::THdamaSight();

	sHoudaiShotGunMgr = nullptr;
}

void Obj::createShotGun() { mShotGunMgr = new HoudaiShotGunMgr(this); }

} // namespace Houdai

} // namespace Game

#include "PikiAI.h"

void PikiAI::ActAttack::calcAttackPos()
{
	bool isLongLegs = false;
	if (mCreature->isTeki()) {
		Game::EnemyBase* enemy = static_cast<Game::EnemyBase*>(mCreature);
		if (enemy->getEnemyTypeID() == Game::EnemyTypeID::EnemyID_Damagumo || enemy->getEnemyTypeID() == Game::EnemyTypeID::EnemyID_BigFoot
		    || enemy->getEnemyTypeID() == Game::EnemyTypeID::EnemyID_Houdai || enemy->getEnemyTypeID() == Game::EnemyTypeID::EnemyID_Hydrant) {
			isLongLegs = true;
		}
	}

	if (isLongLegs) {
		FindCollPartArg findArg;
		FindCondition condition;
		findArg.mCondition = &condition;
		mParent->getBoundingSphere(findArg.mPosition);
		CollPart* part = mCreature->mCollTree->findCollPart(findArg);
		if (part) {
			mAttackSphere.mPosition = part->mPosition;
			mAttackSphere.mRadius   = part->mRadius;
		} else {
			mAttackSphere.mPosition = mCreature->getPosition();
			mAttackSphere.mRadius   = mCreature->getBodyRadius();
		}
	} else {
		mAttackSphere.mPosition = mCreature->getPosition();
		mAttackSphere.mRadius   = mCreature->getBodyRadius();
	}
}
