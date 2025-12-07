#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"
#include <ProjectileManager.h>
#include "CameraController.h"
#include "System/AudioSource.h"
#include "SafetyArea.h"
#include <vector>
#include "SoundManager.h"


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

    float maxHP = 100.0f;
    float hp = 100.0f;

    void AddDamage(float amount);

    void Heal(float amount);

    DirectX::XMFLOAT3 GetPosition() const { return position; }


    void coolgun(float elapsedTime);

    float GetMaxSafeAreaCount() { return maxSafetyAreaCount; }
    bool GetputTrue() { return putTrue; }
    int GetLastSafetyAreaIndex() const { return lastSafetyAreaIndex; }
    float GethitRadius() { return hitRadius; }

    void FillSafetyAreaPosition(std::vector<DirectX::XMFLOAT4>& outPositions) const;

    const std::vector<SafetyArea*>& GetSafety()const { return safetyAreas; }

    void SetRenderer(ModelRenderer* r) { renderer = r; }
   
    std::vector<SafetyArea*> safetyAreas;

    std::vector<DirectX::XMFLOAT4> saPositions;
    ModelRenderer* renderer = nullptr;

    //inside判定
    void SetPlayerInside(bool v) { playerInside = v; }
    bool GetPlayerInside() const { return playerInside; }
private:

    SafetyArea* area;
    //セーフティエリア制限個数
    int maxSafetyAreaCount = 5;

    //デバック表示
    bool shoeDebug = true;
    DirectX::XMFLOAT4 debugColor = { 1, 1, 0, 0.5f }; // 黄色っぽい半透明

    //デバックの半径
    float radius = 2.0f;

    //デバッグの高さ
    float height = 0.35f;

    //当たり判定の大きさ
    float hitRadius = 0.55f;

    float safeCooldown = 0.0f;        // 現在のクールタイム残り時間
    bool canPlaceSafeArea = true;     // 設置可能かどうか
    float safeInterval = 2.0f;

    int lastSafetyAreaIndex = -1;

    bool putTrue = false;

    bool playerInside;
};