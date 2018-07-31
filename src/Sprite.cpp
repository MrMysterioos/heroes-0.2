#include "stdafx.h"
#include "Sprite.h"
#include "Scene.h"

boost::intrusive_ptr<Sprite> Sprite::Create()
{
	SpritePtr ret(new Sprite);
	ret->Init();
	return ret;
}

void Sprite::Init()
{
	Node::Init();
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