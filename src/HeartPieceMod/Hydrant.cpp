#include "Game/Entities/Hydrant.h"

namespace Game {
namespace Hydrant {

Obj::Obj() { }

void Obj::doInteractBubble(Creature* target)
{
	if (target->isNavi() || (target->isPiki() && static_cast<Piki*>(target)->isPikmin())) {
		InteractBubble bubble(this, static_cast<Houdai::Parms*>(mParms)->mGeneral.mAttackDamage.mValue);
		target->stimulate(bubble);
		return;
	}

	if (target->isTeki() && target != this) {
		InteractBubble bubble(this, 500.0f);
		target->stimulate(bubble);
	}
}

} // namespace Hydrant
} // namespace Game
