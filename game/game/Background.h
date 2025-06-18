#pragma once
#include <AEEngine.h>
#include <vector>

class Background
{
public:
	Background();
	~Background();

	void Init();
	void Update();
	void Draw();

private:
	std::vector<AEGfxTexture*> m_backgrounds;
	AEVec2 position;
	AEGfxVertexList* m_meshBG;
};

