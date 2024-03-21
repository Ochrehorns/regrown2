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

void Obj::createEffect() { mTankEffect = nullptr; }

void Obj::setupEffect() { }

void Obj::startEffect()
{
	efx::TOtaGas gas;
	efx::Arg arg(mJoint->getWorldMatrix()->getTranslation());
	gas.create(&arg);
}

void Obj::startYodare() { }

void Obj::finishEffect() { }

void Obj::effectDrawOn() { }

void Obj::effectDrawOff() { }

void Obj::interactCreature(Creature* creature)
{
	InteractGas act(this, static_cast<EnemyParmsBase*>(mParms)->mGeneral.mAttackDamage.mValue);
	creature->stimulate(act);
}

void Obj::stopEffectRadius(f32 radius) { }

void Obj::createChargeSE() { getJAIObject()->startSound(PSSE_EN_KURAGE_PRE_VACUUM, 0); }

void Obj::createDisChargeSE() { getJAIObject()->startSound(PSSE_EN_OTAKARA_ATK_GAS, 0); }

} // namespace Ptank
} // namespace Game
