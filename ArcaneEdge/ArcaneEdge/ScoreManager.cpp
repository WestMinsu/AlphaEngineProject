#include "ScoreManager.h"
#include <fstream>
#include <algorithm>
#include <iostream>

ScoreManager::ScoreManager(const std::string& filename) : m_filename(filename)
{
	LoadScores();
}

ScoreManager::~ScoreManager()
{
	SaveScores();
}

void ScoreManager::LoadScores()
{
	std::ifstream file(m_filename);
	if (!file.is_open())
	{
		//std::cout << "Could not open leaderboard file for reading. A new one will be created." << std::endl;
		return;
	}

	ScoreEntry entry;
	while (file >> entry.playerName >> entry.score)
	{
		m_scores.push_back(entry);
	}

	file.close();
	std::sort(m_scores.begin(), m_scores.end());
}

void ScoreManager::SaveScores()
{
	std::ofstream file(m_filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open leaderboard file for writing." << std::endl;
		return;
	}

	for (const auto& entry : m_scores)
	{
		file << entry.playerName << " " << entry.score << std::endl;
	}

	file.close();
}

void ScoreManager::AddScore(const std::string& playerName, s32 score)
{
	m_scores.push_back({ playerName, score });
	std::sort(m_scores.begin(), m_scores.end());

	if (m_scores.size() > 10)
	{
		m_scores.resize(10);
	}
}

const std::vector<ScoreEntry>& ScoreManager::GetScores() const
{
	return m_scores;
}