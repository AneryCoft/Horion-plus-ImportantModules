#include "SoundPlayer.h"

SoundPlayer::SoundPlayer() : IModule(0, Category::MISC, "Play sounds to the player around you") {
	registerIntSetting("Sound ID", &sound, sound, 0, 329);
}

SoundPlayer::~SoundPlayer() {
}

const char* SoundPlayer::getModuleName() {
	return ("SoundPlayer");
}

void SoundPlayer::onTick(C_GameMode* gm) {
	if (g_Data.getLocalPlayer() == nullptr)
		return;

	LevelSoundEventPacket Sound;
	Sound.entityType = TextHolder("minecraft:player");
	Sound.extraData = 10000;
	Sound.pos = *g_Data.getLocalPlayer()->getPos();
	Sound.sound = sound;

	g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&Sound);
}