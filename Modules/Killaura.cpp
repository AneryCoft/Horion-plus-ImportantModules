#include "Killaura.h"
#include <windows.h>

Killaura::Killaura() : IModule('P', Category::COMBAT, "Attacks entities around you automatically") {
	mode = (*new SettingEnum(this))
		.addEntry(EnumEntry("Single", 0))
		.addEntry(EnumEntry("Multi", 1))
		.addEntry(EnumEntry("Switch", 2));
	registerEnumSetting("Mode", &mode, 1);
	//this->registerBoolSetting("MultiAura", &this->isMulti, this->isMulti);
	this->registerFloatSetting("range", &this->range, this->range, 3.f, 20.f);
	this->registerIntSetting("APS", &this->APS, this->APS, 1, 20);
	//this->registerIntSetting("Delay", &this->delay, this->delay, 0, 20);
	this->registerIntSetting("MultipleAPS", &this->multipleAPS, this->multipleAPS, 1, 50);
	this->registerBoolSetting("MobAura", &this->isMobAura, this->isMobAura);
	this->registerBoolSetting("hurttime", &this->hurttime, this->hurttime);
	this->registerBoolSetting("AutoWeapon", &this->autoweapon, this->autoweapon);
	this->registerBoolSetting("Strafe", &this->strafe, this->strafe);
	this->registerBoolSetting("Rotations", &this->rotations, this->rotations);
	//this->registerBoolSetting("EaseCation", &this->easecation, this->easecation);
	//this->registerBoolSetting("Visualise Target", &this->VisTarget, this->VisTarget);
}

Killaura::~Killaura() {
}

const char* Killaura::getModuleName() {
	return ("Killaura");
}

static std::vector<C_Entity*> targetList;

void findEntity(C_Entity* currentEntity, bool isRegularEntity) {
	static auto killauraMod = moduleMgr->getModule<Killaura>();
	//if (!g_Data.getLocalPlayer()->isAlive())
	//	return;
	if (killauraMod->isMobAura) {
		if (currentEntity == nullptr)
			return;

		if (currentEntity == g_Data.getLocalPlayer())  // Skip Local player
			return;

		if (!g_Data.getLocalPlayer()->canAttack(currentEntity, false))
			return;

		if (!currentEntity->isAlive())
			return;

		if (currentEntity->timeSinceDeath > 0)
			return;

		if (currentEntity->getNameTag()->getTextLength() <= 1 && currentEntity->getEntityTypeId() == 63)
			return;

		if (currentEntity->width <= 0.01f || currentEntity->height <= 0.01f) // Don't hit this pesky antibot on 2b2e.org
			return;

		if (currentEntity->getEntityTypeId() == 64) // item
			return;
		
		if (currentEntity->getEntityTypeId() == 80)  // Arrows
			return;

		if (currentEntity->getEntityTypeId() == 51) // NPC
			return;

		if (currentEntity->getEntityTypeId() == 69)  // XP
			return;
	}
	else {
		if (!Target::isValidTarget(currentEntity))
			return;
	}

	float dist = (*currentEntity->getPos()).dist(*g_Data.getLocalPlayer()->getPos());

	if (dist < killauraMod->range) {
		targetList.push_back(currentEntity);
	}
}

void Killaura::findWeapon() {
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	float damage = 0;
	int slot = supplies->selectedHotbarSlot;
	for (int n = 0; n < 9; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr) {
			float currentDamage = stack->getAttackingDamageWithEnchants();
			if (currentDamage > damage) {
				damage = currentDamage;
				slot = n;
			}
		}
	}
	supplies->selectedHotbarSlot = slot;
}

decltype(GetTickCount64()) start; //推断类型
bool Counter(double a1) {
	if ((GetTickCount64() - start) >= a1) {
		start = GetTickCount64();
		return true;
	}
	return false;
}

void Killaura::onGetPickRange() {
	C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
	if(!g_Data.isInGame())
		this->setEnabled(false);

	if (!localPlayer->isAlive())
		return;

	//Loop through all our players and retrieve their information
	targetList.clear();

	g_Data.forEachEntity(findEntity);

	if (!targetList.empty()) {
		if (n >= targetList.size() || mode.selected != 2)
			n = 0;

		angle = g_Data.getLocalPlayer()->getPos()->CalcAngle(*targetList[n]->getPos());
		if (strafe) {
			localPlayer->setRot(angle);
		}
		else if (rotations) {
			localPlayer->pitch = angle.x;
			//gm->player->yaw = angle.y;
			localPlayer->bodyYaw = angle.y;
			localPlayer->yawUnused1 = angle.y;
		} //转头

		if (autoweapon) findWeapon();

		if (Counter(1000.f / (float)APS)) {
			if(rotations){
				if (g_Data.getLocalPlayer()->velocity.squaredxzlen() < 0.01) {
					C_MovePlayerPacket p(localPlayer, *localPlayer->getPos());
					g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&p);  // make sure to update rotation if player is standing still
				}
			}	

			// Attack all entitys in targetList
			for (int i = 0; i < multipleAPS; i++) {
				switch (mode.selected) {
				case 0:
					if (!(targetList[n]->damageTime > 1 && hurttime)) {
						if (!moduleMgr->getModule<NoSwing>()->isEnabled())
							localPlayer->swing();
						g_Data.getCGameMode()->attack(targetList[n]);
					}
					break;
				case 1:
					for (auto& i : targetList) {
						if (!(i->damageTime > 1 && hurttime)) {
							if (!moduleMgr->getModule<NoSwing>()->isEnabled())
								localPlayer->swing();
							g_Data.getCGameMode()->attack(i);
						}
					}
					break;
				case 2:
					if (!(targetList[n]->damageTime > 1 && hurttime)) {
						if (!moduleMgr->getModule<NoSwing>()->isEnabled())
							localPlayer->swing();
						g_Data.getCGameMode()->attack(targetList[n]);	
					}
				}
			}
			++n;
		}
	}
}

/*
void Killaura::onTick(C_GameMode* gm) {
	C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
	if (!localPlayer->isAlive())
		return;

	//Loop through all our players and retrieve their information
	targetList.clear();

	g_Data.forEachEntity(findEntity);

	if (!targetList.empty()) {
		vec2_t angle = g_Data.getLocalPlayer()->getPos()->CalcAngle(*targetList[0]->getPos());
		if (rotations) {
			gm->player->pitch = angle.x;
			//gm->player->yaw = angle.y;
			gm->player->bodyYaw = angle.y;
			gm->player->yawUnused1 = angle.y;
		}
		else if (strafe) {
			gm->player->setRot(angle);
		} //转头

		if (autoweapon) findWeapon();

		Odelay++;
		if (Odelay >= delay) {
			if (g_Data.getLocalPlayer()->velocity.squaredxzlen() < 0.01) {
				C_MovePlayerPacket p(localPlayer, *localPlayer->getPos());
				g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&p);  // make sure to update rotation if player is standing still
			}

			// Attack all entitys in targetList
			for (int n = 0; n < multipleAPS; n++) {
				if (isMulti) {
					for (auto& i : targetList) {
						if (!(i->damageTime > 1 && hurttime)) {
							if (!moduleMgr->getModule<NoSwing>()->isEnabled())
								localPlayer->swing();
							g_Data.getCGameMode()->attack(i);
						}
					}
				}
				else {
					if (!(targetList[0]->damageTime > 1 && hurttime)) {
						if (!moduleMgr->getModule<NoSwing>()->isEnabled())
							localPlayer->swing();
						g_Data.getCGameMode()->attack(targetList[0]);
					}
				}
			}
		}
	}
}
*/
void Killaura::onSendPacket(C_Packet* packet, bool& cancelSend) {
	/*
	if (packet->isInstanceOf<LevelSoundEventPacket>()) {
		auto pk = reinterpret_cast<LevelSoundEventPacket*>(packet);
		g_Data.getClientInstance()->getGuiData()->displayClientMessageF("%s sound=%i", packet->getName()->getText(), pk->sound);
		return;
	} //临时采集sound值
	*/
	if (!targetList.empty()) {
		if (packet->isInstanceOf<C_MovePlayerPacket>() && rotations && !strafe) {
			auto* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
			movePacket->pitch = angle.x;
			movePacket->headYaw = angle.y;
			movePacket->yaw = angle.y;
		}
		/*
		if (packet->isInstanceOf<PlayerAuthInputPacket>() && rotations && !strafe) {
			PlayerAuthInputPacket* authInputPacket = reinterpret_cast<PlayerAuthInputPacket*>(packet);
			authInputPacket->pitch = angle.x;
			authInputPacket->yaw = angle.y;
			authInputPacket->yawUnused = angle.y;
		}
		*/
		if (strcmp(g_Data.getRakNetInstance()->serverIp.getText(), "ntest.easecation.net") == 0) {
			if (packet->isInstanceOf<LevelSoundEventPacket>()) {
				LevelSoundEventPacket* soundEventPacket = reinterpret_cast<LevelSoundEventPacket*>(packet);
				if (soundEventPacket->sound == 43) //sound 42是空挥手时的数值 也会被计算进CPS 但是攻击的时候不发那个包
					soundEventPacket->sound = 0;
			} //绕过EaseCation服务器CPS检测
		}
	}
}

void Killaura::onEnable() {
	if (g_Data.getLocalPlayer() == nullptr) {
		this->setEnabled(false);
		return;
	}

	if (strcmp(g_Data.getRakNetInstance()->serverIp.getText(), "ntest.easecation.net") == 0)
		clientMessageF(u8"检测到您位于EaseCation，已为您自动开启绕过CPS检测");
}