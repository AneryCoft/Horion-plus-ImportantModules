#include "Criticals.h"
#include <windows.h>

Criticals::Criticals() : IModule(0, Category::COMBAT, "Each hit is a critical hit") {
	mode = (*new SettingEnum(this))
		.addEntry(EnumEntry("MovePacket", 0))
		.addEntry(EnumEntry("AuthPacket", 1))
		.addEntry(EnumEntry("LowHop", 2))
		.addEntry(EnumEntry("TPHop", 3));
	registerEnumSetting("Mode", &mode, 0);
}

Criticals::~Criticals() {
}

const char* Criticals::getModuleName() {
	return ("Criticals");
}

void Criticals::onAttack(C_Entity* attackEnt) {
	static int a = 0;
	++a;
	if (a == 2) {
		a = 0;
		return;
	}

	//clientMessageF("Attack");
	//clientMessageF("%i",attackEnt->getEntityTypeId());
	//clientMessageF("height:%f width:%f", attackEnt->height, attackEnt->width);

	vec3_t localPlayerPos = *g_Data.getLocalPlayer()->getPos();
	if (mode.selected == 0) {
		C_PlayerActionPacket actionPacket;
		actionPacket.action = 8; //ÌøÔ¾
		actionPacket.entityRuntimeId = g_Data.getLocalPlayer()->entityRuntimeId;
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&actionPacket);

		C_MovePlayerPacket movePacket;
		movePacket.onGround = false;
		movePacket.Position = { localPlayerPos.x,localPlayerPos.y + 1.2f,localPlayerPos.z };
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&movePacket);
		movePacket.Position = { localPlayerPos.x,localPlayerPos.y + 0.4f,localPlayerPos.z };
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&movePacket);
		/*
		C_PlayerActionPacket actionPacket;
		actionPacket.action = 8; //ÌøÔ¾
		actionPacket.entityRuntimeId = g_Data.getLocalPlayer()->entityRuntimeId;
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&actionPacket);

		C_MovePlayerPacket movePacket;
		movePacket.onGround = false;
		movePacket.Position = *g_Data.getLocalPlayer()->getPos();
		movePacket.Position.y += 1.2f;
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&movePacket);
		Sleep(0.05);
		movePacket.Position.y -= 0.3f;
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&movePacket);
		//g_Data.getLocalPlayer()->setPos(g_Data.getLocalPlayer()->getPos()->add(0, 0.9, 0));
		*/
	}
	if (mode.selected == 2) {
		g_Data.getLocalPlayer()->velocity.y = 0.1f;
	}
	if (mode.selected == 3) {
		C_MovePlayerPacket movePacket;
		movePacket.onGround = false;
		movePacket.Position = { localPlayerPos.x,localPlayerPos.y + 1.2f,localPlayerPos.z };
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&movePacket);
		g_Data.getLocalPlayer()->setPos(localPlayerPos.add(0.f, 1.2f, 0.f));
	}
}

void Criticals::onSendPacket(C_Packet* packet, bool& cancelSend) {
	if (g_Data.getLocalPlayer() == nullptr)
		return;

	if (mode.selected == 1) {
		if (packet->isInstanceOf<PlayerAuthInputPacket>()) {
			PlayerAuthInputPacket* authPacket = reinterpret_cast<PlayerAuthInputPacket*>(packet);
			if (tick == 0) {
				authPacket->pos.y += 1.25f;
				++tick;
				return;
			}
			if (tick == 1) {
				authPacket->pos.y += 1.24f;
				++tick;
				return;
			}
			if (tick == 2) {
				authPacket->pos.y += 1.23f;
				++tick;
				return;
			}
			if (tick == 3) {
				authPacket->pos.y += 1.22f;
				++tick;
				return;
			}
			if (tick == 4) {
				authPacket->pos.y += 1.21f;
				++tick;
				return;
			}
			if (tick == 5) {
				authPacket->pos.y += 1.20f;
				++tick;
				return;
			}
			if (tick == 6) {
				authPacket->pos.y += 1.19f;
				++tick;
				return;
			}
			if (tick == 7) {
				authPacket->pos.y += 1.18f;
				++tick;
				return;
			}
			if (tick == 8) {
				authPacket->pos.y += 1.17f;
				++tick;
				return;
			}
			if (tick == 9) {
				authPacket->pos.y += 1.16f;
				tick = 0;
				return;
			}
			/*
			if (fakeFall == 0) {
				C_PlayerActionPacket actionPacket;
				actionPacket.action = 8; //ÌøÔ¾
				actionPacket.entityRuntimeId = g_Data.getLocalPlayer()->entityRuntimeId;
				g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&actionPacket);

				authPacket->pos.y += 1.25f;

				++fakeFall;
				return;
			}
			if (fakeFall == 1) {

				authPacket->pos.y += 1.17676f;

				++fakeFall;
				return;
			}
			if (fakeFall == 2) {
				authPacket->pos.y += 1.024424f;

				++fakeFall;
				return;
			}
			if (fakeFall == 3) {
				authPacket->pos.y += 0.796736f;

				++fakeFall;
				return;
			}
			if (fakeFall == 4) {
				authPacket->pos.y += 0.495201f;

				++fakeFall;
				return;
			}
			if (fakeFall == 5) {
				authPacket->pos.y += 0.121297f;

				fakeFall = 0;
				return;
			}
			*/
		}
	}
}
