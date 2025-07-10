#pragma once
#include <memory>
#include "AEEngine.h"
#include "AGameState.h"
#include "IntroState.h"
#include "MainMenuState.h"
#include "MainGameState.h"
#include "GameClearState.h"
#include "GameOverState.h"
#include <map>

enum class BGMTrack
{
	NONE,
	MENU,
	STAGE,
	BOSS,
	CLEAR,
	OVER
};

class GameManager
{
public:
	// constructor
	GameManager();

	// destructor
	~GameManager();

	void Init();
	void Update(f32 dt);

	//static member variable


	// GameState variables to change
	static GameState m_nextState;

	// Game Font
	static s8 m_font;

	// static function
	// Change m_nextState
	static void ChangeState(GameState newGameState);
	static void PlaySFX(AEAudio sfx, float volume = 1.0f, float pitch = 1.0f, s32 loops = 0);
	static void PlayBGM(BGMTrack track, float volume = 1.0f, float pitch = 1.0f, s32 loops = -1);
	static void StopMusic();
	static bool m_isGameRunning;
private:
	void LoadAllMusic();

	const char* m_kTextTitle;

	std::unique_ptr<AGameState> m_GameState;
	static AEAudioGroup m_sfxGroup;
	static AEAudioGroup m_bgmGroup;
	static std::map<BGMTrack, AEAudio> m_bgmTracks;
	static BGMTrack m_currentTrack;
};