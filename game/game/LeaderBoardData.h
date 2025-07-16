#pragma once
#include <string>
#include "AETypes.h"

struct ScoreEntry
{
	std::string playerName;
	s32 score;

	bool operator<(const ScoreEntry& other) const
	{
		return score > other.score;
	}
};
