#pragma once
#include <AEEngine.h>
#include <vector>

class Background
{
public:
	Background();
	~Background();

	void Init();
	void Update(f32);
	void Draw();
	void Destroy();

private:
	std::vector<AEGfxTexture*> m_backgrounds;
	AEVec2 m_position;
};

