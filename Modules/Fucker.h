#pragma once
#include "Module.h"
#include "../ModuleManager.h"

class Fucker : public IModule {
private:
	int range = 5;
	bool targetEsp = false;
	bool rotations = false;
	vec2_t angle;
	bool beds = true;
	bool eggs = false;
	bool cakes = true;
	bool chests = false;
	bool barrels = false;
	//bool eatcakes = false;
	float thick = 0.4f;
	bool outline = false;

	vec3_ti blockPos;

	bool bedsAction = false;
	bool eggsAction = false;
	bool cakesAction = false;
	bool chestsAction = false;
	bool barrelsAction = false;
	bool actionBOOL = false;
public:
	Fucker();
	~Fucker();

	SettingEnum action;

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onPostRender(C_MinecraftUIRenderContext* renderCtx) override;
	virtual void onGetPickRange() override;
	virtual void onSendPacket(C_Packet* packet, bool& cancelSend) override;
};
