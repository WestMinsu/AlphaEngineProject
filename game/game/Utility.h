#pragma once
void DrawRect(f32 x, f32 y, f32 w, f32 h, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f, AEGfxTexture* pTex = nullptr);
void DrawHollowRect(f32 x, f32 y, f32 w, f32 h, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
void DrawTransformedHollowRect(f32 x, f32 y, f32 w, f32 h, float angle, float r, float g, float b, float a);
AEVec2 GetNormalizedCoords(f32 x, f32 y);
void InitUtilityMeshes();
void FreeUtilityMeshes();
bool CheckAABBCollision(const AEVec2& pos1, const AEVec2& size1, const AEVec2& pos2, const AEVec2& size2);