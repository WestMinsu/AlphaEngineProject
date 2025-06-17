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

AEGfxTexture* AssetManager::LoadImageAsset(std::string file)
{
	if (m_imageAssets.find(file) == m_imageAssets.end())
	{
		m_imageAssets[file] = AEGfxTextureLoad(file.c_str());
	}

	return m_imageAssets.find(file)->second;
}

AEAudio AssetManager::LoadSoundAsset(std::string file)
{
	if (m_soundAssets.find(file) == m_soundAssets.end())
	{
		m_soundAssets[file] = AEAudioLoadMusic(file.c_str());
	}

	return m_soundAssets.find(file)->second;
}
