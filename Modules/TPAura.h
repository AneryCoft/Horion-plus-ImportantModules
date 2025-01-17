#pragma once
#include "Module.h"
#include "../ModuleManager.h"
#include "../../../Utils/Target.h"

class TPAura : public IModule {
private:
	int delay = 0;
	int Odelay = 0;
	int turn = 0;

public:
	bool isMobAura = false;
	float range = 7;
	bool lerp = true;
	bool isMulti = true;

	SettingEnum position;
	TPAura();
	~TPAura();

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onEnable() override;
};
