#pragma once
#include "Animation.h"

enum class WeaponType
{
	FIRE,
	ICE
	//LIGHTNING
};

struct ProjectileData
{
	f32 speed;
	s32 damage;
	AEVec2 size;
	AnimData animData;
};