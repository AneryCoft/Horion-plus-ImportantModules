#pragma once
#include "Module.h"

class SoundPlayer : public IModule {
private:
	int sound = 0;
public:
	SoundPlayer();
	~SoundPlayer();

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
};