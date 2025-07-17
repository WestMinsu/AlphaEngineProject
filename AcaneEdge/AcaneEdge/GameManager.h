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
#include "ScoreManager.h"

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
	static GameManager* GetInstance();

	~GameManager();

	void Init();
	void Update();

	static GameState m_nextState;

	static s8 m_font;
	static s8 m_scoreFont;

	static void ChangeState(GameState newGameState);
	static void PlaySFX(AEAudio sfx, float volume = 1.0f, float pitch = 1.0f, s32 loops = 0);
	static void PlayBGM(BGMTrack track, float volume = 0.3f, float pitch = 1.0f, s32 loops = -1);
	static void StopMusic();
	static bool m_isGameRunning;
	static void SetFinalScore(s32 score);
	static s32 GetFinalScore();

	ScoreManager& GetScoreManager();
private:
	GameManager();
	void LoadAllMusic();

	const char* m_kTextTitle;
	std::unique_ptr<AGameState> m_GameState;

	static std::unique_ptr<GameManager> s_pInstance;

	static AEAudioGroup m_sfxGroup;
	static AEAudioGroup m_bgmGroup;
	static std::map<BGMTrack, AEAudio*> m_bgmTracks;
	static BGMTrack m_currentTrack;
	static s32 m_finalScore;

	std::unique_ptr<ScoreManager> m_pScoreManager;
};