#pragma once
#include "Module.h"
#include "../ModuleManager.h"
#include "../../../Utils/Target.h"

class Killaura : public IModule
{
private:
	//bool isMulti = true;
	int n = 0;
	//int delay = 0;
	//int Odelay = 0;
	int APS = 7;
	//int tick = 0;
	int multipleAPS= 1;
	bool autoweapon = false;
	void findWeapon();
	bool rotations = false;
	bool strafe = false;
	vec2_t angle;
public:
	bool isMobAura = false;
	bool hurttime = true;
	float range = 6;

	SettingEnum mode;

	Killaura();
	~Killaura();

	// Inherited via IModule
	virtual const char* getModuleName() override;
	//virtual void onTick(C_GameMode* gm) override;
	virtual void onSendPacket(C_Packet* packet, bool& cancelSend) override;
	virtual void onEnable() override;
	virtual void onGetPickRange() override;
};
