#include "TPAura.h"
#include "../../../Utils/Target.h"

TPAura::TPAura() : IModule(0x0, Category::COMBAT, "TP like fuck so you dont get hit") {
	position = (*new SettingEnum(this))
		.addEntry(EnumEntry("Around", 0))
		.addEntry(EnumEntry("Above", 1))
		.addEntry(EnumEntry("Behind", 2));
	registerEnumSetting("Position", &position, 0);
	this->registerBoolSetting("MobAura", &this->isMobAura, this->isMobAura);
	this->registerBoolSetting("Lerp", &this->lerp, this->lerp);
	this->registerFloatSetting("Range", &this->range, this->range, 1.f, 20.f);
	this->registerIntSetting("TP delay", &this->delay, this->delay, 0, 20);

}

TPAura::~TPAura() {
}

const char* TPAura::getModuleName() {
	return ("TPAura");
}

static std::vector<C_Entity*> targetList;

void findEntity0(C_Entity* currentEntity, bool isRegularEntity) {
	static auto TPMod = moduleMgr->getModule<TPAura>();

	if (TPMod->isMobAura) {
		if (currentEntity == nullptr)
			return;

		if (currentEntity == g_Data.getLocalPlayer())  // Skip Local player
			return;

		if (!g_Data.getLocalPlayer()->canAttack(currentEntity, false))
			return;

		if (!currentEntity->isAlive())
			return;

		//if (currentEntity->timeSinceDeath > 0)
		//	return;

		if (currentEntity->getNameTag()->getTextLength() <= 1 && currentEntity->getEntityTypeId() == 63)
			return;

		if (currentEntity->width <= 0.01f || currentEntity->height <= 0.01f) // Don't hit this pesky antibot on 2b2e.org
			return;

		//if (currentEntity->getEntityTypeId() == 64) // item
		//	return;
		// 
		//if (currentEntity->getEntityTypeId() == 80)  // Arrows
		//	return;

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
	if (dist < TPMod->range) {
		targetList.push_back(currentEntity);
	}
}


void TPAura::onTick(C_GameMode* gm) {
	C_LocalPlayer* player = g_Data.getLocalPlayer();

	if (!player->isAlive())
		return;

	//Loop through all our players and retrieve their information
	targetList.clear();

	g_Data.forEachEntity(findEntity0);

	Odelay++;

	if (!targetList.empty() && Odelay >= delay) {
		vec3_t pos = *targetList[0]->getPos();
		switch (position.selected) {
		case 0:
		{
			pos.y += 2.f;
			if (turn == 0) {
				//pos.y += 2;
				pos.x += -1.f;
				pos.z += 1.f;
			}
			else if (turn == 1) {
				//pos.y += 2.f;
				pos.x += -1.f;
				pos.z += -1.f;
			}
			else if (turn == 2) {
				//pos.y += 2.f;
				pos.x += 1.f;
				pos.z += -1.f;
			}
			else if (turn == 3) {
				//pos.y += 2.f;
				pos.x += 1.f;
				pos.z += 1.f;
				turn = 0;
			}
			++turn;
		}
		break;
		case 1:
		{
			pos.y += 2.f;
		}
		break;
		case 2:
		{
			float yaw = targetList[0]->yaw - 180.f;
			pos.x -= sin(yaw * (PI / 180)) * 2.5f;
			pos.y += 1.f;
			pos.z += cos(yaw * (PI / 180)) * 2.5f;
		}
		}

		if (lerp) {
			float dist2 = gm->player->getPos()->dist(pos);
			player->lerpTo(pos, vec2_t(1, 1), (int)fmax((int)dist2 * 0.1, 1));
			player->lerpMotion(pos);
		}
		else {
			player->setPos(pos);
		}
		Odelay = 0;
	}
	//g_Data.getClientInstance()->getMoveTurnInput()->isSneakDown = false;
}

void TPAura::onEnable() {
	if (g_Data.getLocalPlayer() == nullptr) {
		this->setEnabled(false);
		return;
	}
}