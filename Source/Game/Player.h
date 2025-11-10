#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"
#include <ProjectileManager.h>
#include "CameraController.h"
#include "System/AudioSource.h"
#include "SafetyArea.h"
#include <vector>

//プレイヤー
class Player : public Character
{
public:
    Player();
    ~Player() override;

    //更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    //デバッグ用GUI描画
    void DrawDebugGUI();

    //ジャンプ入力処理
    void InputJump();

    //デバッグプリミティブ描画
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

    bool GetShot() { return interval; }

protected:
    //着地したときに呼ばれる
    void OnLanding() override;


private:
    Model* model = nullptr;

    //移動入力処理
    void InputMove(float elapsedTime);

    //弾丸入力処理
    void InputProjectile();

    //スティック入力値から移動ベクトルを取得
    DirectX::XMFLOAT3 GetMoveVec() const;

    //プレイヤーとエネミーとの衝突処理
    void CollisionPlayerVsEnemies();

    //弾丸と敵の衝突判定
    void CollisionProjectilesVsEnemies();

    //マウス操作
    void SStws();

    void InputSafetrSrea();

    float moveSpeed = 5.0f;

    float turnSpeed = DirectX::XMConvertToRadians(720);

    float                jumpSpeed = 12.0f;

    int                    jumpCount = 0;
    int                    jumpLimit = 2;

    int shottimer = 0;

    //ProjectileManager    projectileManager;

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
   
private:
    std::vector<SafetyArea*> safetyAreas;

    //セーフティエリア制限個数
    int maxSafetyAreaCount = 5;

    //デバック表示
    bool shoeDebug = true;
    DirectX::XMFLOAT4 debugColor = { 1, 1, 0, 0.5f }; // 黄色っぽい半透明


    //デバックの半径
    float radius = 3.0f;
    //デバッグの高さ
    float height = 0.35f;
};