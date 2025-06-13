#pragma once
#include "AEEngine.h"
#include "string"
#include <map>


class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	AEGfxTexture* LoadImageAsset(const char*);
	AEAudio LoadSoundAsset(const char*);

private:
	std::map<std::string, AEGfxTexture* > m_imageAssets;
	std::map<std::string, AEAudio > m_soundAssets;
};

extern AssetManager assetManager;