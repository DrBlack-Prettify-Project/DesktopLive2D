#pragma once
#include "TouchManager.h"
#include "Sprite.h"

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>
#include <Type/csmVector.hpp>
#include <Rendering/D3D11/CubismOffscreenSurface_D3D11.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include <CubismFramework.hpp>

class View
{
public:

    /**
     * @brief LAppModelのレンダリング枠
     */
    enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレ�`ムバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< LAppModelが光徭隔つフレ�`ムバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< LAppViewの隔つフレ�`ムバッファにレンダリング
    };

    /**
    * @brief コンストラクタ
    */
    View();

    /**
    * @brief デストラクタ
    */
    ~View();

    /**
    * @brief 兜豚晒する。
    */
    void Initialize();

    /**
    * @brief 宙鮫する。
    */
    void Render();

    /**
    * @brief 鮫�颪粒�豚晒を佩う。
    */
    void InitializeSprite();

    /**
    * @brief スプライト狼の�_慧
    */
    void ReleaseSprite();

    /**
    * @brief スプライト狼のサイズ壅�O協
    */
    void ResizeSprite();

    /**
    * @brief タッチされたときに柵ばれる。
    *
    * @param[in]       pointX            スクリ�`ンX恙��
    * @param[in]       pointY            スクリ�`ンY恙��
    */
    void OnTouchesBegan(float pointX, float pointY) const;

    /**
    * @brief タッチしているときにポインタが�咾い燭藝瑤个譴襦�
    *
    * @param[in]       pointX            スクリ�`ンX恙��
    * @param[in]       pointY            スクリ�`ンY恙��
    */
    void OnTouchesMoved(float pointX, float pointY) const;

    /**
    * @brief タッチが�K阻したら柵ばれる。
    *
    * @param[in]       pointX            スクリ�`ンX恙��
    * @param[in]       pointY            スクリ�`ンY恙��
    */
    void OnTouchesEnded(float pointX, float pointY) const;

    /**
    * @brief X恙�砲�View恙�砲����Qする。
    *
    * @param[in]       deviceX            デバイスX恙��
    */
    float TransformViewX(float deviceX) const;

    /**
    * @brief Y恙�砲�View恙�砲����Qする。
    *
    * @param[in]       deviceY            デバイスY恙��
    */
    float TransformViewY(float deviceY) const;

    /**
    * @brief X恙�砲�Screen恙�砲����Qする。
    *
    * @param[in]       deviceX            デバイスX恙��
    */
    float TransformScreenX(float deviceX) const;

    /**
    * @brief Y恙�砲�Screen恙�砲����Qする。
    *
    * @param[in]       deviceY            デバイスY恙��
    */
    float TransformScreenY(float deviceY) const;

    /**
     * @brief   モデル1悶を宙鮫する岷念にコ�`ルされる
     */
    void PreModelDraw(Model& refModel);

    /**
     * @brief   モデル1悶を宙鮫した岷瘁にコ�`ルされる
     */
    void PostModelDraw(Model& refModel);

    /**
     * @brief   �eレンダリングタ�`ゲットにモデルを宙鮫するサンプルで
     *           宙鮫�rのαを�Q協する
     */
    float GetSpriteAlpha(int assign) const;

    /**
     * @brief レンダリング枠を俳り紋える
     */
    void SwitchRenderingTarget(SelectTarget targetType);

    /**
     * @brief レンダリング枠をデフォルト參翌に俳り紋えた�Hの嘘尚クリア弼�O協
     * @param[in]   r   橿(0.0~1.0)
     * @param[in]   g   �v(0.0~1.0)
     * @param[in]   b   楳(0.0~1.0)
     */
    void SetRenderTargetClearColor(float r, float g, float b);

    /**
     * @brief   坪何レンダリングバッファの篤��
     */
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

