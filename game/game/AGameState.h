#pragma once


class AGameState
{
public: 
	virtual ~AGameState() {};

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Print() = 0;
	virtual void Exit() = 0;
};