#include "Fly.h"

Fly::Fly() : IModule('F', Category::MOVEMENT, "Fly to the sky") {
	mode = (*new SettingEnum(this))
		.addEntry(EnumEntry("Creative", 0))
		.addEntry(EnumEntry("CubeGlide", 1))
		.addEntry(EnumEntry("Jetpack", 2))
		.addEntry(EnumEntry("AirStuck", 3))
		.addEntry(EnumEntry("Mineplex", 4))
		.addEntry(EnumEntry("Motion", 5))
		.addEntry(EnumEntry("NoClip", 6))
		.addEntry(EnumEntry("ElytraFly", 7));
	registerEnumSetting("Mode", &mode, 5);
	this->registerBoolSetting("Ground Spoof", &this->groundSpoof, this->groundSpoof);
	this->registerBoolSetting("Elytra Spoof", &this->elytraSpoof, this->elytraSpoof);
	this->registerBoolSetting("Pos Confuse", &this->positionConfuse, this->positionConfuse);

	registerFloatSetting("Horizontal Speed", &this->horizontalSpeed, this->horizontalSpeed, 0.1f, 5.f);
	registerFloatSetting("Vertical Speed", &this->verticalSpeed, this->verticalSpeed, 0.1f, 5.f);
	//registerFloatSetting("Fall Speed", &this->fallSpeed, this->fallSpeed, -1.f, 0.f);
}

Fly::~Fly() {
}

const char* Fly::getModuleName() {
	return ("Fly");
}

void Fly::onTick(C_GameMode* gm) {
	if (gm->player == nullptr)
		return;

	if (positionConfuse) {
		vec3_t pos = *g_Data.getLocalPlayer()->getPos();
		pos.y += 1.2f;
		C_MovePlayerPacket a(g_Data.getLocalPlayer(), pos);
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);
		pos.y -= 1.2f;
		C_MovePlayerPacket a2(g_Data.getLocalPlayer(), pos);
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a2);
	}

	switch (mode.selected) {
	case 0:
		gm->player->canFly = true;
		break;
	case 1:
	{
		float calcYaw = (gm->player->yaw + 90) * (PI / 180);

		gameTick++;

		vec3_t pos = *g_Data.getLocalPlayer()->getPos();

		pos.y += 1.3f;
		C_MovePlayerPacket a(g_Data.getLocalPlayer(), pos);
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);
		pos.y -= 1.3f;
		C_MovePlayerPacket a2(g_Data.getLocalPlayer(), pos);
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a2);

		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * horizontalSpeed;
		moveVec.z = sin(calcYaw) * horizontalSpeed;

		gm->player->lerpMotion(moveVec);

		if (gameTick >= 5) {
			gameTick = 0;
			float yaw = gm->player->yaw * (PI / 180);
			float length = 4.f;

			float x = -sin(yaw) * length;
			float z = cos(yaw) * length;

			gm->player->setPos(pos.add(vec3_t(x, 0.5f, z)));
		}
		break;
	}
	case 2:
	{
		float calcYaw = (gm->player->yaw + 90) * (PI / 180);
		float calcPitch = (gm->player->pitch) * -(PI / 180);
		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * cos(calcPitch) * horizontalSpeed;
		moveVec.y = sin(calcPitch) * verticalSpeed;
		moveVec.z = sin(calcYaw) * cos(calcPitch) * horizontalSpeed;

		gm->player->lerpMotion(moveVec);
	}
	break;
	case 3:
		gm->player->velocity = vec3_t(0, 0, 0);
		break;
	case 4:
		gm->player->velocity.y = 0.f;
		break;
	case 6:
		gm->player->aabb.upper.y = gm->player->aabb.lower.y - (float)1.8f;
	case 5:
	{
		gm->player->velocity = vec3_t(0.f, 0.f, 0.f);
		C_GameSettingsInput* input = g_Data.getClientInstance()->getGameSettingsInput();

		if (g_Data.canUseMoveKeys()) {
			if (GameData::isKeyDown(*input->spaceBarKey))
				gm->player->velocity.y = verticalSpeed;
			if (GameData::isKeyDown(*input->sneakKey))
				gm->player->velocity.y = -verticalSpeed;
		}
	}
	}
}

void Fly::onMove(C_MoveInputHandler* input) {
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr) return;

	switch (mode.selected) {
	case 4:
	{
		if (player->isSneaking())
			return;

		vec2_t moveVec2d = { input->forwardMovement, -input->sideMovement };
		bool pressed = moveVec2d.magnitude() > 0.01f;

		if (player->onGround && pressed)
			player->jumpFromGround();

		float calcYaw = (player->yaw + 90) * (PI / 180);
		vec3_t moveVec;
		float c = cos(calcYaw);
		float s = sin(calcYaw);
		moveVec2d = { moveVec2d.x * c - moveVec2d.y * s, moveVec2d.x * s + moveVec2d.y * c };
		moveVec.x = moveVec2d.x * horizontalSpeed;
		moveVec.y = player->velocity.y;
		moveVec.z = moveVec2d.y * horizontalSpeed;
		if (pressed) player->lerpMotion(moveVec);
	}
	break;
	case 7:
		if (!flying)
			break;
	case 5:
	case 6:
	
	{
		float yaw = player->yaw;

		bool pressed = input->forward || input->backward || input->right || input->left;

		if (input->forward && input->backward)
			return;


		if (input->right) {
			yaw += 90.f;
			if (input->forward)
				yaw -= 45.f;
			else if (input->backward)
				yaw += 45.f;
		}
		if (input->left) {
			yaw -= 90.f;
			if (input->forward)
				yaw += 45.f;
			else if (input->backward)
				yaw -= 45.f;
		}

		if (input->backward && !input->left && !input->right)
			yaw += 180.f;
		float calcYaw = (yaw + 90) * (PI / 180);
		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * horizontalSpeed;
		moveVec.y = player->velocity.y;
		moveVec.z = sin(calcYaw) * horizontalSpeed;

		if (pressed) player->lerpMotion(moveVec);
	}
	}
}

void Fly::onSendPacket(C_Packet* packet, bool& cancelSend) {
	if (packet->isInstanceOf<C_PlayerActionPacket>()) {
		C_PlayerActionPacket* packets = reinterpret_cast<C_PlayerActionPacket*>(packet);

		if (mode.selected == 7) {
			if (packets->action == 15) { //Æô¶¯ÇÊ³á
				flying = true;
			}
			else if (packets->action == 16) { //½ûÓÃÇÊ³á
				flying = false;
			}
		}

		if (elytraSpoof) {
			if (packets->action == 16) {
				cancelSend = true; //ÆÁ±ÎÕâ¸ö°ü
			}
		}
	}

	if (packet->isInstanceOf<C_MovePlayerPacket>() && groundSpoof) {
		C_MovePlayerPacket* packets = reinterpret_cast<C_MovePlayerPacket*>(packet);
		packets->onGround = true;
	}
}

void Fly::onEnable() {
	if (elytraSpoof && g_Data.getLocalPlayer() != nullptr) {
		C_PlayerActionPacket actionPacket;
		actionPacket.action = 15;  //¿ªÆôÇÊ³á
		actionPacket.entityRuntimeId = g_Data.getLocalPlayer()->entityRuntimeId;
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&actionPacket);
	}
}


void Fly::onDisable() {
	if (g_Data.getLocalPlayer() == nullptr)
		return;

	switch (mode.selected) {
	case 0:
		if (g_Data.getLocalPlayer()->gamemode != 1)
			g_Data.getLocalPlayer()->canFly = false;
		break;
	case 6:
		g_Data.getLocalPlayer()->aabb.upper.y = g_Data.getLocalPlayer()->aabb.lower.y + (float)1.8f;
	case 1:
	case 4:
	case 5:
		g_Data.getLocalPlayer()->velocity = vec3_t(0, 0, 0);
	}

	if (elytraSpoof) {
		C_PlayerActionPacket actionPacket;
		actionPacket.action = 16;  //½ûÓÃÇÊ³á
		actionPacket.entityRuntimeId = g_Data.getLocalPlayer()->entityRuntimeId;
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&actionPacket);
	}
}