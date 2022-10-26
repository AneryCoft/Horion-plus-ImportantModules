#pragma once
#include "Module.h"
class Fly : public IModule {
private:
	bool groundSpoof = false;
	bool elytraSpoof = false;
	bool positionConfuse = false;

	int gameTick = 0;
	bool flying = false;

	float horizontalSpeed = 1.5f;
	float verticalSpeed = 0.6f;
	//float fallSpeed = 0.f;

public:
	
	SettingEnum mode;

	Fly();
	~Fly();

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onMove(C_MoveInputHandler* input) override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onDisable() override;
	virtual void onSendPacket(C_Packet* packet, bool& cancelSend) override;
	virtual void onEnable() override;
};
