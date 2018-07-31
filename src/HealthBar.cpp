#include "stdafx.h"
#include "HealthBar.h"

boost::intrusive_ptr<HealthBar> HealthBar::Create()
{

	HealthBarPtr ret(new HealthBar);
	ret->Init();
	return ret;
}

void HealthBar::Init()
{
	Node::Init();
}

void HealthBar::DrawNode()
{

	Render::device.SetTexturing(false);

	Render::BeginColor(Color::RED);
	Render::DrawRect(IRect(0.0f, 0.0f, 100.0f * _value, 10.0f));
	Render::EndColor();

	Render::device.PushMatrix();
	Render::device.MatrixTranslate(_value * 100.0f, 0.0f, 0.0f);
	Render::BeginColor(Color(255, 255, 0));
	Render::DrawRect(IRect(0.0f, 0.0f, 100.0f * _track, 10.0f));
	Render::EndColor();
	Render::device.PopMatrix();

	Render::device.PushMatrix();
	Render::device.MatrixTranslate((_value + _track) * 100.0f, 0.0f, 0.0f);
	Render::BeginColor(Color(100, 100, 100));
	Render::DrawRect(IRect(0.0f, 0.0f, 100.0f * (1 - (_value + _track)), 10.0f));
	Render::EndColor();
	Render::device.PopMatrix();

	Render::device.SetTexturing(true);
}

void HealthBar::Update(float dt)
{
	if (_timer < 1) {
		_timer += dt;
	}
	else {
		_track -= dt;
		if (_track < 0.0f) {
			_track = 0.0f;
		}
	}
}

void HealthBar::Damage(float dh)
{
	_timer = 0.0f;
	_value -= dh;
	if (_value > 1.0f) {
		_value = 1.0f;
	}
	else if (_value < 0.0f) {
		_value = 0.0f;
	}
	_track += dh;
	if (_track > 1.0f) {
		_track = 1.0f;
	}
	else if (_track < 0.0f) {
		_track = 0.0f;
	}
}