#include "GameManager.h"
#include "Utility.h"
#include "AssetManager.h"

GameState GameManager::m_nextState = GameState::NONE;
s8 GameManager::m_font;
bool GameManager::m_isGameRunning = true;
AEAudioGroup GameManager::m_sfxGroup;
AEAudioGroup GameManager::m_bgmGroup;
std::map<BGMTrack, AEAudio*> GameManager::m_bgmTracks;
BGMTrack GameManager::m_currentTrack = BGMTrack::NONE;
s32 GameManager::m_finalScore = 0;

GameManager::GameManager()
{
	m_kTextTitle = "Title";

	m_GameState = nullptr;
}

GameManager::~GameManager()
{
	FreeUtilityMeshes();
	AEGfxDestroyFont(m_font);
}

void GameManager::Init()
{
	AESysSetWindowTitle(m_kTextTitle);

	m_GameState = std::move(std::make_unique<IntroState>());
	m_GameState->Init();
	m_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

	m_sfxGroup = AEAudioCreateGroup();
	m_bgmGroup = AEAudioCreateGroup();
	LoadAllMusic();
	m_GameState = std::move(std::make_unique<IntroState>());
	m_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

	InitUtilityMeshes();
	AESysReset();
}

void GameManager::Update(f32 dt)
{
	while (m_isGameRunning)
	{
		if (GameManager::m_nextState != GameState::NONE)
		{
			m_GameState->Exit();

			switch (GameManager::m_nextState)
			{
			case GameState::INTRO:
				m_GameState = std::move(std::make_unique<IntroState>());
				break;
			case GameState::MAIN_MENU:
				m_GameState = std::move(std::make_unique<MainMenuState>());
				break;
			case GameState::MAIN_GAME:
				m_GameState = std::move(std::make_unique<MainGameState>());
				break;
			case GameState::GAME_CLEAR:
				m_GameState = std::move(std::make_unique<GameClearState>());
				break;
			case GameState::GAME_OVER:
				m_GameState = std::move(std::make_unique<GameOverState>());
				break;
			}

			m_GameState->Init();
			GameManager::m_nextState = GameState::NONE;
		}
		AESysFrameStart();

		m_GameState->Update(dt);
		m_GameState->Draw();

		if (0 == AESysDoesWindowExist()) {
			m_isGameRunning = FALSE;
		}
		AESysFrameEnd();
	}
	m_GameState->Exit();
	GetAssetManager().Destroy();
	return;
}

void GameManager::ChangeState(GameState newGameState)
{
	GameManager::m_nextState = newGameState;
}

void GameManager::PlaySFX(AEAudio sfx, float volume, float pitch, s32 loops)
{
	AEAudioPlay(sfx, m_sfxGroup, volume, pitch, loops);
}



void GameManager::LoadAllMusic()
{
	m_bgmTracks[BGMTrack::STAGE] = LoadSoundAsset("Assets/Sounds/Jeremy Blake - Powerup!.mp3");
	m_bgmTracks[BGMTrack::MENU] = LoadSoundAsset("Assets/Sounds/Krayzius & Brainstorm - Virtual Boy.mp3");
	m_bgmTracks[BGMTrack::BOSS] = LoadSoundAsset("Assets/Sounds/Kevin MacLeod - 8bit Dungeon Boss.mp3");
	m_bgmTracks[BGMTrack::CLEAR] = LoadSoundAsset("Assets/Sounds/Kevin MacLeod - Pixelland.mp3");
	m_bgmTracks[BGMTrack::OVER] = LoadSoundAsset("Assets/Sounds/Kubbi - Digestive biscuit.mp3");
}

void GameManager::PlayBGM(BGMTrack track, float volume, float pitch, s32 loops)
{
	if (m_currentTrack == track)
		return;

	AEAudioStopGroup(m_bgmGroup);

	if (m_bgmTracks.count(track))
	{
		AEAudioPlay(*m_bgmTracks[track], m_bgmGroup, volume, pitch, loops);
		m_currentTrack = track;
	}
	else
	{
		m_currentTrack = BGMTrack::NONE;
	}
}

void GameManager::StopMusic()
{
	AEAudioStopGroup(m_bgmGroup);
	m_currentTrack = BGMTrack::NONE;
}

void GameManager::SetFinalScore(s32 score)
{
	m_finalScore = score;
}

s32 GameManager::GetFinalScore()
{
	return m_finalScore;
}
