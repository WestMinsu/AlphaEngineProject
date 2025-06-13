#include "AssetManager.h"

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
}

AEGfxTexture* AssetManager::loadImageAsset(std::string)
{
	return nullptr;
}

AEAudio AssetManager::loadSoundAsset(std::string)
{
	return AEAudio();
}
