#include "PhysicsSystem2d.h"
#include "Game/EnemySlime.h"
#include <algorithm>
#include <vector>
inline float ProjectLength(const DirectX::XMFLOAT2& halfSize,
    const DirectX::XMFLOAT2 axis[2],
    const DirectX::XMFLOAT2& testAxis)
{
    // |halfW * dot(axisX, test)| + |halfH * dot(axisY, test)|
    float p0 = fabsf(halfSize.x * (axis[0].x * testAxis.x + axis[0].y * testAxis.y));
    float p1 = fabsf(halfSize.y * (axis[1].x * testAxis.x + axis[1].y * testAxis.y));
    return p0 + p1;
}

void PhysicsSystem2d::Update()
{
    // Box vs Box
    //for (auto& a : obbs)
    //{
    //    for (auto& b : obbs)
    //    {
    //        if (&a != &b && CheckOBB(a, b)) //OnCollision(a.entityID, b.entityID);
    //            ;
    //    }
    //}

    //// Circle vs Circle
    //for (auto& a : circleMgr->GetAll())
    //    for (auto& b : circleMgr->GetAll())
    //        if (&a != &b && CheckCircle2D(a, b)) //OnCollision(a.entityID, b.entityID);
                ;
    // Box vs Circle
    std::vector<Circle> circles;
    /*{
        Circle circle;
        circle.position = { 0,0,0 };
        circle.size = 0.5f;
        circles.push_back(circle);
    }*/

    for (auto& o : obbs)
    {
        for (auto& circle : circles)
        {
            SegmentHitResult hit;
            if (CheckCircleOBB(o, circle, hit))
            {
                DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&circle.position);
                DirectX::XMVECTOR Vec = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&hit.dir2d));
                Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorScale(Vec, circle.size - hit.distance));
                DirectX::XMStoreFloat3(&circle.position, Pos);
            }
        }
    } ;
}
//void PhysicsSystem2d::BuildProxies(Collider2dObb& out,BoxCollider2dData& bd) 
//{
//    using namespace DirectX;
//    if (bd.transformData == nullptr)return;
//    float c = cosf(bd.transformData->worldRotation.z);
//    float s = sinf(bd.transformData->worldRotation.z);
//
//    out.center = { bd.transformData->worldPosition.x + bd.offset.x, bd.transformData->worldPosition.y + bd.offset.y };
//    out.halfSize = { bd.halfSize.x * bd.transformData->worldScale.x, bd.halfSize.y * bd.transformData->worldScale.y };
//
//    // x軸
//    out.axis[0] = { c, s };
//    // y軸（x軸を +90度回転）
//    out.axis[1] = { -s, c };
//}
//
//bool PhysicsSystem2d::CheckOBB(const Collider2dObb& A, const Collider2dObb& B)
//{
//    using namespace DirectX;
//
//    // 中心差分
//    XMFLOAT2 d = { B.center.x - A.center.x, B.center.y - A.center.y };
//
//    // テストする軸（4つ）
//    XMFLOAT2 testAxis[4] = {
//        A.axis[0],
//        A.axis[1],
//        B.axis[0],
//        B.axis[1]
//    };
//
//    for (int i = 0; i < 4; i++)
//    {
//        float ax = testAxis[i].x;
//        float ay = testAxis[i].y;
//
//        // Aの投影幅
//        float projA = ProjectLength(A.halfSize, A.axis, testAxis[i]);
//        // Bの投影幅
//        float projB = ProjectLength(B.halfSize, B.axis, testAxis[i]);
//
//        // 中心距離の軸方向絶対値
//        float dist = fabsf(d.x * ax + d.y * ay);
//
//        // 分離していたら衝突していない
//        if (dist > projA + projB)
//            return false;
//    }
//
//    return true; // 全ての軸で重なっている
//}

bool PhysicsSystem2d::CheckCircleOBB(const Collider2dObb& A, const Circle& B, SegmentHitResult& hitResult)
{
    using namespace DirectX;

    float radius = B.size;
    XMVECTOR p = XMVectorSet(B.position.x, B.position.z, 0, 0);

    float bestPenetration = -FLT_MAX;
    bool hit = false;
    SegmentHitResult bestHit{};

    auto testSegment = [&](int idx1, int idx2)
        {
            XMVECTOR a = XMLoadFloat2(&A.vertexPos[idx1]);
            XMVECTOR b = XMLoadFloat2(&A.vertexPos[idx2]);

            SegmentHitResult r = ClosestPointOnSegment(p, a, b);
            float dist = r.distance;

            if (dist < radius)
            {
                float penetration = radius - dist;
                //EnemySlime::Instance(). OnHitWaypoint();
                if (penetration > bestPenetration)
                {
                    bestPenetration = penetration;
                    bestHit = r;
                    hit = true;
                }
            }
        };

    // 4 辺を doorNum に応じてテスト
    // 各辺は「閉まっている辺か」「開いている辺＋補助セグメントか」を判定する

    for (int i = 0; i < 4; i++)
    {
        int next = (i + 1) % 4;
        
        testSegment(i, next);
    }

    if (hit)
    {
        hitResult = bestHit;
        return true;
    }

    return false;
}

bool PhysicsSystem2d::CheckCircleVsDoor(const door& A, const Circle& B, SegmentHitResult& hitResult)
{
    using namespace DirectX;

    float radius = B.size;
    XMVECTOR p = XMVectorSet(B.position.x, B.position.z, 0, 0);

    float bestPenetration = -FLT_MAX;
    bool hit = false;
    SegmentHitResult bestHit{};

    auto testSegment = [&](int idx1, int idx2)
        {
            XMVECTOR a = XMLoadFloat2(&A.vertexPos[idx1]);
            XMVECTOR b = XMLoadFloat2(&A.vertexPos[idx2]);

            SegmentHitResult r = ClosestPointOnSegment(p, a, b);
            float dist = r.distance;

            if (dist < radius)
            {
                float penetration = radius - dist;
                if (penetration > bestPenetration)
                {
                    bestPenetration = penetration;
                    bestHit = r;
                    hit = true;
                }
            }
        };

    // 4 辺を doorNum に応じてテスト
    // 各辺は「閉まっている辺か」「開いている辺＋補助セグメントか」を判定する

    for (int i = 0; i < 4; i++)
    {
        int next = (i + 1) % 4;

        if (A.doorNum == i)
        {
            // 扉が開くとき、２つの候補ラインをチェック
            testSegment(i, 4);      // main 開き側
            testSegment(5, next);   // 補助ライン
        }
        else
        {
            // 通常の矩形の辺
            testSegment(i, next);
        }
    }

    if (hit)
    {
        hitResult = bestHit;
        return true;
    }

    return false;
}

bool PhysicsSystem2d::IsInside(const door& d, const DirectX::XMFLOAT2& p)
{
    DirectX::XMVECTOR P = XMLoadFloat2(&p);
    DirectX::XMVECTOR C = XMLoadFloat2(&d.center);

    DirectX::XMVECTOR dVec = DirectX::XMVectorSubtract(P, C);

    DirectX::XMVECTOR ax = XMLoadFloat2(&d.axis[0]);
    DirectX::XMVECTOR ay = XMLoadFloat2(&d.axis[1]);

    float x = DirectX::XMVectorGetX(DirectX::XMVector2Dot(dVec, ax));
    float y = DirectX::XMVectorGetX(DirectX::XMVector2Dot(dVec, ay));

    return fabsf(x) <= d.halfSize.x && fabsf(y) <= d.halfSize.y;
}

bool PhysicsSystem2d::CheckCircle2D(
    Circle c1,
    Circle c2)

{
    float dx = (c2.position.x) - (c1.position.x);
    float dy = (c2.position.z) - (c1.position.z);

    float distSq = dx * dx + dy * dy;
    float radiusSum = c1.size + c2.size;

    return distSq <= radiusSum * radiusSum;
}

//playerとステージの当たり判定。多分一回呼ぶのでいける
DirectX::XMFLOAT3 PhysicsSystem2d::CircleVsStage(DirectX::XMFLOAT3 pos, float size)
{
    Circle circle;
    circle.position = pos;
    circle.size = size;
    for (auto& o : obbs)
    {
        SegmentHitResult hit;
        if (CheckCircleOBB(o, circle, hit))
        {
            // circle.position の XZ を 2D の XY (x,y) として使用する
            DirectX::XMFLOAT2 pos2d = { circle.position.x, circle.position.z };

            // 2Dベクトルをロード
            DirectX::XMVECTOR Pos = DirectX::XMLoadFloat2(&pos2d);
            DirectX::XMVECTOR Dir = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&hit.dir2d));

            // penetration 分押し出し (circle.size - hit.distance)
            float push = circle.size - hit.distance;
            Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorScale(Dir, push));

            // 結果を書き戻す
            DirectX::XMStoreFloat2(&pos2d, Pos);

            

            // y はそのまま、XZ だけ更新
            circle.position = { pos2d.x, circle.position.y, pos2d.y };
        }
    };

    for (auto& o : doorObbs)
    {
        SegmentHitResult hit;
        if (CheckCircleVsDoor(o, circle, hit))
        {
            // circle.position の XZ を 2D の XY (x,y) として使用する
            DirectX::XMFLOAT2 pos2d = { circle.position.x, circle.position.z };

            // 2Dベクトルをロード
            DirectX::XMVECTOR Pos = DirectX::XMLoadFloat2(&pos2d);
            DirectX::XMVECTOR Dir = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&hit.dir2d));

            // penetration 分押し出し (circle.size - hit.distance)
            float push = circle.size - hit.distance;
            Pos = DirectX::XMVectorAdd(Pos, DirectX::XMVectorScale(Dir, push));

            // 結果を書き戻す
            DirectX::XMStoreFloat2(&pos2d, Pos);

            // y はそのまま、XZ だけ更新
            circle.position = { pos2d.x, circle.position.y, pos2d.y };
        }
    };
    return circle.position;
}

// 点 p と線分 ab の情報を返す
PhysicsSystem2d::SegmentHitResult PhysicsSystem2d::ClosestPointOnSegment(
    const DirectX::XMVECTOR& p,
    const DirectX::XMVECTOR& a,
    const DirectX::XMVECTOR& b)
{
    using namespace DirectX;

    SegmentHitResult r{};

    XMVECTOR ab = b - a;
    XMVECTOR ap = p - a;

    // |ab|^2
    float abLenSq = XMVectorGetX(XMVector3Dot(ab, ab));

    // a==b（線分退化）
    if (abLenSq <= 1e-8f)
    {
        r.t = 0.0f;
        XMStoreFloat2(&r.point, a);

        XMVECTOR diff = p - a;
        r.distance = XMVectorGetX(XMVector3Length(diff));

        // 方向ベクトル
        DirectX::XMVECTOR dirN = XMVector2Normalize(diff);
        DirectX::XMFLOAT2 d;
        XMStoreFloat2(&d, dirN);
        r.dir2d = d;

        return r;
    }

    // -------------------------
     // 線分退化 a==b
     // -------------------------
    if (abLenSq <= 1e-8f)
    {
        r.t = 0.0f;
        XMStoreFloat2(&r.point, a);

        XMVECTOR diff = p - a;
        float dist = XMVectorGetX(XMVector3Length(diff));
        r.distance = dist;

        // XY だけ取り出して正規化
        XMVECTOR diff2d = XMVectorSet(XMVectorGetX(diff), XMVectorGetY(diff), 0.0f, 0.0f);

        if (dist > 1e-8f)
        {
            diff2d = XMVector2Normalize(diff2d);
        }
        else
        {
            diff2d = XMVectorZero();
        }

        XMStoreFloat2(&r.dir2d, diff2d);
        return r;
    }

    // -------------------------
    // 無限直線の射影係数 t
    // -------------------------
    float t = XMVectorGetX(XMVector3Dot(ap, ab)) / abLenSq;
    t = std::clamp(t, 0.0f, 1.0f);
    r.t = t;

    // 最近接点 = a + ab * t
    XMVECTOR point = XMVectorMultiplyAdd(ab, XMVectorReplicate(t), a);

    XMFLOAT2 point2;
    XMStoreFloat2(&point2, point);
    r.point = point2;

    // 距離
    XMVECTOR diff = p - point;
    float dist = XMVectorGetX(XMVector3Length(diff));
    r.distance = dist;

    // -------------------------
    // XY のみで方向ベクトル作る（正規化安全化）
    // -------------------------
    XMVECTOR diff2d = XMVectorSet(XMVectorGetX(diff), XMVectorGetY(diff), 0.0f, 0.0f);

    if (dist > 1e-8f)
    {
        diff2d = XMVector2Normalize(diff2d);
    }
    else
    {
        diff2d = XMVectorZero();
    }

    XMStoreFloat2(&r.dir2d, diff2d);

    return r;
}

//関数呼び出すと、この中で四角の当たり判定作って登録してる。四角ごとに呼び出してあげないといかん
void PhysicsSystem2d::AddObb(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size, float ang)
{
    //centerとhalfsizeはxyにxzをいれる。
    Collider2dObb obb;
    DirectX::XMFLOAT2 center2D = { center.x,center.z };
    DirectX::XMFLOAT2 halfSize = { size.x,size.z };
    float c = cosf(ang);
    float s = sinf(ang);

    DirectX::XMFLOAT2 axis0 = { c, s };
    // y軸（x軸を +90度回転）
    DirectX::XMFLOAT2 axis1 = { -s, c };

    DirectX::XMVECTOR cVec = DirectX::XMLoadFloat2(&center2D);
    DirectX::XMVECTOR ax = DirectX::XMLoadFloat2(&axis0);
    DirectX::XMVECTOR ay = DirectX::XMLoadFloat2(&axis1);

    DirectX::XMVECTOR hx = DirectX::XMVectorScale(ax, halfSize.x);
    DirectX::XMVECTOR hy = DirectX::XMVectorScale(ay, halfSize.y);

    obb.vertexPos.resize(4);
    // 4 corner points
     DirectX::XMStoreFloat2(&obb.vertexPos[0],DirectX::XMVectorAdd(DirectX::XMVectorAdd(cVec, hx), hy));
     DirectX::XMStoreFloat2(&obb.vertexPos[1],DirectX::XMVectorAdd(DirectX::XMVectorSubtract(cVec, hx), hy));
     DirectX::XMStoreFloat2(&obb.vertexPos[2],DirectX::XMVectorSubtract(DirectX::XMVectorSubtract(cVec, hx), hy));
     DirectX::XMStoreFloat2(&obb.vertexPos[3],DirectX::XMVectorSubtract(DirectX::XMVectorAdd(cVec, hx), hy));

    obbs.push_back(obb);
}

//doorDir:XZを{1,0,0}で右方向の辺、{0,0,1}で上方向なはず… doorDistで空白が始まる位置を決める。はず…
void PhysicsSystem2d::AddDoorObb(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size, float ang, DirectX::XMFLOAT3 doorDir,
	float doorSize,float doorDist)
{
    door door;
    //centerとhalfsizeはxyにxzをいれる。
    DirectX::XMFLOAT2 center2D = { center.x,center.z };
    DirectX::XMFLOAT2 halfSize = { size.x,size.z };
    float c = cosf(ang);
    float s = sinf(ang);

    DirectX::XMFLOAT2 axis0 = { c, s };
    // y軸（x軸を +90度回転）
    DirectX::XMFLOAT2 axis1 = { -s, c };

    DirectX::XMVECTOR cVec = DirectX::XMLoadFloat2(&center2D);
    DirectX::XMVECTOR ax = DirectX::XMLoadFloat2(&axis0);
    DirectX::XMVECTOR ay = DirectX::XMLoadFloat2(&axis1);

    DirectX::XMVECTOR hx = DirectX::XMVectorScale(ax, halfSize.x);
    DirectX::XMVECTOR hy = DirectX::XMVectorScale(ay, halfSize.y);
    door.vertexPos.resize(6);

    DirectX::XMVECTOR c0 = DirectX::XMVectorAdd(DirectX::XMVectorAdd(cVec, hx), hy);
    DirectX::XMVECTOR c1 = DirectX::XMVectorAdd(DirectX::XMVectorSubtract(cVec, hx), hy);
    DirectX::XMVECTOR c2 = DirectX::XMVectorSubtract(DirectX::XMVectorSubtract(cVec, hx), hy);
    DirectX::XMVECTOR c3 = DirectX::XMVectorSubtract(DirectX::XMVectorAdd(cVec, hx), hy);
    // 4 corner points
    DirectX::XMStoreFloat2(&door.vertexPos[0], c0);
    DirectX::XMStoreFloat2(&door.vertexPos[1], c1);
    DirectX::XMStoreFloat2(&door.vertexPos[2], c2);
    DirectX::XMStoreFloat2(&door.vertexPos[3], c3);
    if (doorDir.x > 0.1f)
    {
        door.doorNum = 3;
        DirectX::XMVECTOR lineDir = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(c0, c3));
        DirectX::XMStoreFloat2(&door.vertexPos[4], DirectX::XMVectorAdd(c3, DirectX::XMVectorScale(lineDir, doorDist)));
        DirectX::XMStoreFloat2(&door.vertexPos[5], DirectX::XMVectorAdd(c3, DirectX::XMVectorScale(lineDir, doorDist + doorSize)));
    }
    else if (doorDir.z > 0.1f)
    {
        door.doorNum = 0;
        DirectX::XMVECTOR lineDir = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(c1, c0));
        DirectX::XMStoreFloat2(&door.vertexPos[4], DirectX::XMVectorAdd(c0, DirectX::XMVectorScale(lineDir, doorDist)));
        DirectX::XMStoreFloat2(&door.vertexPos[5], DirectX::XMVectorAdd(c0, DirectX::XMVectorScale(lineDir, doorDist + doorSize)));

    }
    else if (doorDir.x < -0.1f)
    {
        door.doorNum = 1;
        DirectX::XMVECTOR lineDir = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(c2, c1));
        DirectX::XMStoreFloat2(&door.vertexPos[4], DirectX::XMVectorAdd(c1, DirectX::XMVectorScale(lineDir, doorDist)));
        DirectX::XMStoreFloat2(&door.vertexPos[5], DirectX::XMVectorAdd(c1, DirectX::XMVectorScale(lineDir, doorDist + doorSize)));

    }
    else
    {
        door.doorNum = 2;
        DirectX::XMVECTOR lineDir = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(c3, c2));
        DirectX::XMStoreFloat2(&door.vertexPos[4], DirectX::XMVectorAdd(c2, DirectX::XMVectorScale(lineDir, doorDist)));
        DirectX::XMStoreFloat2(&door.vertexPos[5], DirectX::XMVectorAdd(c2, DirectX::XMVectorScale(lineDir, doorDist + doorSize)));

    }

    //内側判定の中心＋軸＋半サイズを保存
    door.center = center2D;
    door.axis[0] = axis0;
    door.axis[1] = axis1;
    door.halfSize = halfSize;

    doorObbs.push_back(door);
}

void PhysicsSystem2d::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    for (auto& o:obbs)
    {
        DirectX::XMFLOAT3 v0 = { o.vertexPos[0].x,1.0f,o.vertexPos[0].y };
        DirectX::XMFLOAT3 v1 = { o.vertexPos[1].x,1.0f,o.vertexPos[1].y };
        DirectX::XMFLOAT3 v2 = { o.vertexPos[2].x,1.0f,o.vertexPos[2].y };
        DirectX::XMFLOAT3 v3 = { o.vertexPos[3].x,1.0f,o.vertexPos[3].y };
        renderer->RenderLine(rc, v0, v1, { 1,0,0,1 });
        renderer->RenderLine(rc, v1, v2, { 1,0,0,1 });
        renderer->RenderLine(rc, v2, v3, { 1,0,0,1 });
        renderer->RenderLine(rc, v3, v0, { 1,0,0,1 });
    }
    for (auto& o:doorObbs)
    {
        DirectX::XMFLOAT3 v0 = { o.vertexPos[0].x,1.0f,o.vertexPos[0].y };
        DirectX::XMFLOAT3 v1 = { o.vertexPos[1].x,1.0f,o.vertexPos[1].y };
        DirectX::XMFLOAT3 v2 = { o.vertexPos[2].x,1.0f,o.vertexPos[2].y };
        DirectX::XMFLOAT3 v3 = { o.vertexPos[3].x,1.0f,o.vertexPos[3].y };
        DirectX::XMFLOAT3 v4 = { o.vertexPos[4].x,1.0f,o.vertexPos[4].y };
        DirectX::XMFLOAT3 v5 = { o.vertexPos[5].x,1.0f,o.vertexPos[5].y };

        if (o.doorNum == 0)
        {
            renderer->RenderLine(rc, v0, v4, { 1,0,0,1 });
            renderer->RenderLine(rc, v5, v1, { 1,0,0,1 });
        }
        else
        {
	        renderer->RenderLine(rc, v0, v1, { 1,0,0,1 });
        }
        if (o.doorNum == 1)
        {

            renderer->RenderLine(rc, v1, v4, { 1,0,0,1 });
            renderer->RenderLine(rc, v5, v2, { 1,0,0,1 });
        }
    	else
    	{
    		renderer->RenderLine(rc, v1, v2, { 1,0,0,1 });
    	}
        if (o.doorNum == 2)
        {

            renderer->RenderLine(rc, v2, v4, { 1,0,0,1 });
            renderer->RenderLine(rc, v5, v3, { 1,0,0,1 });
        }
    	else
    	{
    		renderer->RenderLine(rc, v2, v3, { 1,0,0,1 });
    	}
        if (o.doorNum == 3)
        {

            renderer->RenderLine(rc, v3, v4, { 1,0,0,1 });
            renderer->RenderLine(rc, v5, v0, { 1,0,0,1 });
        }
    	else
    	{
    		renderer->RenderLine(rc, v3, v0, { 1,0,0,1 });
    	}
    }
}
