#pragma once
#include "TouchManager.h"
#include "Sprite.h"

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>
#include <Type/csmVector.hpp>
#include <Rendering/D3D11/CubismOffscreenSurface_D3D11.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include <CubismFramework.hpp>
#include "Model.h"

class View
{
public:

    enum SelectTarget
    {
        SelectTarget_None,
        SelectTarget_ModelFrameBuffer,
        SelectTarget_ViewFrameBuffer,
    };

    View();

    ~View();

    void Initialize();

    void Render();

    void InitializeSprite();

    void ReleaseSprite();
    
    void ResizeSprite();

    void OnTouchesBegan(float pointX, float pointY) const;

    void OnTouchesMoved(float pointX, float pointY) const;

    void OnTouchesEnded(float pointX, float pointY) const;

    float TransformViewX(float deviceX) const;

    float TransformViewY(float deviceY) const;

    float TransformScreenX(float deviceX) const;

    float TransformScreenY(float deviceY) const;

    void PreModelDraw(Model& refModel);

    void PostModelDraw(Model& refModel);

    float GetSpriteAlpha(int assign) const;

    void SwitchRenderingTarget(SelectTarget targetType);

    void SetRenderTargetClearColor(float r, float g, float b);

    void DestroyOffscreenFrame();

private:
    TouchManager* touchManager;
    Csm::CubismMatrix44* deviceToScreen;
    Csm::CubismViewMatrix* viewMatrix;

    Sprite* back;
    Sprite* gear;
    Sprite* power;

    Sprite* renderSprite;
    Csm::Rendering::CubismOffscreenFrame_D3D11 renderBuffer;
    SelectTarget renderTarget;
    float clearColor[4];
};

