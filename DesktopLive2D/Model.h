#pragma once

#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>
#include <Rendering/D3D11/CubismOffscreenSurface_D3D11.hpp>

class Model : public Csm::CubismUserModel
{
public:
    Model();

    virtual ~Model();

    void LoadAssets(const Csm::csmChar* dir, const  Csm::csmChar* fileName);

    void ReloadRenderer();

    void Update();

    void Draw(Csm::CubismMatrix44& matrix);

    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    void SetExpression(const Csm::csmChar* expressionID);

    void SetRandomExpression();

    virtual void MotionEventFired(const Live2D::Cubism::Framework::csmString& eventValue);

    virtual Csm::csmBool HitTest(const Csm::csmChar* hitAreaName, Csm::csmFloat32 x, Csm::csmFloat32 y);

    void DeleteMark() { deleteModel = true; }

    Csm::Rendering::CubismOffscreenFrame_D3D11& GetRenderBuffer();

protected:
    void DoDraw();

private:
    void SetupModel(Csm::ICubismModelSetting* setting);

    void SetupTextures();

    void PreloadMotionGroup(const Csm::csmChar* group);

    void ReleaseMotionGroup(const Csm::csmChar* group) const;

    void ReleaseMotions();

    void ReleaseExpressions();

    Csm::ICubismModelSetting* modelSetting;
    Csm::csmString modelHomeDir;
    Csm::csmFloat32 userTimeSeconds;
    Csm::csmVector<Csm::CubismIdHandle> eyeBlinkIds;
    Csm::csmVector<Csm::CubismIdHandle> lipSyncIds;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   motions;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   expressions;
    Csm::csmVector<Csm::csmRectF> hitArea;
    Csm::csmVector<Csm::csmRectF> userArea;
    const Csm::CubismId* idParamAngleX;
    const Csm::CubismId* idParamAngleY;
    const Csm::CubismId* idParamAngleZ;
    const Csm::CubismId* idParamBodyAngleX;
    const Csm::CubismId* idParamEyeBallX;
    const Csm::CubismId* idParamEyeBallY;

    Csm::csmVector<Csm::csmUint64> bindTextureId;

    bool deleteModel;

    Csm::Rendering::CubismOffscreenFrame_D3D11 renderBuffer;
};

