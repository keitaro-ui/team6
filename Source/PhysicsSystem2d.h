#pragma once
#include <vector>

#include"Game/Stage.h"

#include "DirectXMath.h"
#include "System/ShapeRenderer.h"

class PhysicsSystem2d
{
public:

    void Update();


private:
    // プロキシ（型をまたいで１つの配列にまとめる構造体）
    struct Collider2dObb {
        std::vector<DirectX::XMFLOAT2>vertexPos;
    };

    struct door
    {
        std::vector<DirectX::XMFLOAT2>vertexPos;
        int doorNum = -1;

        //内側判定の変数
        DirectX::XMFLOAT2 center;
        DirectX::XMFLOAT2 axis[2];
        DirectX::XMFLOAT2 halfSize;
    };

    struct Circle
    {
        DirectX::XMFLOAT3 position;
        float size;
    };
    struct SegmentHitResult
    {
        float distance;        // 点→線分の最短距離
        float t;               // 線分上の最近接位置
        DirectX::XMFLOAT2 point; // 最近接点
        DirectX::XMFLOAT2 dir2d;  // (sign, absCrossValue)
    };
  /*  void BuildProxies(
        Collider2dObb& out,
        BoxCollider2dData& bd);*/

    std::vector<Collider2dObb> obbs;
    std::vector<door> doorObbs;

public:
    //視覚同士の当たり判定を使う場合この下の関数とCPPがわののコメントを外して下さい
   /* bool CheckOBB(const Collider2dObb& A, const Collider2dObb& B);*/
    bool CheckCircle2D(Circle c1, Circle c2);
    bool CheckCircleOBB(const Collider2dObb& A, const Circle& B, SegmentHitResult& hitResult);
    bool CheckCircleVsDoor(const door& A, const Circle& B, SegmentHitResult& hitResult);
    bool IsInside(const door& d, const DirectX::XMFLOAT2& p);

	DirectX::XMFLOAT3 PhysicsSystem2d::CircleVsDoor(DirectX::XMFLOAT3 pos, float size);
    DirectX::XMFLOAT3 CircleVsStage(DirectX::XMFLOAT3 pos, float size);
    SegmentHitResult ClosestPointOnSegment(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b);
    void AddObb(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size, float ang); //sizeはy=0でxz二つのみで計算させる
    void AddDoorObb(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size, float ang, DirectX::XMFLOAT3 doorDir, float doorSize,float doorDist);

    //内部判定で使うゲッター
    const std::vector<door>& GetDoorObbs() const { return doorObbs; }

    //デバッグプリミティブ描画
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

};
