#pragma once
#include "ACharacter.h"

class CharacterPlayer : public ACharacter
{
public:
	CharacterPlayer();
	~CharacterPlayer();

	void Init() override;
	void Update() override;
	void Move() override;
	void Attack() override;
	void Draw() override;

private:
	// Weapon
};

