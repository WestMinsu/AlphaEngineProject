#include "AssetManager.h"

AssetManager assetManager;

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
	for (auto& [key, ptr] : m_imageAssets) 
	{
		AEGfxTextureUnload(ptr);
	}

	for (auto& [key, ptr] : m_soundAssets ) 
	{
		AEAudioUnloadAudio(ptr);
	}
}

AEGfxTexture* LoadImageAsset(std::string file)
{
	if (assetManager.m_imageAssets.find(file) == assetManager.m_imageAssets.end())
	{
		assetManager.m_imageAssets[file] = AEGfxTextureLoad(file.c_str());
	}

	return assetManager.m_imageAssets.find(file)->second;
}

AEAudio LoadSoundAsset(std::string file)
{
	if (assetManager.m_soundAssets.find(file) == assetManager.m_soundAssets.end())
	{
		assetManager.m_soundAssets[file] = AEAudioLoadMusic(file.c_str());
	}

	return assetManager.m_soundAssets.find(file)->second;
}
