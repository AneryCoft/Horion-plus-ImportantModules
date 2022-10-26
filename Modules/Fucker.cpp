#include "Fucker.h"

Fucker::Fucker() : IModule(VK_NUMPAD9, Category::MISC, "Destroys blocks around you") {
	action = (*new SettingEnum(this))
		.addEntry(EnumEntry("Dig", 0))
		.addEntry(EnumEntry("Destroy", 1))
		.addEntry(EnumEntry("Build", 2));
	registerEnumSetting("Action", &action, 0);
	registerIntSetting("Range", &this->range, this->range, 1, 10);
	this->registerFloatSetting("Outline Thickness", &this->thick, this->thick, 0.1f, 0.8f);
	registerBoolSetting("TargetESP", &this->targetEsp, this->targetEsp);
	registerBoolSetting("Rotations", &this->rotations, this->rotations);
	registerBoolSetting("Beds", &this->beds, this->beds);
	registerBoolSetting("Dragon Eggs", &this->eggs, this->eggs);
	registerBoolSetting("Cakes", &this->cakes, this->cakes);
	registerBoolSetting("Chests", &this->chests, this->chests);
	registerBoolSetting("Barrels", &this->barrels, this->barrels);
	//registerBoolSetting("Eat Cakes", &this->eatcakes, this->eatcakes); //ÓÃÓÚMineplexµÄCake Wars
}

Fucker::~Fucker() {
}

const char* Fucker::getModuleName() {
	return ("Fucker");
}

void Fucker::onPostRender(C_MinecraftUIRenderContext* renderCtx) {
	if (g_Data.getLocalPlayer() == nullptr || !GameData::canUseMoveKeys())
		return;

	if (targetEsp) {
		if (bedsAction) {
			DrawUtils::setColor(0.5803f, 0.0901f, 0.0784f, 1);
			DrawUtils::drawBox(blockPos.toVec3t(), vec3_t(blockPos.toVec3t()).add(1), (float)thick / (float)1.f, outline);
		}
		if (eggsAction) {
			DrawUtils::setColor(0.2078f, 0.f, 0.2313f, 1);
			DrawUtils::drawBox(blockPos.toVec3t(), vec3_t(blockPos.toVec3t()).add(1), (float)thick / (float)1.f, outline);
		}
		if (cakesAction) {
			DrawUtils::setColor(0.7882f, 0.3843f, 0.1450f, 1);
			DrawUtils::drawBox(blockPos.toVec3t(), vec3_t(blockPos.toVec3t()).add(1), (float)thick / (float)1.f, outline);
		}
		if (chestsAction) {
			DrawUtils::setColor(0.6823f, 0.4823f, 0.1765f, 1);
			DrawUtils::drawBox(blockPos.toVec3t(), vec3_t(blockPos.toVec3t()).add(1), (float)thick / (float)1.f, outline);
		}
		if (barrelsAction) {
			DrawUtils::setColor(0.5450f, 0.4039f, 0.2353f, 1);
			DrawUtils::drawBox(blockPos.toVec3t(), vec3_t(blockPos.toVec3t()).add(1), (float)thick / (float)1.f, outline);
		}
	}
}

void Fucker::onTick(C_GameMode* gm) {
	if (g_Data.getLocalPlayer() == nullptr)
		return;

	vec3_t* pos = gm->player->getPos();
	for (int x = (int)pos->x - range; x < pos->x + range; x++) {
		for (int z = (int)pos->z - range; z < pos->z + range; z++) {
			for (int y = (int)pos->y - range; y < pos->y + range; y++) {
				blockPos = vec3_ti(x, y, z);
				bedsAction = false;
				eggsAction = false;
				cakesAction = false;
				chestsAction = false;
				barrelsAction = false;
				actionBOOL = false;
				/*
				bool dig = false;
				bool destroy = false;
				bool build = false;
				*/

				int id = gm->player->region->getBlock(blockPos)->toLegacy()->blockId;

				if (id == 26 && this->beds) {
					bedsAction = true;
					actionBOOL = true;
				} // Beds

				if (id == 122 && this->eggs) {
					eggsAction = true;
					actionBOOL = true;
				} // Dragon Eggs

				if (id == 92 && this->cakes) {
					cakesAction = true;
					actionBOOL = true;
				} // Cakes

				if (id == 54 && this->chests) {
					chestsAction = true;
					actionBOOL = true;
				} // Chests

				if (id == 458 && this->barrels) {
					barrelsAction = true;
					actionBOOL = true;
				} // Barrels

				if (actionBOOL) {
					angle = g_Data.getLocalPlayer()->getPos()->CalcAngle(blockPos.toVec3t());

					switch (action.selected) {
					case 0:{
						bool isDestroyedOut = false;
						gm->survivalDestroyBlockHack(blockPos, 1, isDestroyedOut, false);
					}
						return;
					case 1:
						gm->destroyBlock(&blockPos, 0);
						return;
					case 2:
						gm->buildBlock(&blockPos, 0);
						return;
					}
				}

				/*
				bool isDestroyedOut = false;
				if (dig) {
					gm->survivalDestroyBlockHack(blockPos, 1, isDestroyedOut, false);
					//gm->destroyBlock(&blockPos, 0);
					//if (!moduleMgr->getModule<NoSwing>()->isEnabled())
					//	g_Data.getLocalPlayer()->swingArm();
					return;
				}

				if (destroy) {
					gm->destroyBlock(&blockPos, 0);
					return;
				}

				if (build) {
					gm->buildBlock(&blockPos, 0);
					//if (!moduleMgr->getModule<NoSwing>()->isEnabled())
					//	g_Data.getLocalPlayer()->swingArm();
					return;
				}
				*/
			}
		}
	}
}

void Fucker::onSendPacket(C_Packet* packet, bool& cancelSend) {
	if (actionBOOL && rotations) {
		if (packet->isInstanceOf<C_MovePlayerPacket>() && rotations) {
			auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
			movePacket->pitch = angle.x;
			movePacket->headYaw = angle.y;
			movePacket->yaw = angle.y;
		}

		if (packet->isInstanceOf<PlayerAuthInputPacket>() && rotations) {
			PlayerAuthInputPacket* authInputPacket = reinterpret_cast<PlayerAuthInputPacket*>(packet);
			authInputPacket->pitch = angle.x;
			authInputPacket->yaw = angle.y;
			authInputPacket->yawUnused = angle.y;
		}
	}
}

void Fucker::onGetPickRange() {
	if (actionBOOL && rotations) {
		g_Data.getLocalPlayer()->bodyYaw = angle.y;
		g_Data.getLocalPlayer()->yawUnused1 = angle.y;
		g_Data.getLocalPlayer()->pitch = angle.x;
	}
}
