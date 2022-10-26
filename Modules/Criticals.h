#pragma once
#include "Module.h"
class Criticals : public IModule {
private:
	bool critical = false;
	int tick = 0;
public:
	Criticals();
	~Criticals();

	SettingEnum mode;

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onSendPacket(C_Packet* packet, bool& cancelSend) override;
	//virtual void onTick(C_GameMode* gm) override;
	virtual void onAttack(C_Entity* attackEnt) override;
};
