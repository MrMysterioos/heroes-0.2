#include "stdafx.h"
#include "HealthBar.h"

boost::intrusive_ptr<HealthBar> HealthBar::Create(Scene* scene)
{

	HealthBarPtr ret(new HealthBar);
	ret->Init(scene);
	return ret;
}

void HealthBar::Init(Scene* scene)
{
	Node::Init(scene);
}

void HealthBar::DrawNode()
{

	Render::device.SetTexturing(false);
	Render::BeginColor(Color::GREEN);
	Render::DrawRect(IRect(0.0f, 0.0f, 100.0f, 10.0f));
	Render::EndColor();
	Render::device.SetTexturing(true);
}