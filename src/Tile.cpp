#include "stdafx.h"
#include "Tile.h"

TilePtr Tile::Create(Area area) {
	TilePtr ptr(new Tile);
	ptr->Init(area);
	return ptr;
}

void Tile::Init(Area area) {
	if (_eff) {
		_eff->Finish();
	}

	SetTexture(area.texture);
	if (area.effect != "None") {
		_eff = _effCont.AddEffect(area.effect);
		float rand = (float)(std::rand() % 100) / 10.f;
		_eff->Update(rand);
	}
}

void Tile::Draw() {
	Sprite::Draw();

	Render::device.PushMatrix();

	Render::device.MatrixTranslate(_position);
	Render::device.MatrixRotate(math::Vector3(1, 0, 0), _rotation.x);
	Render::device.MatrixRotate(math::Vector3(0, 1, 0), _rotation.y);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _rotation.z);

	_effCont.Draw();

	Render::device.PopMatrix();

}

void Tile::Update(float dt) {
	_effCont.Update(dt);
}