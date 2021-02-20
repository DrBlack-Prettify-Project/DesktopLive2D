#include "Model.h"

#include <fstream>
#include <vector>
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <Physics/CubismPhysics.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include <Utils/CubismString.hpp>
#include <Id/CubismIdManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include "App.h"
#include "TextureManager.h"
#include "Utils.h"

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;

namespace {
    csmByte* CreateBuffer(const csmChar* path, csmSizeInt* size)
    {
        return Utils::LoadFileAsBytes(path, size);
    }

    void DeleteBuffer(csmByte* buffer, const csmChar* path = "")
    {
        Utils::ReleaseBytes(buffer);
    }
}

Model::Model()
    : CubismUserModel()
    , modelSetting(NULL)
    , userTimeSeconds(0.0f)
    , deleteModel(false)
{
    idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    idParamEyeBallX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    idParamEyeBallY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);
}

Model::~Model()
{
    renderBuffer.DestroyOffscreenFrame();

    ReleaseMotions();
    ReleaseExpressions();

    for (csmInt32 i = 0; i < modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = modelSetting->GetMotionGroupName(i);
        ReleaseMotionGroup(group);
    }

    for (csmUint32 i = 0; i < bindTextureId.GetSize(); i++)
    {
        App::GetInstance()->GetTextureManager()->ReleaseTexture(bindTextureId[i]);
    }
    bindTextureId.Clear();

    delete modelSetting;
}

void Model::LoadAssets(const csmChar* dir, const csmChar* fileName)
{
    modelHomeDir = dir;

    OutputDebugString(L"Load model setting\n");

    csmSizeInt size;
    const csmString path = csmString(dir) + fileName;

    csmByte* buffer = CreateBuffer(path.GetRawString(), &size);
    ICubismModelSetting* setting = new CubismModelSettingJson(buffer, size);
    DeleteBuffer(buffer, path.GetRawString());

    SetupModel(setting);

    CreateRenderer();

    SetupTextures();
}


void Model::SetupModel(ICubismModelSetting* setting)
{
    updating = true;
    initialized = false;

    modelSetting = setting;

    csmByte* buffer;
    csmSizeInt size;

    if (strcmp(modelSetting->GetModelFileName(), "") != 0)
    {
        csmString path = modelSetting->GetModelFileName();
        path = modelHomeDir + path;

        OutputDebugString(L"Load model\n");

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadModel(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    if (modelSetting->GetExpressionCount() > 0)
    {
        const csmInt32 count = modelSetting->GetExpressionCount();
        for (csmInt32 i = 0; i < count; i++)
        {
            csmString name = modelSetting->GetExpressionName(i);
            csmString path = modelSetting->GetExpressionFileName(i);
            path = modelHomeDir + path;

            buffer = CreateBuffer(path.GetRawString(), &size);
            ACubismMotion* motion = LoadExpression(buffer, size, name.GetRawString());

            if (expressions[name] != NULL)
            {
                ACubismMotion::Delete(expressions[name]);
                expressions[name] = NULL;
            }
            expressions[name] = motion;

            DeleteBuffer(buffer, path.GetRawString());
        }
    }

    if (strcmp(modelSetting->GetPhysicsFileName(), "") != 0)
    {
        csmString path = modelSetting->GetPhysicsFileName();
        path = modelHomeDir + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadPhysics(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    if (strcmp(modelSetting->GetPoseFileName(), "") != 0)
    {
        csmString path = modelSetting->GetPoseFileName();
        path = modelHomeDir + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadPose(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    if (modelSetting->GetEyeBlinkParameterCount() > 0)
    {
        eyeBlink = CubismEyeBlink::Create(modelSetting);
    }

    {
        breath = CubismBreath::Create();

        csmVector<CubismBreath::BreathParameterData> breathParameters;

        breathParameters.PushBack(CubismBreath::BreathParameterData(idParamAngleX, 0.0f, 15.0f, 6.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(idParamAngleY, 0.0f, 8.0f, 3.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(idParamAngleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(idParamBodyAngleX, 0.0f, 4.0f, 15.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));

        breath->SetParameters(breathParameters);
    }

    if (strcmp(modelSetting->GetUserDataFile(), "") != 0)
    {
        csmString path = modelSetting->GetUserDataFile();
        path = modelHomeDir + path;
        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadUserData(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    {
        csmInt32 eyeBlinkIdCount = modelSetting->GetEyeBlinkParameterCount();
        for (csmInt32 i = 0; i < eyeBlinkIdCount; ++i)
        {
            eyeBlinkIds.PushBack(modelSetting->GetEyeBlinkParameterId(i));
        }
    }

    {
        csmInt32 lipSyncIdCount = modelSetting->GetLipSyncParameterCount();
        for (csmInt32 i = 0; i < lipSyncIdCount; ++i)
        {
            lipSyncIds.PushBack(modelSetting->GetLipSyncParameterId(i));
        }
    }

    csmMap<csmString, csmFloat32> layout;
    modelSetting->GetLayoutMap(layout);
    modelMatrix->SetupFromLayout(layout);

    model->SaveParameters();

    for (csmInt32 i = 0; i < modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = modelSetting->GetMotionGroupName(i);
        PreloadMotionGroup(group);
    }

    motionManager->StopAllMotions();

    updating = false;
    initialized = true;
}

void Model::PreloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = modelSetting->GetMotionCount(group);

    for (csmInt32 i = 0; i < count; i++)
    {
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = modelSetting->GetMotionFileName(group, i);
        path = modelHomeDir + path;

        OutputDebugString(L"Load motion\n");

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString()));

        csmFloat32 fadeTime = modelSetting->GetMotionFadeInTimeValue(group, i);
        if (fadeTime >= 0.0f)
        {
            tmpMotion->SetFadeInTime(fadeTime);
        }

        fadeTime = modelSetting->GetMotionFadeOutTimeValue(group, i);
        if (fadeTime >= 0.0f)
        {
            tmpMotion->SetFadeOutTime(fadeTime);
        }
        tmpMotion->SetEffectIds(eyeBlinkIds, lipSyncIds);

        if (motions[name] != NULL)
        {
            ACubismMotion::Delete(motions[name]);
        }
        motions[name] = tmpMotion;

        DeleteBuffer(buffer, path.GetRawString());
    }
}

void Model::ReleaseMotionGroup(const csmChar* group) const
{
    const csmInt32 count = modelSetting->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++)
    {
        csmString voice = modelSetting->GetMotionSoundFileName(group, i);
        if (strcmp(voice.GetRawString(), "") != 0)
        {
            csmString path = voice;
            path = modelHomeDir + path;
        }
    }
}

void Model::ReleaseMotions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = motions.Begin(); iter != motions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    motions.Clear();
}

void Model::ReleaseExpressions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = expressions.Begin(); iter != expressions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    expressions.Clear();
}

void Model::Update()
{
    const csmFloat32 deltaTimeSeconds = LAppPal::GetDeltaTime();
    userTimeSeconds += deltaTimeSeconds;

    dragManager->Update(deltaTimeSeconds);
    dragX = _dragManager->GetX();
    dragY = _dragManager->GetY();

    csmBool motionUpdated = false;


    model->LoadParameters();
    if (motionManager->IsFinished())
    {
        StartRandomMotion(MotionGroupIdle, PriorityIdle);
    }
    else
    {
        motionUpdated = motionManager->UpdateMotion(_model, deltaTimeSeconds);
    }
    model->SaveParameters();


    if (!motionUpdated)
    {
        if (eyeBlink != NULL)
        {
            eyeBlink->UpdateParameters(_model, deltaTimeSeconds);
        }
    }

    if (expressionManager != NULL)
    {
        expressionManager->UpdateMotion(model, deltaTimeSeconds);
    }

    model->AddParameterValue(idParamAngleX, _dragX * 30);
    model->AddParameterValue(idParamAngleY, _dragY * 30);
    model->AddParameterValue(idParamAngleZ, _dragX * _dragY * -30);

    model->AddParameterValue(idParamBodyAngleX, _dragX * 10);

    model->AddParameterValue(idParamEyeBallX, _dragX);
    model->AddParameterValue(idParamEyeBallY, _dragY);

    if (breath != NULL)
    {
        breath->UpdateParameters(_model, deltaTimeSeconds);
    }

    if (physics != NULL)
    {
        physics->Evaluate(_model, deltaTimeSeconds);
    }

    if (_lipSync)
    {
        csmFloat32 value = 0;

        for (csmUint32 i = 0; i < lipSyncIds.GetSize(); ++i)
        {
            model->AddParameterValue(lipSyncIds[i], value, 0.8f);
        }
    }

    if (pose != NULL)
    {
        pose->UpdateParameters(model, deltaTimeSeconds);
    }

    model->Update();

}

CubismMotionQueueEntryHandle Model::StartMotion(const csmChar* group, csmInt32 no, csmInt32 priority, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler)
{
    if (priority == PriorityForce)
    {
        motionManager->SetReservePriority(priority);
    }
    else if (!motionManager->ReserveMotion(priority))
    {
        OutputDebugString(L"Cannot start motion\n");
        return InvalidMotionQueueEntryHandleValue;
    }

    const csmString fileName = modelSetting->GetMotionFileName(group, no);

    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (motion == NULL)
    {
        csmString path = fileName;
        path = modelHomeDir + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, NULL, onFinishedMotionHandler));
        csmFloat32 fadeTime = modelSetting->GetMotionFadeInTimeValue(group, no);
        if (fadeTime >= 0.0f)
        {
            motion->SetFadeInTime(fadeTime);
        }

        fadeTime = modelSetting->GetMotionFadeOutTimeValue(group, no);
        if (fadeTime >= 0.0f)
        {
            motion->SetFadeOutTime(fadeTime);
        }
        motion->SetEffectIds(eyeBlinkIds, lipSyncIds);
        autoDelete = true;

        DeleteBuffer(buffer, path.GetRawString());
    }
    else
    {
        motion->SetFinishedMotionHandler(onFinishedMotionHandler);
    }

    csmString voice = modelSetting->GetMotionSoundFileName(group, no);
    if (strcmp(voice.GetRawString(), "") != 0)
    {
        csmString path = voice;
        path = modelHomeDir + path;
    }

    OutputDebugString(L"Cannot start motion\n");
    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);
}

CubismMotionQueueEntryHandle Model::StartRandomMotion(const csmChar* group, csmInt32 priority, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler)
{
    if (modelSetting->GetMotionCount(group) == 0)
    {
        return InvalidMotionQueueEntryHandleValue;
    }

    csmInt32 no = rand() % modelSetting->GetMotionCount(group);

    return StartMotion(group, no, priority, onFinishedMotionHandler);
}

void Model::DoDraw()
{
    GetRenderer<Rendering::CubismRenderer_D3D11>()->DrawModel();
}

void Model::Draw(Csm::CubismMatrix44& matrix)
{
    Rendering::CubismRenderer_D3D11* renderer = GetRenderer<Rendering::CubismRenderer_D3D11>();

    if (model == NULL || deleteModel || renderer == NULL)
    {
        return;
    }

    matrix.MultiplyByMatrix(_modelMatrix);

    renderer->SetMvpMatrix(&matrix);

    DoDraw();
}

csmBool Model::HitTest(const csmChar* hitAreaName, csmFloat32 x, csmFloat32 y)
{
    if (opacity < 1)
    {
        return false;
    }
    const csmInt32 count = modelSetting->GetHitAreasCount();
    for (csmInt32 i = 0; i < count; i++)
    {
        if (strcmp(modelSetting->GetHitAreaName(i), hitAreaName) == 0)
        {
            const CubismIdHandle drawID = modelSetting->GetHitAreaId(i);
            return IsHit(drawID, x, y);
        }
    }
    return false;
}

void Model::SetExpression(const csmChar* expressionID)
{
    ACubismMotion* motion = expressions[expressionID];

    if (motion != NULL)
    {
        expressionManager->StartMotionPriority(motion, false, PriorityForce);
    }
    else
    {
        OutputDebugString(L"Expression is null\n");
    }
}

void Model::SetRandomExpression()
{
    if (expressions.GetSize() == 0)
    {
        return;
    }

    csmInt32 no = rand() % expressions.GetSize();
    csmMap<csmString, ACubismMotion*>::const_iterator map_ite;
    csmInt32 i = 0;
    for (map_ite = expressions.Begin(); map_ite != expressions.End(); map_ite++)
    {
        if (i == no)
        {
            csmString name = (*map_ite).First;
            SetExpression(name.GetRawString());
            return;
        }
        i++;
    }
}

void Model::ReloadRenderer()
{
    DeleteRenderer();

    CreateRenderer();

    SetupTextures();
}

void Model::SetupTextures()
{
#ifdef PREMULTIPLIED_ALPHA_ENABLE
    const bool isPreMult = true;
    const bool isTextureMult = false;
#else
    const bool isPreMult = false;
    const bool isTextureMult = false;
#endif

    bindTextureId.Clear();

    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < modelSetting->GetTextureCount(); modelTextureNumber++)
    {
        if (strcmp(modelSetting->GetTextureFileName(modelTextureNumber), "") == 0)
        {
            continue;
        }

        csmString texturePath = modelSetting->GetTextureFileName(modelTextureNumber);
        texturePath = modelHomeDir + texturePath;

        TextureManager::TextureInfo* texture = App::GetInstance()->GetTextureManager()->CreateTextureFromPngFile(texturePath.GetRawString(), isTextureMult);

        if (texture)
        {
            const csmUint64 textureManageId = texture->id;

            ID3D11ShaderResourceView* textureView = NULL;
            if (App::GetInstance()->GetTextureManager()->GetTexture(textureManageId, textureView))
            {
                GetRenderer<Rendering::CubismRenderer_D3D11>()->BindTexture(modelTextureNumber, textureView);
                bindTextureId.PushBack(textureManageId);
            }
        }
    }

    GetRenderer<Rendering::CubismRenderer_D3D11>()->IsPremultipliedAlpha(isPreMult);
}

void Model::MotionEventFired(const csmString& eventValue)
{
    CubismLogInfo("%s is fired on LAppModel!!", eventValue.GetRawString());
}


Csm::Rendering::CubismOffscreenFrame_D3D11& Model::GetRenderBuffer()
{
    return renderBuffer;
}
