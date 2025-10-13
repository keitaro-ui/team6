#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"
#include <ProjectileManager.h>
#include "CameraController.h"
#include "System/AudioSource.h"

//�v���C���[
class Player : public Character
{
public:
    Player();
    ~Player() override;

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    //�f�o�b�O�pGUI�`��
    void DrawDebugGUI();

    //�W�����v���͏���
    void InputJump();

    //�f�o�b�O�v���~�e�B�u�`��
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

    bool GetShot() { return interval; }

protected:
    //���n�����Ƃ��ɌĂ΂��
    void OnLanding() override;


private:
    Model* model = nullptr;

    //�ړ����͏���
    void InputMove(float elapsedTime);

    //�e�ۓ��͏���
    void InputProjectile();

    //�X�e�B�b�N���͒l����ړ��x�N�g�����擾
    DirectX::XMFLOAT3 GetMoveVec() const;

    //�v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();

    //�e�ۂƓG�̏Փ˔���
    void CollisionProjectilesVsEnemies();

    //�}�E�X����
    void SStws();

    float moveSpeed = 5.0f;

    float turnSpeed = DirectX::XMConvertToRadians(720);

    float                jumpSpeed = 12.0f;

    int                    jumpCount = 0;
    int                    jumpLimit = 2;

    int shottimer = 0;

    ProjectileManager    projectileManager;

    float guntime = 1.5f;
    float v_guntime = 0.7f;

    AudioSource* hitSE = nullptr;
    AudioSource* shotSE = nullptr;
    AudioSource* takeSE = nullptr;

    float v_angle = 0;

public:
    CameraController* cameraController = nullptr;

    float recoiltimer = 3.0f;
    bool interval = true;
    bool vibe_interval = true;

    bool finish = false;
    //std::chrono::system_clock::time_point minutes;

    void coolgun(float elapsedTime);

};