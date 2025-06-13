#pragma once
#include "AEEngine.h"
#include "string"
#include <map>


class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	AEGfxTexture* loadImageAsset(std::string);
	AEAudio loadSoundAsset(std::string);

private:
	std::map<std::string, AEGfxTexture* > imageAssets;
	std::map<std::string, AEAudio > soundAssets;
};

extern AssetManager assetManager;