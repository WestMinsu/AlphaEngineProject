#include "AssetManager.h"

AssetManager assetManager;

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{

}

void AssetManager::Destroy()
{
	for (auto& [key, ptr] : m_imageAssets)
	{
		AEGfxTextureUnload(ptr);
	}
	m_imageAssets.clear();

	for (auto& [key, audio] : m_soundAssets)
	{
		AEAudioUnloadAudio(audio);
	}
	m_soundAssets.clear();
}

AEGfxTexture* LoadImageAsset(const std::string& file)
{
	if (assetManager.m_imageAssets.find(file) == assetManager.m_imageAssets.end())
	{
		assetManager.m_imageAssets[file] = AEGfxTextureLoad(file.c_str());
	}

	return assetManager.m_imageAssets.find(file)->second;
}

AEAudio* LoadSoundAsset(const std::string& file)
{
	if (assetManager.m_soundAssets.find(file) == assetManager.m_soundAssets.end())
	{
		assetManager.m_soundAssets[file] = AEAudioLoadMusic(file.c_str());
	}

	return &assetManager.m_soundAssets.find(file)->second;
}

AssetManager& GetAssetManager()
{
	return assetManager;
}
