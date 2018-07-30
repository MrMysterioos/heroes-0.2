#include "stdafx.h"
#include "Sprite.h"
#include "Scene.h"

boost::intrusive_ptr<Sprite> Sprite::Create(Scene* scene)
{
	SpritePtr ret(new Sprite);
	ret->Init(scene);
	return SpritePtr(new Sprite);
}

void Sprite::Init(Scene * scene)
{
	Node::Init(scene);
}

void Sprite::DrawNode()
{
	Render::device.PushMatrix();

	FPoint offset = FPoint(_texture->Width() * _anchorPoint.x, _texture->Height() * _anchorPoint.y);
	Render::device.MatrixTranslate(-offset);
	
	Render::BeginColor(_color);
	_texture->Draw();
	Render::EndColor();

	Render::device.PopMatrix();
}