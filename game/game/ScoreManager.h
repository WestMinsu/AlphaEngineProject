#pragma once
#include "LeaderBoardData.h"
#include <vector>
#include <string>

class ScoreManager
{
public:
	ScoreManager(const std::string& filename);
	~ScoreManager();

	void AddScore(const std::string& playerName, s32 score);
	const std::vector<ScoreEntry>& GetScores() const;

private:
	void LoadScores();
	void SaveScores();

	std::string m_filename;
	std::vector<ScoreEntry> m_scores;
};