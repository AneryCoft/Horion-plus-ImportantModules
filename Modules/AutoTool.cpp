#include "AutoTool.h"

AutoTool::AutoTool() : IModule(0, Category::PLAYER, "Automatically select the fastest tool when digging blocks") {
}

AutoTool::~AutoTool() {
}

const char* AutoTool::getModuleName() {
	return ("AutoTool");
}

void AutoTool::onTick(C_GameMode* gm) {
	if (GameData::isLeftClickDown() && GameData::canUseMoveKeys()) {
		PointingStruct* point = g_Data.getClientInstance()->getPointerStruct();
		C_Block* block = g_Data.getLocalPlayer()->region->getBlock(point->block);

		C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
		C_Inventory* inventory = supplies->inventory;
		float destroySpeed = 0;
		int slot = supplies->selectedHotbarSlot;

		for (int i = 0; i < 9; i++) {
			C_ItemStack* stack = inventory->getItemStack(i);
			if (stack->item != nullptr && (*stack->item)->isTool()) {
				float currentDestroySpeed = (*stack->item)->getDestroySpeed(*stack,*block);
				clientMessageF("%f", currentDestroySpeed);
				if (currentDestroySpeed > destroySpeed) {
					destroySpeed = currentDestroySpeed;
					slot = i;
				}
			}
		}
		supplies->selectedHotbarSlot = slot;
	}
}