#include "stdafx.h"
#include "Sprite.h"

boost::intrusive_ptr<Sprite> Sprite::Create()
{
	return SpritePtr(new Sprite);
}

void Sprite::Draw()
{
	Render::device.PushMatrix();

	Render::device.MatrixTranslate(_position);
	Render::device.MatrixRotate(math::Vector3(1, 0, 0), _rotation.x);
	Render::device.MatrixRotate(math::Vector3(0, 1, 0), _rotation.y);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _rotation.z);

	FPoint offset = FPoint(_texture->Width() * _anchorPoint.x, _texture->Height() * _anchorPoint.y);
	Render::device.MatrixTranslate(-offset);
	
	Render::BeginColor(_color);
	_texture->Draw();
	Render::EndColor();

	Render::device.PopMatrix();
}