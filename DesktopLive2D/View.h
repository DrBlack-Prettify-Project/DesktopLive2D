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
     * @brief LAppModel�Υ��������
     */
    enum SelectTarget
    {
        SelectTarget_None,                ///< �ǥե���ȤΥե�`��Хåե��˥������
        SelectTarget_ModelFrameBuffer,    ///< LAppModel�����Գ֤ĥե�`��Хåե��˥������
        SelectTarget_ViewFrameBuffer,     ///< LAppView�γ֤ĥե�`��Хåե��˥������
    };

    /**
    * @brief ���󥹥ȥ饯��
    */
    View();

    /**
    * @brief �ǥ��ȥ饯��
    */
    ~View();

    /**
    * @brief ���ڻ����롣
    */
    void Initialize();

    /**
    * @brief �軭���롣
    */
    void Render();

    /**
    * @brief ����γ��ڻ����Ф���
    */
    void InitializeSprite();

    /**
    * @brief ���ץ饤��ϵ���_��
    */
    void ReleaseSprite();

    /**
    * @brief ���ץ饤��ϵ�Υ��������O��
    */
    void ResizeSprite();

    /**
    * @brief ���å����줿�Ȥ��˺��Ф�롣
    *
    * @param[in]       pointX            ������`��X����
    * @param[in]       pointY            ������`��Y����
    */
    void OnTouchesBegan(float pointX, float pointY) const;

    /**
    * @brief ���å����Ƥ���Ȥ��˥ݥ��󥿤��Ӥ�������Ф�롣
    *
    * @param[in]       pointX            ������`��X����
    * @param[in]       pointY            ������`��Y����
    */
    void OnTouchesMoved(float pointX, float pointY) const;

    /**
    * @brief ���å����K�ˤ�������Ф�롣
    *
    * @param[in]       pointX            ������`��X����
    * @param[in]       pointY            ������`��Y����
    */
    void OnTouchesEnded(float pointX, float pointY) const;

    /**
    * @brief X���ˤ�View���ˤˉ�Q���롣
    *
    * @param[in]       deviceX            �ǥХ���X����
    */
    float TransformViewX(float deviceX) const;

    /**
    * @brief Y���ˤ�View���ˤˉ�Q���롣
    *
    * @param[in]       deviceY            �ǥХ���Y����
    */
    float TransformViewY(float deviceY) const;

    /**
    * @brief X���ˤ�Screen���ˤˉ�Q���롣
    *
    * @param[in]       deviceX            �ǥХ���X����
    */
    float TransformScreenX(float deviceX) const;

    /**
    * @brief Y���ˤ�Screen���ˤˉ�Q���롣
    *
    * @param[in]       deviceY            �ǥХ���Y����
    */
    float TransformScreenY(float deviceY) const;

    /**
     * @brief   ��ǥ�1����軭����ֱǰ�˥��`�뤵���
     */
    void PreModelDraw(Model& refModel);

    /**
     * @brief   ��ǥ�1����軭����ֱ��˥��`�뤵���
     */
    void PostModelDraw(Model& refModel);

    /**
     * @brief   �e������󥰥��`���åȤ˥�ǥ���軭���륵��ץ��
     *           �軭�r�Φ���Q������
     */
    float GetSpriteAlpha(int assign) const;

    /**
     * @brief ��������Ȥ��Ф��椨��
     */
    void SwitchRenderingTarget(SelectTarget targetType);

    /**
     * @brief ��������Ȥ�ǥե����������Ф��椨���H�α������ꥢɫ�O��
     * @param[in]   r   ��(0.0~1.0)
     * @param[in]   g   �v(0.0~1.0)
     * @param[in]   b   ��(0.0~1.0)
     */
    void SetRenderTargetClearColor(float r, float g, float b);

    /**
     * @brief   �ڲ�������󥰥Хåե����Ɨ�
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

