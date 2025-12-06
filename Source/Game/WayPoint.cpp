#include "WayPoint.h"
#include <System/RenderContext.h>
#include <System/ModelRenderer.h>
#include "System/Graphics.h"

WayPoint::WayPoint()
{
	//position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < EdgeNo; ++i)
	{
		edge[i] = nullptr;
	}
	shape = std::make_unique<ShapeRenderer>(Graphics::Instance().GetDevice());
}
WayPoint::~WayPoint()
{
	for (int i = 0; i < EdgeNo; ++i)
	{
		if (edge[i] != nullptr)
			delete edge[i];
	}
}

//•`‰æˆ—
void WayPoint::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	shape->RenderSphere(rc, position, 0.5f, { 1,1,1,1 });
}