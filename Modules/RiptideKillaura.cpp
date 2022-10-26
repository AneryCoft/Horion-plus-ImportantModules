#include "RiptideKillaura.h"

RiptideKillaura::RiptideKillaura() : IModule(0, Category::COMBAT, "It's like Use Riptide Trident") {
} //¼¤Á÷É±Â¾

RiptideKillaura::~RiptideKillaura() {
}

const char* RiptideKillaura::getModuleName() {
	return ("RiptideKillaura");
}

void RiptideKillaura::onTick(C_GameMode* gm) { //¿ªÆô
	if (gm->player == nullptr) {
		this->setEnabled(false);
		return;
	}

	gm->player->startSpinAttack();
}

void RiptideKillaura::onDisable() { //¹Ø±Õ
	g_Data.getLocalPlayer()->stopSpinAttack();
}

void RiptideKillaura::onSendPacket(C_Packet* packet, bool& cancelSend) {
	if (packet->isInstanceOf<C_PlayerActionPacket>()) {
		C_PlayerActionPacket* packets = reinterpret_cast<C_PlayerActionPacket*>(packet);

		if (packets->action == 23 || packets->action == 25) { //¿ªÆô¼¤Á÷¹¥»÷ ¹Ø±Õ¼¤Á÷¹¥»÷
			cancelSend = true;
		}
	} //·ÀÖ¹¼ì²â
}