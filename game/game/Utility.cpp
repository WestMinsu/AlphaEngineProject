#include "AEEngine.h"
#include "Utility.h"
#include "Constants.h"
AEGfxVertexList* rectMesh = nullptr;      
AEGfxVertexList* hollowRectMesh = nullptr;

void InitUtilityMeshes()
{
    if (rectMesh == nullptr)
    {
        AEGfxMeshStart();
        AEGfxTriAdd(
            -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
            0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        AEGfxTriAdd(
            0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
            0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        rectMesh = AEGfxMeshEnd();
    }

    if (hollowRectMesh == nullptr)
    {
        AEGfxMeshStart();
        AEGfxVertexAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        AEGfxVertexAdd(0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        AEGfxVertexAdd(0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        AEGfxVertexAdd(-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        AEGfxVertexAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        hollowRectMesh = AEGfxMeshEnd();
    }
}

void FreeUtilityMeshes()
{
    if (rectMesh)
    {
        AEGfxMeshFree(rectMesh);
    }
    if (hollowRectMesh)
    {
        AEGfxMeshFree(hollowRectMesh);
    }
}

void DrawRect(f32 x, f32 y, f32 w, f32 h, float r, float g, float b, float a, AEGfxTexture* pTex)
{ 
    AEMtx33 scale = { 0 };
    AEMtx33Scale(&scale, w, h);

    AEMtx33 rotate = { 0 };
    AEMtx33Rot(&rotate, 0);

    AEMtx33 translate = { 0 };
    AEMtx33Trans(&translate, x, y);

    AEMtx33 transform = { 0 };
    AEMtx33Concat(&transform, &rotate, &scale);
    AEMtx33Concat(&transform, &translate, &transform);

    if (pTex == nullptr)
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    else
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

    AEGfxSetColorToMultiply(r, g, b, a);

    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    AEGfxTextureSet(pTex, 0, 0);

    AEGfxSetTransform(transform.m);

    AEGfxMeshDraw(rectMesh, AE_GFX_MDM_TRIANGLES);
}

void DrawHollowRect(f32 x, f32 y, f32 w, f32 h, float r, float g, float b, float a)
{  
    AEMtx33 scale = { 0 };
    AEMtx33Scale(&scale, w, h);

    AEMtx33 rotate = { 0 };
    AEMtx33Rot(&rotate, 0);

    AEMtx33 translate = { 0 };
    AEMtx33Trans(&translate, x, y);

    AEMtx33 transform = { 0 };
    AEMtx33Concat(&transform, &rotate, &scale);
    AEMtx33Concat(&transform, &translate, &transform);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);

    AEGfxSetColorToMultiply(r, g, b, a);

    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    AEGfxSetTransform(transform.m);

    AEGfxMeshDraw(hollowRectMesh, AE_GFX_MDM_LINES_STRIP);
}

void DrawTransformedHollowRect(f32 x, f32 y, f32 w, f32 h, float angle, float r, float g, float b, float a)
{
    AEMtx33 scale = { 0 };
    AEMtx33Scale(&scale, w, h);

    AEMtx33 rotate = { 0 };
    AEMtx33Rot(&rotate, angle);

    AEMtx33 translate = { 0 };
    AEMtx33Trans(&translate, x, y);

    AEMtx33 transform = { 0 };
    AEMtx33Concat(&transform, &rotate, &scale);
    AEMtx33Concat(&transform, &translate, &transform);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToMultiply(r, g, b, a);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    AEGfxSetTransform(transform.m);

    AEGfxMeshDraw(hollowRectMesh, AE_GFX_MDM_LINES_STRIP);
}

AEVec2 GetNormalizedCoords(f32 x, f32 y)
{
    AEVec2 ndcCoord;
    ndcCoord.x = (2.0f * x / kWindowWidth);
    ndcCoord.y = (2.0f * y / kWindowHeight);
    return ndcCoord;
}

bool CheckAABBCollision(const AEVec2& pos1, const AEVec2& size1, const AEVec2& pos2, const AEVec2& size2)
{
    float left1 = pos1.x - size1.x / 2;
    float right1 = pos1.x + size1.x / 2;
    float top1 = pos1.y + size1.y / 2;
    float bottom1 = pos1.y - size1.y / 2;

    float left2 = pos2.x - size2.x / 2;
    float right2 = pos2.x + size2.x / 2;
    float top2 = pos2.y + size2.y / 2;
    float bottom2 = pos2.y - size2.y / 2;
    if (right1 > left2 && left1 < right2 && top1 > bottom2 && bottom1 < top2)
    {
        return true;
    }

    return false;
}

