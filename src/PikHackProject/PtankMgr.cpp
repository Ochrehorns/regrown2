#include "Game/Entities/Tank.h"
#include "LoadResource.h"

namespace Game {
namespace Ptank {

namespace {
static const char* cTankChangeTexName = "/enemy/data/Ptank/gas_butadokkuri_main_s3tc.bti";
} // namespace

Mgr::Mgr(int objLimit, u8 modelType)
    : Tank::Mgr(objLimit, modelType)
{
	mName = "ミズブタドックリマネージャ"; // water pig dog manager
}

void Mgr::doAlloc() { init(new Tank::Parms); }

void Mgr::createObj(int count) { mObj = new Obj[count]; }

EnemyBase* Mgr::getEnemy(int index) { return &mObj[index]; }

void Mgr::loadTexData()
{
	mChangeTexture = nullptr;
	LoadResource::Arg loadArg(cTankChangeTexName);
	LoadResource::Node* resource = gLoadResourceMgr->load(loadArg);
	if (resource) {
		mChangeTexture = static_cast<ResTIMG*>(resource->mFile);
	}
	P2ASSERTLINE(82, mChangeTexture);
}

} // namespace Ptank
} // namespace Game
