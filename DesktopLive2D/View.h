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
     * @brief LAppModelのレンダリング先
     */
    enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< LAppModelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< LAppViewの持つフレームバッファにレンダリング
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
    * @brief 初期化する。
    */
    void Initialize();

    /**
    * @brief 描画する。
    */
    void Render();

    /**
    * @brief 画像の初期化を行う。
    */
    void InitializeSprite();

    /**
    * @brief スプライト系の開放
    */
    void ReleaseSprite();

    /**
    * @brief スプライト系のサイズ再設定
    */
    void ResizeSprite();

    /**
    * @brief タッチされたときに呼ばれる。
    *
    * @param[in]       pointX            スクリーンX座標
    * @param[in]       pointY            スクリーンY座標
    */
    void OnTouchesBegan(float pointX, float pointY) const;

    /**
    * @brief タッチしているときにポインタが動いたら呼ばれる。
    *
    * @param[in]       pointX            スクリーンX座標
    * @param[in]       pointY            スクリーンY座標
    */
    void OnTouchesMoved(float pointX, float pointY) const;

    /**
    * @brief タッチが終了したら呼ばれる。
    *
    * @param[in]       pointX            スクリーンX座標
    * @param[in]       pointY            スクリーンY座標
    */
    void OnTouchesEnded(float pointX, float pointY) const;

    /**
    * @brief X座標をView座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    float TransformViewX(float deviceX) const;

    /**
    * @brief Y座標をView座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    float TransformViewY(float deviceY) const;

    /**
    * @brief X座標をScreen座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    float TransformScreenX(float deviceX) const;

    /**
    * @brief Y座標をScreen座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    float TransformScreenY(float deviceY) const;

    /**
     * @brief   モデル1体を描画する直前にコールされる
     */
    void PreModelDraw(Model& refModel);

    /**
     * @brief   モデル1体を描画した直後にコールされる
     */
    void PostModelDraw(Model& refModel);

    /**
     * @brief   別レンダリングターゲットにモデルを描画するサンプルで
     *           描画時のαを決定する
     */
    float GetSpriteAlpha(int assign) const;

    /**
     * @brief レンダリング先を切り替える
     */
    void SwitchRenderingTarget(SelectTarget targetType);

    /**
     * @brief レンダリング先をデフォルト以外に切り替えた際の背景クリア色設定
     * @param[in]   r   赤(0.0~1.0)
     * @param[in]   g   緑(0.0~1.0)
     * @param[in]   b   青(0.0~1.0)
     */
    void SetRenderTargetClearColor(float r, float g, float b);

    /**
     * @brief   内部レンダリングバッファの破棄
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

