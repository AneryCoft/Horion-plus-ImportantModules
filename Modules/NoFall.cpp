#include "NoFall.h"

NoFall::NoFall() : IModule(VK_NUMPAD3, Category::PLAYER, "Prevents you from taking falldamage") {
	mode = (*new SettingEnum(this))
		.addEntry(EnumEntry("Vanilla", 0))
		.addEntry(EnumEntry("Mineplex", 1))
		.addEntry(EnumEntry("Nukkit", 2))
		.addEntry(EnumEntry("Water", 3));
	registerEnumSetting("Mode", &mode, 5);
}

NoFall::~NoFall() {
}

const char* NoFall::getModuleName() {
	return ("NoFall");
}

void NoFall::onTick(C_GameMode* gm) {
	C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();

	if (localPlayer == nullptr)
		return;

	vec3_t* localPlayerPos = localPlayer->getPos();
	switch (mode.selected) {
	case 2:
		if (g_Data.getLocalPlayer()->fallDistance > 2.f) {
			C_PlayerActionPacket packet;
			packet.action = 15;  //�����ʳ�
			packet.entityRuntimeId = g_Data.getLocalPlayer()->entityRuntimeId;
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&packet);
		}
		break;
	case 3:
	{
		static bool canBuild = true; //��ֹ��η���
		static int delay = 0; //ʹ����Ʒ���л���Ʒ��֮����ӳ٣�������ӳٽ��л����ˣ�

		if (localPlayerPos->y < 0.f)
			return; //������������

		if (localPlayer->fallDistance > 2.5f && canBuild) {
			for (int i = (int)localPlayerPos->y - 1; i > -1; i--) {
				blockPos = vec3_ti(floor(localPlayerPos->x), i, floor(localPlayerPos->z));
				int blockID = gm->player->region->getBlock(blockPos)->toLegacy()->blockId;

				if (blockID != 0 && blockID != 8 && blockID != 9) //����������ˮ��ˮ
					break;

				if (i == 0 && blockID == 0)
					return; //����Ϊ���
			} //�������ʱ���µĵķ���

			//if (localPlayerPos->dist(blockPos.toVec3t()) < 5.f) {
			if (localPlayer->eyePos0.y - (float)blockPos.y < 6.f) {
				switchWaterBucket();

				if (localPlayer->getSelectedItemId() == 325) {
					gm->buildBlock(&blockPos, 1);

					canBuild = false;
				}
			}
		}

		if (localPlayer->fallDistance < 1.f && !canBuild) {
			++delay;
			if (delay == 2) {
				gm->buildBlock(&blockPos, 1);
			} //�ջ�ˮ

			if (delay == 3) {
				localPlayer->getSupplies()->selectedHotbarSlot = prevSlot; //�л�ԭ������Ʒ
				canBuild = true;
				delay = 0;
			}
		}
	}
	}
}

void NoFall::switchWaterBucket() {
	C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory* inv = supplies->inventory;
	prevSlot = supplies->selectedHotbarSlot;
	/*
	C_ItemStack* stack = inv->getItemStack(prevSlot);
	clientMessageF("%i", (*stack->item)->itemId); //��ʱ��ȡˮͰ��ID
	*/

	for (int n = 0; n < 9; n++) {
		C_ItemStack* stack = inv->getItemStack(n);
		if (stack->item != nullptr && (*stack->item)->itemId == 325) {
			if (prevSlot != n)
				supplies->selectedHotbarSlot = n;
			return;
		}
	}
	//clientMessageF("No water bucket found.");
} //�л�ˮͰ

void NoFall::onSendPacket(C_Packet* packet, bool& cancelSend) {
	if (g_Data.getLocalPlayer() == nullptr)
		return;
	if (g_Data.getLocalPlayer()->fallDistance > 2.f) {
		switch (mode.selected) {
		case 0:
			if (g_Data.getLocalPlayer()->fallDistance > 2.f) {
				if (packet->isInstanceOf<C_MovePlayerPacket>()) {
					C_MovePlayerPacket* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
					movePacket->onGround = true;
				}
				else if (packet->isInstanceOf<C_ActorFallPacket>()) {
					C_ActorFallPacket* fallPacket = reinterpret_cast<C_ActorFallPacket*>(packet);
					fallPacket->fallDistance = 0.f;
				}
			}
			break;
		case 1:
			if (packet->isInstanceOf<C_MovePlayerPacket>()) {
				C_MovePlayerPacket* movePacket = reinterpret_cast<C_MovePlayerPacket*>(packet);
				movePacket->onGround = true;
			}
		}
	}
}
