#include "View.h"
#include "App.h"

View::View() :
    back(NULL),
    gear(NULL),
    power(NULL),
    renderSprite(NULL),
    renderTarget(SelectTarget_None)
{
    clearColor[0] = 1.0f;
    clearColor[1] = 1.0f;
    clearColor[2] = 1.0f;
    clearColor[3] = 0.0f;

    touchManager = new TouchManager();

    deviceToScreen = new Csm::CubismMatrix44();

    viewMatrix = new Csm::CubismViewMatrix();
}

View::~View()
{
    renderBuffer.DestroyOffscreenFrame();

    delete renderSprite;
    delete viewMatrix;
    delete deviceToScreen;
    delete touchManager;
    delete back;
    delete gear;
    delete power;
}

void View::Initialize()
{
    int width, height;
    App::GetClientSize(width, height);

    if (width == 0 || height == 0)
    {
        return;
    }

    float ratio = static_cast<float>(height) / static_cast<float>(width);
    float left = ViewLogicalLeft;
    float right = ViewLogicalRight;
    float bottom = -ratio;
    float top = ratio;

    viewMatrix->SetScreenRect(left, right, bottom, top);

    float screenW = fabsf(left - right);
    deviceToScreen->LoadIdentity();
    deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    viewMatrix->SetMaxScale(ViewMaxScale);
    viewMatrix->SetMinScale(ViewMinScale);

    viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );
}

void View::Render()
{
    Live2DManager* live2DManager = Live2DManager::GetInstance();
    if (!live2DManager)
    {
        return;
    }

    int width, height;
    App::GetInstance()->GetClientSize(width, height);

    if (back)
    {
        back->Render(width, height);
    }
    if (gear)
    {
        gear->Render(width, height);
    }
    if (power)
    {
        power->Render(width, height);
    }

    live2DManager->OnUpdate();

    if (renderTarget == SelectTarget_ModelFrameBuffer && renderSprite)
    {
        for (Csm::csmUint32 i = 0; i < live2DManager->GetModelNum(); i++)
        {
            float alpha = GetSpriteAlpha(i);
            renderSprite->SetColor(1.0f, 1.0f, 1.0f, alpha);

            Model* model = live2DManager->GetModel(i);
            if (model)
            {
                renderSprite->RenderImmidiate(width, height, model->GetRenderBuffer().GetTextureView());
            }
        }
    }
}

void View::InitializeSprite()
{
    int width, height;
    App::GetInstance()->GetClientSize(width, height);

    TextureManager* textureManager = App::GetInstance()->GetTextureManager();
    const std::string resourcesPath = ResourcesPath;

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    std::string imageName = resourcesPath + BackImageName;
    TextureManager::TextureInfo* backgroundTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    x = width * 0.5f;
    y = height * 0.5f;
    fWidth = static_cast<float>(backgroundTexture->width * 2);
    fHeight = static_cast<float>(height) * 0.95f;
    back = new Sprite(x, y, fWidth, fHeight, backgroundTexture->id);

    imageName = resourcesPath + GearImageName;
    TextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    x = static_cast<float>(width - gearTexture->width * 0.5f);
    y = static_cast<float>(height - gearTexture->height * 0.5f);
    fWidth = static_cast<float>(gearTexture->width);
    fHeight = static_cast<float>(gearTexture->height);
    gear = new Sprite(x, y, fWidth, fHeight, gearTexture->id);

    imageName = resourcesPath + PowerImageName;
    TextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    x = static_cast<float>(width - powerTexture->width * 0.5f);
    y = static_cast<float>(powerTexture->height * 0.5f);
    fWidth = static_cast<float>(powerTexture->width);
    fHeight = static_cast<float>(powerTexture->height);
    power = new Sprite(x, y, fWidth, fHeight, powerTexture->id);

    x = width * 0.5f;
    y = height * 0.5f;
    renderSprite = new Sprite(x, y, static_cast<float>(width), static_cast<float>(height), 0);
}

void View::ReleaseSprite()
{
    delete renderSprite;
    renderSprite = NULL;

    delete gear;
    gear = NULL;

    delete power;
    power = NULL;

    delete back;
    back = NULL;
}

void View::ResizeSprite()
{
    TextureManager* textureManager = App::GetInstance()->GetTextureManager();
    if (!textureManager)
    {
        return;
    }

    int width, height;
    App::GetInstance()->GetClientSize(width, height);

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    if (back)
    {
        Csm::csmUint64 id = back->GetTextureId();
        TextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = width * 0.5f;
            y = height * 0.5f;
            fWidth = static_cast<float>(texInfo->width * 2);
            fHeight = static_cast<float>(height) * 0.95f;
            back->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (power)
    {
        Csm::csmUint64 id = power->GetTextureId();
        TextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            power->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (gear)
    {
        Csm::csmUint64 id = gear->GetTextureId();
        TextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(height - texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            gear->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (renderSprite)
    {
        x = width * 0.5f;
        y = height * 0.5f;
        renderSprite->ResetRect(x, y, static_cast<float>(width), static_cast<float>(height));
    }
}

void View::OnTouchesBegan(float px, float py) const
{
    touchManager->TouchesBegan(px, py);
}

void View::OnTouchesMoved(float px, float py) const
{
    float viewX = this->TransformViewX(touchManager->GetX());
    float viewY = this->TransformViewY(touchManager->GetY());

    touchManager->TouchesMoved(px, py);

    Live2DManager* live2DManager = Live2DManager::GetInstance();
    live2DManager->OnDrag(viewX, viewY);
}

void View::OnTouchesEnded(float px, float py) const
{
    Live2DManager* live2DManager = Live2DManager::GetInstance();
    live2DManager->OnDrag(0.0f, 0.0f);
    {
        int width, height;
        App::GetInstance()->GetClientSize(width, height);

        float x = deviceToScreen->TransformX(px);
        float y = deviceToScreen->TransformY(py);
        /*if (DebugTouchLogEnable)
        {
            LAppPal::PrintLog("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }*/
        live2DManager->OnTap(x, y);

        if (gear->IsHit(px, py))
        {
            live2DManager->NextScene();
        }

        if (power->IsHit(px, py))
        {
            App::GetInstance()->AppEnd();
        }
    }
}

float View::TransformViewX(float deviceX) const
{
    float screenX = deviceToScreen->TransformX(deviceX);
    return viewMatrix->InvertTransformX(screenX);
}

float View::TransformViewY(float deviceY) const
{
    float screenY = deviceToScreen->TransformY(deviceY);
    return viewMatrix->InvertTransformY(screenY);
}

float View::TransformScreenX(float deviceX) const
{
    return deviceToScreen->TransformX(deviceX);
}

float View::TransformScreenY(float deviceY) const
{
    return deviceToScreen->TransformY(deviceY);
}

void View::PreModelDraw(Model& refModel)
{
    Csm::Rendering::CubismOffscreenFrame_D3D11* useTarget = NULL;

    if (renderTarget != SelectTarget_None)
    {

        useTarget = (renderTarget == SelectTarget_ViewFrameBuffer) ? &renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {
            int width, height;
            App::GetClientSize(width, height);

            if (width != 0 && height != 0)
            {
                useTarget->CreateOffscreenFrame(App::GetInstance()->GetD3dDevice(),
                    static_cast<Csm::csmUint32>(width), static_cast<Csm::csmUint32>(height));
            }
        }

        useTarget->BeginDraw(App::GetInstance()->GetD3dContext());
        useTarget->Clear(App::GetInstance()->GetD3dContext(), clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }
}

void View::PostModelDraw(Model& refModel)
{
    Csm::Rendering::CubismOffscreenFrame_D3D11* useTarget = NULL;

    if (renderTarget != SelectTarget_None)
    {

        useTarget = (renderTarget == SelectTarget_ViewFrameBuffer) ? &renderBuffer : &refModel.GetRenderBuffer();

        useTarget->EndDraw(App::GetInstance()->GetD3dContext());

        if (renderTarget == SelectTarget_ViewFrameBuffer && renderSprite)
        {
            int width, height;
            App::GetInstance()->GetClientSize(width, height);

            renderSprite->SetColor(1.0f, 1.0f, 1.0f, GetSpriteAlpha(0));
            renderSprite->RenderImmidiate(width, height, useTarget->GetTextureView());
        }
    }
}

void View::SwitchRenderingTarget(SelectTarget targetType)
{
    renderTarget = targetType;
}

void View::SetRenderTargetClearColor(float r, float g, float b)
{
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
}

void View::DestroyOffscreenFrame()
{
    renderBuffer.DestroyOffscreenFrame();
}

float View::GetSpriteAlpha(int assign) const
{
    float alpha = 0.25f + static_cast<float>(assign) * 0.5f;
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    if (alpha < 0.1f)
    {
        alpha = 0.1f;
    }

    return alpha;
}
