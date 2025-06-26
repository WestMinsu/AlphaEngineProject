#pragma once
#include "AEEngine.h"
#include "string"
#include <map>


class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	friend AEGfxTexture* LoadImageAsset(const std::string&);
	friend AEAudio LoadSoundAsset(std::string);

private:
	std::map<std::string, AEGfxTexture* > m_imageAssets;
	std::map<std::string, AEAudio > m_soundAssets;
};

AEGfxTexture* LoadImageAsset(const std::string&);
AEAudio LoadSoundAsset(std::string);