#pragma once
#include "AEEngine.h"

enum class ElementType
{
	NONE,
	FIRE,
	ICE,
	DARK
};

enum class DamageType
{
	NONE,
	FIRE,
	ICE,
	LIGHTNING
};

enum class CharacterAnimationState
{
	IDLE,
	WALK,
	JUMP,
	MELEE_ATTACK,
	RANGED_ATTACK,
	DASH,
	DEATH,
	HURT,

	//for boss
	APPEARANCE,
	GLOWING,
	LASER_CAST,
	LASER_SHEET,
	BUFF
};

enum class CharacterDirection
{
	LEFT,
	RIGHT
};

enum class SpriteSheetOrientation 
{
	HORIZONTAL, VERTICAL 
};

struct AnimData
{
	std::string texturePath;
	AEGfxTexture* pTexture = nullptr;
	s32 frameCount = 0;
	SpriteSheetOrientation orientation = SpriteSheetOrientation::VERTICAL;
	f32 frameDuration = 0.1f;
	bool loop = true;
};

struct ProjectileData
{
	DamageType type;
	f32 speed;
	s32 damage;	
	AEVec2 size;
	AnimData animData;
};

struct AttackHitbox
{
	AEVec2 offset;
	AEVec2 size;
};