#include "Game/Entities/Tank.h"

namespace Game {
namespace Ptank {

Obj::Obj() { createEffect(); }

void Obj::changeMaterial()
{
	J3DModelData* modelData;
	J3DModel* j3dModel = mModel->mJ3dModel;
	modelData          = j3dModel->mModelData;
	ResTIMG* texture   = C_MGR->getChangeTexture();

	j3dModel->calcMaterial();

	ResTIMG* newTexture;
	J3DTexture* j3dTexture = mModel->mJ3dModel->mModelData->mMaterialTable.mTextures;
	newTexture             = j3dTexture->mRes;

	*newTexture = *texture;
	;

	j3dTexture->setImageOffset((u32)texture, 0);
	j3dTexture->setPaletteOffset((u32)texture, 0);

	for (u16 i = 0; i < modelData->mMaterialTable.mMaterialNum; i++) {
		J3DMatPacket* packet  = &j3dModel->mMatPackets[i];
		j3dSys.mMatPacket     = packet;
		J3DMaterial* material = modelData->mMaterialTable.mMaterials[i];
		material->diff(packet->mShapePacket->mDiffFlag);
	}
}

void Obj::createEffect() { mTankEffect = new efx::TPtankEffect(nullptr); }

void Obj::setupEffect()
{
	efx::TPtankEffect* effect;
	Matrixf* mtx = mJoint->getWorldMatrix();
	effect       = mTankEffect;

	effect->mEfxGas.setMtxptr(mtx->mMatrix.mtxView);
	effect->mEfxGasYodare.mMtx = mtx;
}

void Obj::startEffect()
{
	mTankEffect->mEfxGas.create(nullptr);
	getJAIObject()->startSound(0x5991, 0);
}

void Obj::startYodare()
{
	efx::TPtankEffect* tankEffect = mTankEffect;
	tankEffect->mEfxGas.fade();
	tankEffect->mEfxGasYodare.create(nullptr);
}

void Obj::finishEffect()
{
	efx::TPtankEffect* effect = mTankEffect;
	effect->mEfxGas.fade();
	effect->mEfxGasYodare.fade();
}

void Obj::effectDrawOn()
{
	efx::TPtankEffect* effect = mTankEffect;
	effect->mEfxGas.endDemoDrawOn();
	effect->mEfxGasYodare.endDemoDrawOn();
}

void Obj::effectDrawOff()
{
	efx::TPtankEffect* effect = mTankEffect;
	effect->mEfxGas.startDemoDrawOff();
	effect->mEfxGasYodare.startDemoDrawOff();
}

void Obj::interactCreature(Creature* creature)
{
	InteractGas act(this, static_cast<EnemyParmsBase*>(mParms)->mGeneral.mAttackDamage.mValue);
	creature->stimulate(act);
}

void Obj::stopEffectRadius(f32 radius)
{
	efx::TPtankEffect* effect             = mTankEffect;
	effect->mEfxGas.mParticleCallBack._04 = radius;
}

void Obj::createChargeSE() { getJAIObject()->startSound(0x5992, 0); }

void Obj::createDisChargeSE() {  }

} // namespace Ptank
} // namespace Game
